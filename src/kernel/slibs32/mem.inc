;
; Memory (32-bit) Static Library
;
; Author: Jakub Verner
; Date: 22-10-2022
;

%ifndef _MEM32
%define _MEM32

;
; Block
;
; Offset:
;  0x00000000 | byte  | free
;  0x00000001 | dword | size
;  0x00000005 | dword | previous
;  0x00000009 | dword | next
;

;
; Magic Numbers
;

%define NULL 0x00000000

%define TRUE 0xFF
%define FALSE 0x00

%define SZ_OF_BLOCK 0x0000000d

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

mov byte [eax], TRUE
mov dword [eax+0x00000001], edx
mov dword [eax+0x00000005], NULL
mov dword [eax+0x00000009], NULL
ret

;
; Find The Smallest Possible Block
;
; Input: EDX (req size in bytes)
; Output: EDX (addr or NULL)
;

find_small_block:
push ebp
mov ebp, esp

mov eax, dword [first_block]
mov dword [ebp-0x00000004], NULL

cmp edx, 0x00000000
jz .return_null

.do_while:
cmp byte [eax], TRUE
jnz .skip

mov ebx, dword [eax+0x00000001]

cmp ebx, edx
jb .skip

cmp dword [ebp-0x00000004], NULL
jz .continue

mov ecx, dword [ebp-0x00000004]

cmp ebx, dword [ecx+0x00000001]
jnb .skip

.continue:
mov dword [ebp-0x00000004], eax

cmp dword [eax+0x00000001], edx
jz .return

.skip:
mov eax, dword [eax+0x00000009]

cmp eax, NULL
jnz .do_while

.return:
cmp dword [ebp-0x00000004], NULL
jz .return_null

;
; TODO: Create a new block which will represent
;  the remaining free memory. The first_block
;  will contain the address of this new block.
;

;mov dword [ebp-0x00000008], 

add dword [ebp-0x00000004], SZ_OF_BLOCK

.return_null:
mov edx, dword [ebp-0x00000004]

pop ebp
ret

;
; Find The Largest Possible Block
;
; Input: EDX (req size in bytes)
; Output: EDX (addr or NULL)
;

find_large_block:
ret

%endif