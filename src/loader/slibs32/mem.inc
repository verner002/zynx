;
; Memory (32-bit) Static Library
;
; Author: Jakub Verner
; Date: 02-11-2022
;

%ifndef _MEM32
%define _MEM32

;
; Block
;
; Offset:
;  0x00000000 | dword | magic (block identifier)
;  0x00000004 | byte  | free (usable or not)
;  0x00000005 | dword | start (start of mem block)
;  0x00000009 | dword | size (without size of block)
;  0x0000000d | dword | previous (previous block)
;  0x00000011 | dword | next (next block)
;

%define SZ_OF_BLOCK 0x00000015

%define MAGIC 0x600dc0fe

%define FREE    0x00000004
%define START   0x00000005
%define SIZE    0x00000009
%define PREV    0x0000000d    
%define NEXT    0x00000011

;
; Global Variables
;

first_block dd NULL

;
; Initialize Memory
;
; Input: ECX (mem start), EBX (mem size)
; Output: CF (set on err)
;

init_mem:
push eax
push edi
mov edi, SZ_OF_BLOCK
call malloc
jz .return

mov dword [first_block], eax

mov dword [eax], MAGIC
mov byte [eax+FREE], TRUE
mov dword [eax+START], ecx
mov dword [eax+SIZE], ebx
mov dword [eax+PREV], NULL
mov dword [eax+NEXT], NULL

.return:
pop edi
pop eax
ret

;
; Block Allocation
;
; Input: EDI (size in bytes)
; Output: EAX (block ptr)
;

balloc:
;push ebp
;mov ebp, esp
;sub esp, [value]
cmp edi, 0x00000000
je .return_null

mov eax, dword [first_block]

.do:
test byte [eax+FREE], 0xff
jz .continue

mov edx, dword [eax+SIZE]

cmp edx, edi
jb .continue
je .dont_split

push eax
push edi
mov edi, SZ_OF_BLOCK
call malloc
;jz .terminate
mov ebx, eax
pop edi
pop eax

mov dword [ebx], MAGIC
mov byte [ebx+FREE], TRUE

sub edx, edi
mov dword [ebx+SIZE], edx
mov dword [ebx+PREV], eax

mov edx, edi
;shl edx, 0x0a ; to bytes - already is in bytes
add edx, dword [eax+START]
mov dword [ebx+START], edx

mov edx, dword [eax+NEXT]
mov dword [ebx+NEXT], edx

cmp eax, dword [first_block]
jne .dont_change_first_block

mov dword [first_block], ebx

.dont_change_first_block:
mov dword [eax+SIZE], edi
mov dword [eax+NEXT], ebx

.dont_split:
mov byte [eax+FREE], FALSE

;mov eax, dword [eax+START]
jmp .return

.continue:
mov eax, dword [eax+NEXT]

test eax, 0xffffffff
jnz .do

.return_null:
xor eax, eax

.return:
;leave
ret

;
; Block Free
;
; Input: EDI (addr)
; Output: Nothing
;

bfree:
;push ebp
;mov ebp, esp
;sub esp, [value]
pusha
cmp dword [edi], MAGIC
jnz .dont_change_first_block

test byte [edi+FREE], 0xff
jnz .dont_change_first_block

mov byte [edi+FREE], TRUE

mov eax, dword [edi+NEXT]

test eax, 0xffffffff
jz .try_prev

test byte [eax+FREE], 0xff
jz .try_prev

call .merge

.try_prev:
mov eax, dword [edi+PREV]

test eax, 0xffffffff
jz .return

test byte [eax+FREE], 0xff
jz .return

mov edi, eax

call .merge

.return:
cmp dword [first_block], edi
jna .dont_change_first_block

mov dword [first_block], edi

.dont_change_first_block:
popa
;leave
ret

.merge:
mov ebx, dword [edi+NEXT]

mov ecx, dword [ebx+SIZE]
add dword [edi+SIZE], ecx

mov ecx, dword [ebx+NEXT]
mov dword [edi+NEXT], ecx
ret

%endif