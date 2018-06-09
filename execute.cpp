#include "thumbsim.hpp"
// These are just the register NUMBERS
#define PC_REG 15  
#define LR_REG 14
#define SP_REG 13

// These are the contents of those registers
#define PC rf[PC_REG]
#define LR rf[LR_REG]
#define SP rf[SP_REG]

Stats stats;
Caches caches(0);

// CPE 315: you'll need to implement a custom sign-extension function
// in addition to the ones given below, specifically for the unconditional
// branch instruction, which has an 11-bit immediate field
unsigned int signExtend16to32ui(short i) {
  return static_cast<unsigned int>(static_cast<int>(i));
}

unsigned int signExtend8to32ui(char i) {
  return static_cast<unsigned int>(static_cast<int>(i));
}

unsigned int signExtend11to32ui(int i) {
   return static_cast<unsigned int>(static_cast<int>(i));
}

// This is the global object you'll use to store condition codes N,Z,V,C
// Set these bits appropriately in execute below.
ASPR flags;

// CPE 315: You need to implement a function to set the Negative and Zero
// flags for each instruction that does that. It only needs to take
// one parameter as input, the result of whatever operation is executing
void setNegativeZero(int x) {
   if (x == 0)
      flags.Z = 1;
   else
      flags.Z = 0;
   if (x < 0)
      flags.N = 1;
   else
      flags.N = 0;
}

// This function is complete, you should not have to modify it
void setCarryOverflow (int num1, int num2, OFType oftype) {
  switch (oftype) {
    case OF_ADD:
      if (((unsigned long long int)num1 + (unsigned long long int)num2) ==
          ((unsigned int)num1 + (unsigned int)num2)) {
        flags.C = 0;
      }
      else {
        flags.C = 1;
      }
      if (((long long int)num1 + (long long int)num2) ==
          ((int)num1 + (int)num2)) {
        flags.V = 0;
      }
      else {
        flags.V = 1;
      }
      break;
    case OF_SUB:
      if (num1 >= num2) {
        flags.C = 1;
      }
      else if (((unsigned long long int)num1 - (unsigned long long int)num2) ==
          ((unsigned int)num1 - (unsigned int)num2)) {
        flags.C = 0;
      }
      else {
        flags.C = 1;
      }
      if (((num1==0) && (num2==0)) ||
          (((long long int)num1 - (long long int)num2) ==
           ((int)num1 - (int)num2))) {
        flags.V = 0;
      }
      else {
        flags.V = 1;
      }
      break;
    case OF_SHIFT:
      // C flag unaffected for shifts by zero
      if (num2 != 0) {
        if (((unsigned long long int)num1 << (unsigned long long int)num2) ==
            ((unsigned int)num1 << (unsigned int)num2)) {
          flags.C = 0;
        }
        else {
          flags.C = 1;
        }
      }
      // Shift doesn't set overflow
      break;
    default:
      cerr << "Bad OverFlow Type encountered." << __LINE__ << __FILE__ << endl;
      exit(1);
  }
}

// CPE 315: You're given the code for evaluating BEQ, and you'll need to 
// complete the rest of these conditions. See Page 208 of the armv7 manual
// 
// *** Filling it in with data from p208
static int checkCondition(unsigned short cond) {
  switch(cond) {
    case EQ:
      if (flags.Z == 1) {
        return TRUE;
      }
      break;
    case NE:
      if (flags.Z == 0) {
         return TRUE;
      }
      break;
    case CS:
      if (flags.C == 1) {
         return TRUE;
      }
      break;
    case CC:
      if (flags.C == 0) {
         return TRUE;
      }
      break;
    case MI:
      if (flags.N == 1) {
         return TRUE;
      }
      break;
    case PL:
      if (flags.N == 0) {
         return TRUE;
      }
      break;
    case VS:
      if (flags.V == 1) {
         return TRUE;
      }
      break;
    case VC:
      if (flags.V == 0) {
         return TRUE;
      }
      break;
    case HI:
      if (flags.C == 1 && flags.Z == 0) {
         return TRUE;
      }
      break;
    case LS:
      if (flags.C == 0 || flags.Z == 1) {
         return TRUE;
      }
      break;
    case GE:
      if (flags.N == flags.V) {
         return TRUE;
      }
      break;
    case LT:
      if (flags.N != flags.V) {
         return TRUE;
      }
      break;
    case GT:
      if (flags.Z == 0 && flags.N == flags.V) {
         return TRUE;
      }
      break;
    case LE:
      if (flags.Z == 1 || flags.N != flags.V) {
         return TRUE;
      }
      break;
    case AL:
      return TRUE;
      break;
  }
  return FALSE;
}

