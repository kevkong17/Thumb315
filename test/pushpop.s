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
   mov   r4, #20
   push  {r4, lr}
   mov   r4, #5
   mov   r5, #3
   add   r6, r4, r5
   pop   {r4, pc}
