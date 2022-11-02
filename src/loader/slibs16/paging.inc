;
; Paging (16-bit) Static Library
;
; Author: Jakub Verner
; Date: 02-11-2022
;

%ifndef _PAGING16
%define _PAGING16

;
; Initialize Paging
;
; Input: Nothing
; Output: Nothing
;
; Note:
;  Prepares a page directory and self
;  maps first MiB.
;

init_paging:
pusha
mov cx, 0x0400 ; num of pg dir entries
mov di, 0xe000 ; pg dir addr
movzx ebx, di

mov eax, 0x00000002
rep stosd ; all pg dir entries not present

.fill_entries:
movzx eax, cl
shl eax, 0x0c ; translate to addr
or al, 0x03 ; r/w and present
stosd
inc cl
jnz .fill_entries

mov dword [bx], 0x0000f003
mov cr3, ebx ; load pg dir
popa
ret

%endif