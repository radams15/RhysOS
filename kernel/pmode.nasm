bits 16

global _call_pmode
extern _seg_copy

_call_pmode:
   xor ax, ax             ; make it zero
   mov ds, ax             ; DS=0
   mov ss, ax             ; stack starts at seg 0
   mov es, ax             ; stack starts at seg 0
   mov sp, 0x9c00         ; 2000h past code start,
                          ; making the stack 7.5k in size

   cli                    ; no interrupts
   push ds                ; save real mode

   lgdt [0x1002]

   mov  eax, cr0          ; switch to pmode by
   or al,1                ; set pmode bit

   mov  cr0, eax

   xchg bx, bx
   jmp 0x8:0x1000
   ; jmp 0x8:pmode

pmode:
   mov  bx, 0x10          ; select descriptor 2, instead of 1
   mov  ds, bx            ; 10h = 10000b

   and  al,0xFE            ; back to realmode
   mov  cr0, eax          ; by toggling bit again
   jmp 0x0:huge_unreal

huge_unreal:
   pop ds                 ; get back old segment
   sti

   mov bx, 0x0f01         ; attrib/char of smiley
   mov eax, 0x0b8000      ; note 32 bit offset
   mov word [ds:eax], bx

   jmp $                  ; loop forever
