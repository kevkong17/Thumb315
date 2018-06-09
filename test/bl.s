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
   mov   r5, #1
   bl    other
   mov   r7, #3
   pop   {pc}

other:
   push  {lr}
   mov   r6, #2
   pop   {pc}