int count_bits(unsigned int reg_list, unsigned int reg_lr) {
   int bits = 0;

   for (int i = 1; i <= 128; i *= 2) {
      if (reg_list & i)
         bits++;
   }

   if (reg_lr)
      bits++;

   return bits;
}

void execute() {
  Data16 instr = imem[PC];
  Data16 instr2;
  Data32 temp(0); // Use this for STRB instructions
  Thumb_Types itype;
  // the following counts as a read to PC
  unsigned int pctarget = PC + 2;
  stats.numRegReads++;
  unsigned int addr;
  int i, n, offset;
  unsigned int list, mask;
  int num1, num2, result, BitCount;
  unsigned int bit;
  int direction = 0;

  /* Convert instruction to correct type */
  /* Types are described in Section A5 of the armv7 manual */
  BL_Type blupper(instr);
  ALU_Type alu(instr);
  SP_Type sp(instr);
  DP_Type dp(instr);
  LD_ST_Type ld_st(instr);
  MISC_Type misc(instr);
  COND_Type cond(instr);
  UNCOND_Type uncond(instr);
  LDM_Type ldm(instr);
  STM_Type stm(instr);
  LDRL_Type ldrl(instr);
  ADD_SP_Type addsp(instr);

  BL_Ops bl_ops;
  ALU_Ops add_ops;
  DP_Ops dp_ops;
  SP_Ops sp_ops;
  LD_ST_Ops ldst_ops;
  MISC_Ops misc_ops;

  // This counts as a write to the PC register
  rf.write(PC_REG, pctarget);
  stats.numRegWrites++;
  itype = decode(ALL_Types(instr)); // This got taken out?

  // CPE 315: The bulk of your work is in the following switch statement
  // All instructions will need to have stats and cache access info added
  // as appropriate for that instruction.
  switch(itype) {
    case ALU:
      add_ops = decode(alu);
      switch(add_ops) {
        case ALU_LSLI:
          // Done
          rf.write(alu.instr.lsli.rd, rf[alu.instr.lsli.rm] << alu.instr.lsli.imm);
          setCarryOverflow(rf[alu.instr.lsli.rm], alu.instr.lsli.imm, OF_SHIFT);
          setNegativeZero(rf[alu.instr.lsli.rd]);
          stats.numRegWrites++;
          stats.numRegReads++;
          stats.instrs++;
          break;
        case ALU_ADDR:
          // Done
          rf.write(alu.instr.addr.rd, rf[alu.instr.addr.rn] + rf[alu.instr.addr.rm]);
          setCarryOverflow(rf[alu.instr.addr.rn], rf[alu.instr.addr.rm], OF_ADD);
          setNegativeZero(rf[alu.instr.addr.rn] + rf[alu.instr.addr.rm]);
          stats.numRegWrites++;
          stats.numRegReads += 2;
          stats.instrs++;
          break;
        case ALU_SUBR:
          // Done
          rf.write(alu.instr.subr.rd, rf[alu.instr.subr.rn] - rf[alu.instr.subr.rm]);
          setCarryOverflow(rf[alu.instr.subr.rn], rf[alu.instr.subr.rm], OF_SUB);
          setNegativeZero(rf[alu.instr.subr.rn] - rf[alu.instr.subr.rm]);
          stats.numRegWrites++;
          stats.numRegReads += 2;
          stats.instrs++;
          break;
        case ALU_ADD3I:
          // Done
          rf.write(alu.instr.add3i.rd, rf[alu.instr.add3i.rn] + alu.instr.add3i.imm);
          setCarryOverflow(rf[alu.instr.add3i.rn], alu.instr.add3i.imm, OF_ADD);
          setNegativeZero(rf[alu.instr.add3i.rn] + alu.instr.add3i.imm);
          stats.numRegWrites++;
          stats.numRegReads++;
          stats.instrs++;
          break;
        case ALU_SUB3I:
          // Done
          rf.write(alu.instr.sub3i.rd, rf[alu.instr.sub3i.rn] - alu.instr.sub3i.imm);
          setCarryOverflow(rf[alu.instr.sub3i.rn], alu.instr.sub3i.imm, OF_SUB);
          setNegativeZero(rf[alu.instr.sub3i.rn] - alu.instr.sub3i.imm);
          stats.numRegWrites++;
          stats.numRegReads++;
          stats.instrs++;
          break;
        case ALU_MOV:
          // Done
          rf.write(alu.instr.mov.rdn, alu.instr.mov.imm);
          setNegativeZero(alu.instr.mov.imm);
          stats.numRegWrites++;
          stats.instrs++;
          break;
        case ALU_CMP:
          // Done
          setCarryOverflow(rf[alu.instr.cmp.rdn], alu.instr.cmp.imm, OF_SUB);
          setNegativeZero(rf[alu.instr.cmp.rdn] - alu.instr.cmp.imm);
          stats.numRegReads++;
          stats.instrs++;
          break;
        case ALU_ADD8I:
          // Done
          rf.write(alu.instr.add8i.rdn, rf[alu.instr.add8i.rdn] + alu.instr.add8i.imm);
          setCarryOverflow(rf[alu.instr.add8i.rdn], alu.instr.add8i.imm, OF_ADD);
          setNegativeZero(rf[alu.instr.add8i.rdn] + alu.instr.add8i.imm);
          stats.numRegWrites++;
          stats.numRegReads++;
          stats.instrs++;
          break;
        case ALU_SUB8I:
          // Done
          rf.write(alu.instr.sub8i.rdn, rf[alu.instr.sub8i.rdn] - alu.instr.sub8i.imm);
          setCarryOverflow(rf[alu.instr.sub8i.rdn], alu.instr.sub8i.imm, OF_SUB);
          setNegativeZero(rf[alu.instr.sub8i.rdn] - alu.instr.sub8i.imm);
          stats.numRegWrites++;
          stats.numRegReads++;
          stats.instrs++;
          break;
        default:
          cout << "instruction not implemented" << endl;
          exit(1);
          break;
      }
      break;
    case BL: 
      // This instruction is complete, nothing needed here
      bl_ops = decode(blupper);
      if (bl_ops == BL_UPPER) {
        // PC has already been incremented above
        instr2 = imem[PC];
        BL_Type bllower(instr2);
        if (blupper.instr.bl_upper.s) {
          addr = static_cast<unsigned int>(0xff<<24) | 
            ((~(bllower.instr.bl_lower.j1 ^ blupper.instr.bl_upper.s))<<23) |
            ((~(bllower.instr.bl_lower.j2 ^ blupper.instr.bl_upper.s))<<22) |
            ((blupper.instr.bl_upper.imm10)<<12) |
            ((bllower.instr.bl_lower.imm11)<<1);
        }
        else {
          addr = ((blupper.instr.bl_upper.imm10)<<12) |
            ((bllower.instr.bl_lower.imm11)<<1);
        }
        // return address is 4-bytes away from the start of the BL insn
        rf.write(LR_REG, PC + 2);
        // Target address is also computed from that point
        rf.write(PC_REG, PC + 2 + addr);

        stats.numRegReads += 1;
        stats.numRegWrites += 2; 
        stats.instrs++;
      }
      else {
        cerr << "Bad BL format." << endl;
        exit(1);
      }
      break;
    case DP:
      dp_ops = decode(dp);
      switch(dp_ops) {
        case DP_CMP:
          // Done
          setCarryOverflow(rf[dp.instr.DP_Instr.rdn], rf[dp.instr.DP_Instr.rm], OF_SUB);
          setNegativeZero(rf[dp.instr.DP_Instr.rdn] - rf[dp.instr.DP_Instr.rm]);
          stats.numRegReads += 2;
          stats.instrs++;
          break;
      }
      break;
    case SPECIAL:
      sp_ops = decode(sp);
      switch(sp_ops) {
        case SP_MOV:
          // Done
          rf.write((sp.instr.mov.d << 3 ) | sp.instr.mov.rd, rf[sp.instr.mov.rm]);
          setNegativeZero(rf[sp.instr.mov.rm]);
          stats.numRegWrites++;
          stats.numRegReads++;
          stats.instrs++;
          break;
        case SP_ADD:
          // Done
          rf.write((sp.instr.add.d << 3) | sp.instr.add.rd, rf[(sp.instr.add.d << 3) | 
            sp.instr.add.rd] + rf[sp.instr.add.rm]);
          setCarryOverflow((sp.instr.add.d << 3) | sp.instr.add.rd, rf[(sp.instr.add.d << 3) |
            sp.instr.add.rd] + rf[sp.instr.add.rm], OF_ADD);
          setNegativeZero(rf[(sp.instr.add.d << 3) | sp.instr.add.rd] + rf[sp.instr.add.rm]);
          stats.numRegWrites++;
          stats.numRegReads += 2;
          stats.instrs++;
          break;
        case SP_CMP:
          // Done
          setCarryOverflow(rf[(sp.instr.cmp.d << 3) | sp.instr.cmp.rd], rf[sp.instr.cmp.rm], OF_SUB);
          setNegativeZero(rf[(sp.instr.cmp.d << 3) | sp.instr.cmp.rd] - rf[sp.instr.cmp.rm]);
          stats.numRegReads += 2;
          stats.instrs++;
          break;
      }
      break;
    case LD_ST:
      // You'll want to use these load and store models
      // to implement ldrb/strb, ldm/stm and push/pop
      ldst_ops = decode(ld_st);
      switch(ldst_ops) {
        case STRI:
          // Done
          addr = rf[ld_st.instr.ld_st_imm.rn] + ld_st.instr.ld_st_imm.imm * 4;
          caches.access(addr);
          dmem.write(addr, rf[ld_st.instr.ld_st_imm.rt]);
          stats.numMemWrites++;
          stats.numRegReads += 2;
          stats.instrs++;
          break;
        case LDRI:
          // Done
          addr = rf[ld_st.instr.ld_st_imm.rn] + ld_st.instr.ld_st_imm.imm * 4;
          caches.access(addr);
          rf.write(ld_st.instr.ld_st_imm.rt, dmem[addr]);
          stats.numMemReads++;
          stats.numRegReads++;
          stats.numRegWrites++;
          stats.instrs++;
          break;
        case STRR:
          // Done
          addr = rf[ld_st.instr.ld_st_reg.rn] + rf[ld_st.instr.ld_st_reg.rm];
          caches.access(addr);
          dmem.write(addr, rf[ld_st.instr.ld_st_reg.rt]);
          stats.numMemWrites++;
          stats.numRegReads += 3;         // ?
          stats.instrs++;
          break;
        case LDRR:
          // Done
          addr = rf[ld_st.instr.ld_st_reg.rn] + rf[ld_st.instr.ld_st_reg.rm];
          rf.write(ld_st.instr.ld_st_reg.rt, dmem[addr]);
          caches.access(addr);
          stats.numRegWrites++;
          stats.numRegReads += 2;         // ?
          stats.numMemReads++;
          stats.instrs++;
          break;
        case STRBI:
          // Done
          // Gets address
          addr = rf[ld_st.instr.ld_st_imm.rn] + ld_st.instr.ld_st_imm.imm * 4;
          caches.access(addr);
          // got to separate it and place into temp since it returns void. it just edits 
          // the value itself. s
          temp = dmem[addr];
          // pass 0 since we want index 0-3. see driver.hpp for ubyte implementation
          temp.set_data_ubyte4(0,rf[ld_st.instr.ld_st_imm.rt]);
          //Gets value at address and then its least sig byte and store it
          dmem.write(addr, temp);
          stats.numMemWrites++;
          stats.numRegReads += 2;
          stats.numMemReads++;
          stats.instrs++;
          break;
        case LDRBI:
          // Done
          // loads byte from memory 
          addr = ld_st.instr.ld_st_imm.rn + ld_st.instr.ld_st_imm.imm*4;
          caches.access(addr);
          // no need to separate into a temp like in strb. This is because data_utype 
          // returns a value rather than edits the value
          // see driver.hpp to review implementation
          // Gets value at address, least sig byte and store at register
          rf.write(ld_st.instr.ld_st_imm.rt, dmem[addr].data_ubyte4(0));
          // I think
          stats.numRegWrites++;
          // stats.numRegReads++;         // ?
          stats.numMemReads++;
          stats.instrs++;
          break;
        case STRBR:
          // Done
          // stores least significant byte of register
          // i think
          addr = rf[ld_st.instr.ld_st_imm.rn] + rf[ld_st.instr.ld_st_reg.rm]; // Gets address
          caches.access(addr);
          temp = dmem[addr];
          temp.set_data_ubyte4(0, rf[ld_st.instr.ld_st_imm.rt]);
          //Gets value at address and then its least sig byte and store it
          dmem.write(addr, temp);
          stats.numMemWrites++;
          stats.numRegReads += 3;
          stats.numMemReads++;
          stats.instrs++;
          break;
        case LDRBR:
          // Done
          // i think
          addr = ld_st.instr.ld_st_imm.rn + rf[ld_st.instr.ld_st_reg.rm];
          caches.access(addr);
          // Gets value at address, least sig byte and store at register
          rf.write(ld_st.instr.ld_st_imm.rt, dmem[addr].data_ubyte4(0));
          stats.numRegWrites++;
          stats.numRegReads++;            // ?
          stats.numMemReads++;
          stats.instrs++;
          break;
      }
      break;
    case MISC:
      misc_ops = decode(misc);
      switch(misc_ops) {
        case MISC_PUSH:
          BitCount = count_bits(misc.instr.push.reg_list, misc.instr.push.m);
          addr = SP - 4 * BitCount;

          for (int i = 1, j = 0; i <= 128; i *= 2, j++) {
            if (misc.instr.push.reg_list & i) {
              dmem.write(addr, rf[j]);
              caches.access(addr);
              addr += 4;
              stats.numMemWrites++;
              stats.numRegReads++;
            }
          }

          if (misc.instr.push.m) {
            dmem.write(addr, LR);
            caches.access(addr);
            stats.numMemWrites++;
            stats.numRegReads++;
          }

          rf.write(SP_REG, SP - 4 * BitCount);
          stats.numRegReads++;
          stats.numRegWrites++;
          stats.instrs++;
          break;
        case MISC_POP:
          BitCount = count_bits(misc.instr.pop.reg_list, misc.instr.pop.m);
          addr = SP;

          if (misc.instr.pop.m) {
            rf.write(PC_REG, dmem[addr]);
            caches.access(addr);
            stats.numRegWrites++;
            stats.numMemReads++;
          }

          for (int i = 128, j = 7; i >= 1; i /= 2, j--) {
            if (misc.instr.pop.reg_list & i) {
              rf.write(j, dmem[addr]);
              caches.access(addr);
              addr += 4;
              stats.numRegWrites++;
              stats.numMemReads++;
            }
          }

          rf.write(SP_REG, SP + 4 * BitCount);
          stats.numRegWrites++;
          stats.numRegReads++;
          stats.instrs++;
          break;
        case MISC_SUB:
          // Done
          rf.write(SP_REG, SP - (misc.instr.sub.imm*4));
          stats.numRegWrites++;
          stats.numRegReads++;
          stats.instrs++;
          break;
        case MISC_ADD:
          // Done
          rf.write(SP_REG, SP + (misc.instr.add.imm*4));
          stats.numRegWrites++;
          stats.numRegReads++;
          stats.instrs++;
          break;
      }
      break;
    case COND:
      decode(cond);
      // Once you've completed the checkCondition function,
      // this should work for all your conditional branches.
      // needs stats
      direction = 2 * signExtend8to32ui(cond.instr.b.imm) + 2;    // the offset
      if (checkCondition(cond.instr.b.cond)){
        if (direction > 0) {
          stats.numForwardBranchesTaken++;
        }
        else {
          stats.numBackwardBranchesTaken++;
        }
        rf.write(PC_REG, PC + 2 * signExtend8to32ui(cond.instr.b.imm) + 2);
        stats.numRegWrites++;
        stats.numRegReads++;
      }
      else {
        if (direction > 0) {
          stats.numForwardBranchesNotTaken++;
        }
        else {
          stats.numBackwardBranchesNotTaken++;
        }
      }
      stats.instrs++;
      break;
    case UNCOND:
      // Essentially the same as the conditional branches, but with no
      // condition check, and an 11-bit immediate field
      decode(uncond);
      rf.write(PC_REG, PC + 2 * signExtend11to32ui(cond.instr.b.imm) + 2);
      stats.numRegWrites++;
      stats.numRegReads++;
      stats.instrs++;
      break;
    case LDM:
      decode(ldm);
      // Done
      addr = rf[ldm.instr.ldm.rn]; // Starting Address
      for(num1 = 0; num1 < 8; num1++ ) {
        if((1 << num1) & ldm.instr.ldm.reg_list) { // Check if register is present
            addr += 4; // Move up 1 spot
            caches.access(addr);
            rf.write(num1,dmem[addr]);
            stats.numRegWrites++;
            stats.numMemReads++;
        }
      }
      stats.numRegReads++;
      stats.instrs++;
      break;
    case STM:
      decode(stm);
      // Done
      addr = rf[stm.instr.stm.rn]; // Starting Address
      for(num1 = 0; num1 < 8; num1++ ) {
        if((1 << num1) & stm.instr.stm.reg_list) { // Check if register is present
            dmem.write(addr,rf[num1]);
            caches.access(addr);
            addr += 4; // Move up 1 spot
            stats.numRegWrites++;
            stats.numMemReads++;
        }
      }
      rf.write(stm.instr.stm.rn,addr);
      stats.numRegReads++;
      stats.numRegWrites++;
      stats.instrs++;
      break;
    case LDRL:
      // This instruction is complete, nothing needed
      decode(ldrl);
      // Need to check for alignment by 4
      if (PC & 2) {
        addr = PC + 2 + (ldrl.instr.ldrl.imm)*4;
      }
      else {
        addr = PC + (ldrl.instr.ldrl.imm)*4;
      }
      // Requires two consecutive imem locations pieced together
      temp = imem[addr] | (imem[addr+2]<<16);  // temp is a Data32
      rf.write(ldrl.instr.ldrl.rt, temp);

      // One write for updated reg
      stats.numRegWrites++;
      // One read of the PC
      stats.numRegReads++;
      // One mem read, even though it's imem, and there's two of them
      stats.numMemReads++;
      stats.instrs++;
      break;
    case ADD_SP:
      // Done
      decode(addsp);
      rf.write(addsp.instr.add.rd, SP + (addsp.instr.add.imm*4));
      stats.numRegWrites++;
      stats.numRegReads++;
      stats.instrs++;
      break;
    default:
      cout << "[ERROR] Unknown Instruction to be executed" << endl;
      exit(1);
      break;
  }
}
