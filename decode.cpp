#include <iostream>
#include <iomanip>
#include "thumbsim.hpp"

using namespace std;


/* Decode to find what type it is */
Thumb_Types decode (const ALL_Types);

/* Decodes for each type */
ALU_Ops decode (const ALU_Type);
DP_Ops decode (const DP_Type);
SP_Ops decode (const SP_Type);
LD_ST_Ops decode (const LD_ST_Type);
MISC_Ops decode (const MISC_Type);
BL_Ops decode (const BL_Type);
int decode (const COND_Type);
int decode (const UNCOND_Type);
int decode (const LDM_Type);
int decode (const STM_Type);
int decode (const LDRL_Type);
int decode (const ADD_SP_Type);

/* This function is complete, no modifications needed */
Thumb_Types decode (const ALL_Types data) {
  if (data.type.alu.instr.class_type.type_check == ALU_TYPE) {
    return ALU;
  }
  else if (data.type.dp.instr.class_type.type_check == DP_TYPE) {
    return DP;
  }
  else if (data.type.sp.instr.class_type.type_check == SP_TYPE) {
    return SPECIAL;
  }
  else if (data.type.uncond.instr.class_type.type_check == UNCOND_TYPE) {
    return UNCOND;
  }
  else if (data.type.bl.instr.class_type.type_check == BL_TYPE) {
    return BL;
  }
  else if (data.type.misc.instr.class_type.type_check == MISC_TYPE) {
    return MISC;
  }
  else if (data.type.cond.instr.class_type.type_check == COND_TYPE) {
    return COND;
  }
  else if (data.type.ldm.instr.class_type.type_check == LDM_TYPE) {
    return LDM;
  }
  else if (data.type.stm.instr.class_type.type_check == STM_TYPE) {
    return STM;
  }
  else if (data.type.ldrl.instr.class_type.type_check == LDRL_TYPE) {
    return LDRL;
  }
  else if (data.type.addsp.instr.class_type.type_check == ADD_SP_TYPE) {
    return ADD_SP;
  }
  else {
    if (data.type.ld_st.instr.class_type.opA == LD_ST_REG_OPA) {
    }
    else if (data.type.ld_st.instr.class_type.opA == LD_ST_IMM_OPA) {
    }
    else if (data.type.ld_st.instr.class_type.opA == LD_ST_IMMB_OPA) {
    }
    else if (data.type.ld_st.instr.class_type.opA == LD_ST_IMMH_OPA) {
    }
    else if (data.type.ld_st.instr.class_type.opA == LD_ST_IMMSP_OPA) {
    }
    else {
      cout << "NO TYPE FOUND" << endl;
      return ERROR_TYPE;
    }
    return LD_ST;
  }
}

//Done
ALU_Ops decode (const ALU_Type data) {
  if (data.instr.lsli.op == ALU_LSLI_OP) {
    //Done
    if (opts.instrs) { 
        cout << "lsls r" << data.instr.lsli.rd  << ", r" << data.instr.lsli.rm << ", #" << data.instr.lsli.imm << endl;
    }
     return ALU_LSLI;
  }
  else if (data.instr.addr.op == ALU_ADDR_OP) {
    if (opts.instrs) { 
      cout << "adds r" << data.instr.addr.rd  << ", r" << data.instr.addr.rn << ", r" << data.instr.addr.rm << endl;
    }
    return ALU_ADDR;
  }
  else if (data.instr.subr.op == ALU_SUBR_OP) {
    // Done
    if (opts.instrs) { 
      cout << "sub r" << data.instr.subr.rd  << ", r" << data.instr.subr.rn << ", r" << data.instr.subr.rm << endl;
    }
    return ALU_SUBR;
  }
  else if (data.instr.add3i.op == ALU_ADD3I_OP) {
    // complete
    if (opts.instrs) { 
      cout << "adds r" << data.instr.add3i.rd << ", r" << data.instr.add3i.rn << ", #" << data.instr.add3i.imm << endl;
    }
    return ALU_ADD3I;
  }
  else if (data.instr.sub3i.op == ALU_SUB3I_OP) {
     // complete
    if (opts.instrs) { 
      cout << "subs r" << data.instr.sub3i.rd << ", r" << data.instr.sub3i.rn << ", #" << data.instr.sub3i.imm << endl;
    }
    return ALU_SUB3I;
  }
  else if (data.instr.add8i.op == ALU_ADD8I_OP) {
    // complete
    if (opts.instrs) { 
      cout << "adds r" << data.instr.add8i.rdn << ", #" << setbase(10) << data.instr.add8i.imm << endl;
    }
    return ALU_ADD8I;
  }
  else if (data.instr.sub8i.op == ALU_SUB8I_OP) {
    // complete
    if (opts.instrs) { 
      cout << "subs r" << data.instr.sub8i.rdn << ", #" << setbase(10) << data.instr.sub8i.imm << endl;
    }
    return ALU_SUB8I;
  }
  else if (data.instr.cmp.op == ALU_CMP_OP) { 
    // complete
    if (opts.instrs) { 
      cout << "cmp r" << data.instr.cmp.rdn << ", #" << setbase(10) << data.instr.cmp.imm << endl;
    }
    return ALU_CMP;
  }
  else if (data.instr.mov.op == ALU_MOV_OP) { 
    // complete
    if (opts.instrs) { 
      cout << "movs r" << data.instr.mov.rdn << ", #" << setbase(10) << (data.instr.mov.imm) << endl;
    }
    return ALU_MOV;
  }
}

