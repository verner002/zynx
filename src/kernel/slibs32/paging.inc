;
; Paging (32-bit) Static Library
;
; Author: Jakub Verner
; Date: 02-11-2022
;

%ifndef _PAGING32
%define _PAGING32

;
; Create Page Directory
;
; Input: Nothing
; Output: EAX (page dir ptr)
;

;crte_page_dir:
;push ecx
;push edi
;mov edi, 0x00001000
;call balloc
;jz .terminate

;mov eax, dword [eax+5] ; store linked-list node's addr somewhere

;mov edi, eax
;mov ecx, 0x00000400

;push eax
;mov eax, 0x00000002
;rep stosd
;pop eax
;pop edi
;pop ecx
;ret

;
; Create Page Table
;
; Input: EDI (page dir ptr)
; Output: Nothing
;

;crte_page_tbl:
;mov esi, edi

;mov cx, 0x0400

;.next_entry:
;lodsd
;test al, 0x01
;jz .found_aval
;dec cx
;jnz .next_entry
;stc
;xor eax, eax
;ret

;.found_aval:
;sub esi, 0x00000004

;mov edi, 0x00001000
;call balloc
;jz .terminate

;mov eax, dword [eax+5] ; store linked-list node's addr somewhere

;or al, 0x03

;mov dword [esi], eax
;and al, 0xfc
;clc
;ret

;
; Map Page
;
; Input: EAX (phys addr), EBX (virt addr), DI (flags)
; Output: Nothing
;

map_page:
push ebx
push ecx
push di
mov ecx, ebx

shr ebx, 0x16
shl ebx, 0x02

add ebx, 0x0000e000

mov ebx, dword [ebx]
and ebx, 0xfffff000

shr ecx, 0x0c
shl ecx, 0x02
and ecx, 0x00000fff

add ebx, ecx

and di, 0x0fff
or ax, di
or al, 0x01 ; present

mov dword [ebx], eax ; write phys addr
pop di
pop ecx
pop ebx
ret

%endif