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
   b     next
   pop   {pc}

next:
   mov   r1, #1
   pop   {pc}
