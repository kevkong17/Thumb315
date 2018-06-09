   .arch armv6
   .fpu softvfp
   .code 16
   .text
   .align   2
   .global  main
   .code 16
   .thumb_func
   .type main, %function

main:
   push  {lr}
   mov   r1, #5
   mov   r2, #3
   cmp   r1, r2      @ Compare r1 and r2
   beq   equal0      @ Not equal, skip b
   mov   r3, #0      @ Set r3 to 0

   mov   r4, #4
   mov   r5, #4
   cmp   r4, r5      @ Compare r4 and r5
   beq   equal1      @ Are equal, branch to equal1
   mov   r6, #0      @ Should not hit this
   pop   {pc}

equal0:
   mov   r3, #1
   pop   {pc}        @ If it got here something went wrong, end

equal1:
   mov   r6, #20     @ If it got here, set r6 to 20
   pop   {pc}        @ End
