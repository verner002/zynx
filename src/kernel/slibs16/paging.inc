;
; Paging Static Library
;
; Author: Jakub Verner
; Date: 12-10-2022
;

%ifndef _PAGING16
%define _PAGING16

;
; Initialize Paging
;
; Input: Nothing
; Output: Nothing
;

init_paging:
pusha
mov cx, 0x0400
mov di, 0xa000

mov eax, 0x00000002
rep stosd

xor ecx, ecx
;mov di, 0xb000

.fill_entries:
mov eax, ecx
shl eax, 0x0c
or al, 0x03
stosd
inc cl
jnz .fill_entries

mov ebx, 0x0000a000
mov dword [bx], 0x0000b003
mov cr3, ebx
popa
ret

%endif