//Done
DP_Ops decode (const DP_Type data) {
  if (data.instr.DP_Instr.op == DP_CMP) {
	// Complete
    if (opts.instrs) { 
        cout << dec << "cmp r" << data.instr.DP_Instr.rdn << ", r" << data.instr.DP_Instr.rm << endl;
    }
    return DP_CMP;
  }
  else {
    cerr << "UNKNOWN DP TYPE -- Exiting" << endl;
    exit(1);
  }
}

//Done
SP_Ops decode (const SP_Type data) {
  if (data.instr.mov.op == 2) {
    if (opts.instrs) { 
      cout << "mov";
      if (data.instr.mov.d) {  // this is the D bit is active. this is for rd for when rd is using registers greater than 7
        // These two cases handle stack pointer printing
        if (data.instr.mov.rd == 5) { // if rd is stack pointer
          cout << " sp, r" << setbase(10) << data.instr.mov.rm << endl;
        }
        else if (data.instr.mov.rm == 13) { //rm is stack pointer
          cout << " r" << setbase(10) << (8+data.instr.mov.rd) << ", sp" << endl;
        }
        // this case is for registers greater than r7 that aren't sp
        else { // a standard mov r#, r# where # for rd is > 7
          cout << " r" << setbase(10) << (8+data.instr.mov.rd) << ", r" << setbase(10) << data.instr.mov.rm << endl;
        }
      }
      //d bit inactive. so rd is < 8
      // another stack pointer case
      else if (data.instr.mov.rm == 13) {
        cout << " r" << data.instr.mov.rd << ", sp" << endl;
      }
      else {
        cout << " r" << setbase(10) << data.instr.mov.rd << ", r" << data.instr.mov.rm << endl;
      }
    }
    return SP_MOV;
  }
  else if (data.instr.add.op == 0) {
    // Here you'll need to SP_ADD similar to above
    cout << "add";
      if (data.instr.add.d) {  // this is the D bit is active. this is for rd for when rd is using registers greater than 7
        // These two cases handle stack pointer printing
        if (data.instr.add.rd == 5) { // if rd is stack pointer
          cout << " sp, r" << setbase(10) << data.instr.add.rm << endl;
        }
        else if (data.instr.add.rm == 13) { //rm is stack pointer
          cout << " r" << setbase(10) << (8+data.instr.add.rd) << ", sp" << endl;
        }
        // this case is for registers greater than r7 that aren't sp
        else { // a standard mov r#, r# where # for rd is > 7
          cout << " r" << setbase(10) << (8+data.instr.add.rd) << ", r" << setbase(10) << data.instr.add.rm << endl;
        }
      }
      //d bit inactive. so rd is < 8
      // another stack pointer case
      else if (data.instr.add.rm == 13) {
        cout << " r" << data.instr.add.rd << ", sp" << endl;
      }
      else {
        cout << " r" << setbase(10) << data.instr.add.rd << ", r" << data.instr.add.rm << endl;
      }
    return SP_ADD;
  }
  else if (data.instr.cmp.op == 1) {
    // Here you'll need to SP_CMP similar to above
		cout << "cmp";
      if (data.instr.cmp.d) {  // this is the D bit is active. this is for rd for when rd is using registers greater than 7
        // These two cases handle stack pointer printing
        if (data.instr.cmp.rd == 5) { // if rd is stack pointer
          cout << " sp, r" << setbase(10) << data.instr.cmp.rm << endl;
        }
        else if (data.instr.cmp.rm == 13) { //rm is stack pointer
          cout << " r" << setbase(10) << (8+data.instr.cmp.rd) << ", sp" << endl;
        }
        // this case is for registers greater than r7 that aren't sp
        else { // a standard mov r#, r# where # for rd is > 7
          cout << " r" << setbase(10) << (8+data.instr.cmp.rd) << ", r" << setbase(10) << data.instr.cmp.rm << endl;
        }
      }
      //d bit inactive. so rd is < 8
      // another stack pointer case
      else if (data.instr.cmp.rm == 13) {
        cout << " r" << data.instr.cmp.rd << ", sp" << endl;
      }
      else {
        cout << " r" << setbase(10) << data.instr.cmp.rd << ", r" << data.instr.cmp.rm << endl;
      }
    return SP_CMP;
  }
  else {
    cerr << "UNKNOWN SP TYPE -- Exiting" << endl;
    exit(1);
  }
}

