bits 16

global _call_pmode
extern _seg_copy

_call_pmode:
   push ds

   xor ax, ax             ; make it zero
   mov ds, ax             ; DS=0

   cli                    ; no interrupts

   lgdt [0x1002]

   mov  eax, cr0          ; switch to pmode by
   or al,1                ; set pmode bit

   mov  cr0, eax

   call 0x8:0x1000

   xchg bx, bx

   and  al, 0xFE            ; back to realmode
   mov  cr0, eax          ; by toggling bit again

   pop ds
   sti

   ret
