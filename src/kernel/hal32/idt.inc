;
; IDT Static Library
;
; Author: Jakub Verner
; Date: 31-10-2022
;

%ifndef _IDT32
%define _IDT32

%define IDT_TBL 0x00020000

;
; Set Handler
;
; Input: AL (int num), BX (selector), EDX (isr addr)
; Output: Nothing
;

set_handler:
push eax
push edx
movzx eax, al
shl eax, 0x03
add eax, IDT_TBL

mov word [eax], dx
mov word [eax+0x00000002], bx
mov byte [eax+0x00000004], 0x00
mov byte [eax+0x00000005], 10001110b
shr edx, 0x10
mov word [eax+0x00000006], dx
pop edx
pop eax
ret

;
; Unset Handler
;
; Input: AL (int num)
; Output: Nothing
;

unset_handler:
push eax
movzx eax, al
shl eax, 0x03
add eax, IDT_TBL
mov dword [eax], 0x00000000
mov dword [eax+0x00000004], 0x00000000
pop eax
ret

%endif