// should be done
LD_ST_Ops decode (const LD_ST_Type data) {
  if (data.instr.class_type.opA == LD_ST_REG_OPA) {
    if (data.instr.class_type.opB == LD_ST_OPB_LDRB) {
      if(opts.instrs){
         cout << "LDRB r" << setbase(10) << data.instr.ld_st_reg.rt << ", [r" << setbase(10) << data.instr.ld_st_reg.rn << ", r" << setbase(10) << data.instr.ld_st_reg.rm << "]" << endl;
      }
      return LDRBR;
    }
    else if (data.instr.class_type.opB == LD_ST_OPB_STRB) {
      if(opts.instrs){
         cout << "STRB r" << setbase(10) << data.instr.ld_st_reg.rt << ", [r" << setbase(10) << data.instr.ld_st_reg.rn << ", r" << setbase(10) << data.instr.ld_st_reg.rm << "]" << endl;
      }
      return STRBR;
    }
    else if (data.instr.class_type.opB == LD_ST_OPB_LDR) {
      if(opts.instrs){
         cout << "LDR r" << setbase(10) << data.instr.ld_st_reg.rt << ", [r" << setbase(10) << data.instr.ld_st_reg.rn << ", r" << setbase(10) << data.instr.ld_st_reg.rm << "]" << endl;
      }
      return LDRR;
    }
    else if (data.instr.class_type.opB == LD_ST_OPB_STR) {
      if(opts.instrs){
         cout << "STR r" << setbase(10) << data.instr.ld_st_reg.rt << ", [r" << setbase(10) << data.instr.ld_st_reg.rn << ", r" << setbase(10) << data.instr.ld_st_reg.rm << "]" << endl;
      }return STRR;
    }
  }
  else if (data.instr.class_type.opA == LD_ST_IMM_OPA) {
    if (data.instr.class_type.opB == LD_ST_OPB_STR) {
      if (opts.instrs) { 
        cout << "str r" << data.instr.ld_st_imm.rt << ", [r" << data.instr.ld_st_imm.rn << ", #" << setbase(10) << (data.instr.ld_st_imm.imm*4) << "]" << endl;
      }
      return STRI;
    }
    else if (data.instr.class_type.opB == LD_ST_OPB_LDR) {
      if (opts.instrs) { 
        cout << "ldr r" << data.instr.ld_st_imm.rt << ", [r" << data.instr.ld_st_imm.rn << ", #" << setbase(10) << (data.instr.ld_st_imm.imm*4) << "]" << endl;
      }
      return LDRI;
    }
  }
  else if (data.instr.class_type.opA == LD_ST_IMMB_OPA) {
    if (data.instr.ld_st_imm.op == LD_ST_LDB) {
      if(opts.instrs) {
      	cout << "ldrb r" << data.instr.ld_st_imm.rt << ", [r" << data.instr.ld_st_imm.rn << ", #" << data.instr.ld_st_imm.imm*4 << "]" << endl;
      }
      return LDRBI;
    }
    else if (data.instr.ld_st_imm.op == LD_ST_STB) {
      if(opts.instrs) {
      	cout << "strb r" << data.instr.ld_st_imm.rt << ", [r" << data.instr.ld_st_imm.rn << ", #" << data.instr.ld_st_imm.imm*4 << "]" << endl;
      }return STRBI;
    }
  }
  else if (data.instr.class_type.opA == LD_ST_IMMB_OPA) {
  }
  else if (data.instr.class_type.opA == LD_ST_IMMH_OPA) {
  }
  else if (data.instr.class_type.opA == LD_ST_IMMSP_OPA) {
    if (data.instr.ld_st_sp_imm.opB) {
      if (opts.instrs) {
        cout << "ldr r" << data.instr.ld_st_sp_imm.rt << ", [sp, #" << setbase(10) << data.instr.ld_st_sp_imm.imm << "]" << endl;
        return LDRSP;
      }
    }
    else {
      if (opts.instrs) {
        cout << "str r" << data.instr.ld_st_sp_imm.rt << ", [sp, #" << setbase(10) << data.instr.ld_st_sp_imm.imm << "]" << endl;
        return STRSP;
      }
    }
  }
}

