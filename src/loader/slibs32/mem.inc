;
; Memory (32-bit) Static Library
;
; Author: Jakub Verner
; Date: 29-10-2022
;

%ifndef _MEM32
%define _MEM32

;
; Magic Numbers
;

%define NULL 0x00000000

%define FALSE 0x00
%define TRUE !FALSE

;
; Block
;
; Offset:
;  0x00000000 | byte  | free (usable or not)
;  0x00000001 | dword | size (without size of block)
;  0x00000005 | dword | previous (previous block)
;  0x00000009 | dword | next (next block)
;

%define SZ_OF_BLOCK 0x00000011

%define MAGIC       0xdeadc0de

%define FREE        0x00000004
%define SIZE        0x00000005
%define PREV        0x00000009
%define NEXT        0x0000000d

;
; Global Variables
;

heap_start dd 0x00000000
heap_size dd 0x00000000

first_block dd 0x00000000

;
; Initialize Heap
;
; Input: EAX (heap start), EDX (heap size)
; Output: Nothing
;

init_heap:
mov dword [heap_start], eax
mov dword [heap_size], edx

mov dword [first_block], eax

mov dword [eax], MAGIC
mov byte [eax+FREE], TRUE
mov dword [eax+SIZE], edx
mov dword [eax+PREV], NULL
mov dword [eax+NEXT], NULL
ret

;
; Memory Allocation
;
; Input: EDI (size)
; Output: EAX (ptr or null)
;

malloc:
push ebp
mov ebp, esp
;sub esp, [value]

cmp edi, 0x00000000
je .return_null

mov eax, dword [first_block]

.do:
test byte [eax+FREE], 0xff ; BYTE [EAX+FREE] == TRUE?
jz .continue

mov edx, dword [eax+SIZE]

cmp edx, edi
jb .continue
je .dont_split

mov ebx, eax
add ebx, edi
add ebx, SZ_OF_BLOCK

mov dword [ebx], MAGIC
mov byte [ebx+FREE], TRUE

sub edx, edi
mov dword [ebx+SIZE], edx
mov dword [ebx+PREV], eax

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

add eax, SZ_OF_BLOCK
jmp .return

.continue:
mov eax, dword [eax+NEXT]

test eax, 0xffffffff ; EAX == NULL?
jnz .do

.return_null:
xor eax, eax

.return:
leave
ret

;
; Free
;
; Input: EDI (ptr to block)
; Output: Nothing
;

free:
push ebp
mov ebp, esp
;sub esp, [value]

sub edi, SZ_OF_BLOCK

cmp dword [edi], MAGIC
jnz .return

test byte [edi+FREE], 0xff ; BYTE [EDI+FREE] == TRUE?
jnz .return

mov byte [edi+FREE], TRUE

mov eax, dword [edi+NEXT]

test eax, 0xff ; EAX == NULL?
jz .try_prev

test byte [eax+FREE], 0xff ; BYTE [EAX+FREE] == TRUE?
jz .try_prev

call .merge

.try_prev:
mov eax, dword [edi+PREV]

test eax, 0xff ; EAX == NULL?
jz .return

test byte [eax+FREE], 0xff ; BYTE [EAX+FREE] == TRUE?
jz .return

mov edi, eax

call .merge

.return:
mov dword [first_block], edi

leave
ret

.merge:
mov ebx, dword [edi+NEXT]

mov ecx, dword [ebx+SIZE]
add dword [edi+SIZE], ecx

mov ecx, dword [ebx+NEXT]
mov dword [edi+NEXT], ecx
ret

;
; Find The Smallest Possible Block
;
; Input: EDX (req size in bytes)
; Output: EDX (addr or NULL)
;

;find_small_block:
;push ebp
;mov ebp, esp
;sub esp, [value] - reserve space

;mov eax, dword [first_block]
;mov dword [FOUND_BLOCK], NULL

;cmp edx, 0x00000000
;jz .return_null

;.do_while:
;cmp byte [eax], TRUE
;jnz .skip

;mov ebx, dword [eax+SIZE]

;cmp ebx, edx
;jb .skip

;cmp dword [FOUND_BLOCK], NULL
;jz .continue

;mov ecx, dword [FOUND_BLOCK]

;cmp ebx, dword [ecx+SIZE]
;jnb .skip

;.continue:
;mov dword [FOUND_BLOCK], eax

;cmp dword [eax+SIZE], edx
;jz .return

;.skip:
;mov eax, dword [eax+NEXT]

;cmp eax, NULL
;jnz .do_while

;.return:
;cmp dword [FOUND_BLOCK], NULL
;jz .return_null

;
; TODO: Create a new block which will represent
;  the remaining free memory. The first_block
;  will contain the address of this new block.
;

;mov dword [ebp-0x00000008], 

;add dword [FOUND_BLOCK], SZ_OF_BLOCK

;.return_null:
;mov edx, dword [FOUND_BLOCK]

;leave
;ret

;
; Find The Largest Possible Block
;
; Input: EDX (req size in bytes)
; Output: EDX (addr or NULL)
;

;find_large_block:
;ret

%endif