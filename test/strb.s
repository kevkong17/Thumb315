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
   push  {r7, lr}
   mov   r7, sp
   mov   r0, #65        @ 'A'
   strb  r0, [r7]
   ldrb  r1, [r7]
   pop   {r7, pc}