//Given Complete
MISC_Ops decode (const MISC_Type data) {
  if (data.instr.push.op == MISC_PUSH_OP) {
    // Push is complete
    if (opts.instrs) { 
      bool multiple = FALSE;
      cout << "push ";
      cout << "{";
      if (data.instr.push.reg_list & 1) {
        cout << "r0";
        multiple = TRUE;
      }
      if (data.instr.push.reg_list & 2) {
        if (multiple)
          cout << ", ";
        cout << "r1";
        multiple = TRUE;
      }
      if (data.instr.push.reg_list & 4) {
        if (multiple)
          cout << ", ";
        cout << "r2";
        multiple = TRUE;
      }
      if (data.instr.push.reg_list & 8) {
        if (multiple)
          cout << ", ";
        cout << "r3";
        multiple = TRUE;
      }
      if (data.instr.push.reg_list & 16) {
        if (multiple)
          cout << ", ";
        cout << "r4";
        multiple = TRUE;
      }
      if (data.instr.push.reg_list & 32) {
        if (multiple)
          cout << ", ";
        cout << "r5";
        multiple = TRUE;
      }
      if (data.instr.push.reg_list & 64) {
        if (multiple)
          cout << ", ";
        cout << "r6";
        multiple = TRUE;
      }
      if (data.instr.push.reg_list & 128) {
        if (multiple)
          cout << ", ";
        cout << "r7";
        multiple = TRUE;
      }
      if (data.instr.push.m) {
        if (multiple)
          cout << ", ";
        cout << "lr";
      }
      cout << "}" << endl;
    }
    return MISC_PUSH;
  }
  else if (data.instr.pop.op == MISC_POP_OP) {
    // Pop is complete
    if (opts.instrs) { 
      bool multiple = FALSE;
      cout << "pop ";
      cout << "{";
      if (data.instr.pop.reg_list & 1) {
        cout << "r0";
        multiple = TRUE;
      }
      if (data.instr.pop.reg_list & 2) {
        if (multiple)
          cout << ", ";
        cout << "r1";
        multiple = TRUE;
      }
      if (data.instr.pop.reg_list & 4) {
        if (multiple)
          cout << ", ";
        cout << "r2";
        multiple = TRUE;
      }
      if (data.instr.pop.reg_list & 8) {
        if (multiple)
          cout << ", ";
        cout << "r3";
        multiple = TRUE;
      }
      if (data.instr.pop.reg_list & 16) {
        if (multiple)
          cout << ", ";
        cout << "r4";
        multiple = TRUE;
      }
      if (data.instr.pop.reg_list & 32) {
        if (multiple)
          cout << ", ";
        cout << "r5";
        multiple = TRUE;
      }
      if (data.instr.pop.reg_list & 64) {
        if (multiple)
          cout << ", ";
        cout << "r6";
        multiple = TRUE;
      }
      if (data.instr.pop.reg_list & 128) {
        if (multiple)
          cout << ", ";
        cout << "r7";
        multiple = TRUE;
      }
      if (data.instr.pop.m) {
        if (multiple)
          cout << ", ";
        cout << "pc";
      }
      cout << "}" << endl;
    }
    return MISC_POP;
  }
  else if (data.instr.sub.op == MISC_SUB_OP) {
    // complete
    if (opts.instrs) { 
      cout << "sub sp, #";
      cout << setbase(10) << (static_cast<unsigned int>(data.instr.sub.imm)<< 2) << endl;
    } 
    return MISC_SUB;
  }
  else if (data.instr.add.op == MISC_ADD_OP) {
    // complete
    if (opts.instrs) { 
      cout << "add sp, #";
      cout << setbase(10) << (static_cast<unsigned int>(data.instr.add.imm)<< 2) << endl;
    } 
    return MISC_ADD;
  }
  else if (data.instr.class_type.type_check == ADD_SP8I_TYPE) {
    // complete
    if (opts.instrs) { 
      cout << "add sp, #" << setbase(10) << (data.instr.add.imm<<2)<< endl;
    }
    return MISC_ADD;
  }
}

