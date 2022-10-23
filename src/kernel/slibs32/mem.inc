;
; Memory (32-bit) Static Library
;
; Author: Jakub Verner
; Date: 22-10-2022
;

%ifndef _MEM32
%define _MEM32

;
; Magic Numbers
;

%define NULL 0x00000000

%define TRUE 0xFF
%define FALSE 0x00

;
; Global Variables
;

heap_start dd 0x00000000
heap_size dd 0x00000000

first_free_block dd 0x00000000

;
; Initialize Heap
;
; Input: EAX (heap start), EDX (heap size)
; Output: Nothing
;

init_heap:
mov dword [heap_start], eax
mov dword [heap_size], edx

mov dword [first_free_block], eax

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