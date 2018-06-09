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
   mov   r1, #3
   cmp   r1, #5
   ble   next
   pop   {pc}

next:
   mov   r2, #1
   cmp   r1, #1
   ble   not
   mov   r3, #1
   pop   {pc}

not:
   mov   r5, #20     @ Should not hit this
   pop   {pc}