//Given Complete
int decode (const COND_Type data) {
  // complete
  if (opts.instrs) { 
    cout << "b";
    printCond(data.instr.b.cond);
    cout << " 0x" << hex << rf[15] + 2*(int)((char)(data.instr.b.imm))+2 << endl;
  }
}

//Given Complete
int decode (const UNCOND_Type data) {
  // complete
  if (opts.instrs) { 
    cout << "b 0x" << hex << rf[15] + 2*(int)((char)(data.instr.b.imm))+2 << endl;
  }
}

//Given Complete
BL_Ops decode (const BL_Type data) {
  // complete
  if (opts.instrs) { 
    cout << "bl 0x" << hex << data.instr.bl_upper.imm10 << endl;
  }
  return BL_UPPER;
}

// Thumb always has !
// Load Multiple
int decode (const LDM_Type data) {
	bool first = FALSE;
	if (opts.instrs) {
      cout << dec << "ldm r" << data.instr.ldm.rn << "!, { ";
      for(int i = 0; i < 8; i++) {
      // Register List determines which register is listed 
      // Each has a bit representing its presence. 0001 for R0, 0010 for R1, etc. up to R7
	    if(data.instr.ldm.reg_list & (1 << i)) {
	      	if(first == TRUE) {
	      		cout << ", ";
	      	}
	      	cout << "r" << i;
	      	first = TRUE;
	    }
      }
      cout << "}" << endl;
  // 315: add code to print ldm 
  return LDM;
  }
}
 
// Thumb always has !
// Store Multiple
int decode (const STM_Type data) {
	bool first = FALSE;
	if (opts.instrs) {
      cout << dec << "stm r" << data.instr.stm.rn << "!, { ";
	for(int i = 0; i < 8; i++) {
	    if(data.instr.stm.reg_list & (1 << i)) {
	      	if(first == TRUE) {
	      		cout << ", ";
	      	}
	      	cout << "r" << i;
	      	first = TRUE;
	    }
      }
      cout << "}" << endl;
  return STM;
  }
}

// Given in Piazza
int decode (const LDRL_Type data) {
  if (opts.instrs) {
    cout << "ldr r" << data.instr.ldrl.rt << ", [pc, #" << setbase(10) << data.instr.ldrl.imm*4 << "]" << endl;
  }
  return LDRL;
}

// Given Complete
int decode (const ADD_SP_Type data) {
  // complete
  if (opts.instrs) { 
    cout << "add r" << data.instr.add.rd << ", sp, #" << data.instr.add.imm*4 << endl;
  }
}