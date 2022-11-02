;
; Heap (32-bit) Static Library
;
; Author: Jakub Verner
; Date: 02-11-2022
;

%ifndef _HEAP32
%define _HEAP32

;
; Chunk
;
; Offset:
;  0x00000000 | dword | magic (chunk identifier)
;  0x00000004 | byte  | free (usable or not)
;  0x00000005 | dword | size (without size of chunk)
;  0x00000009 | dword | previous (previous chunk)
;  0x0000000d | dword | next (next chunk)
;

%define SZ_OF_CHUNK 0x00000011

%define CHUNK_MAGIC 0xdeadc0de

%define CHUNK_FREE  0x00000004
%define CHUNK_SIZE  0x00000005
%define CHUNK_PREV  0x00000009
%define CHUNK_NEXT  0x0000000d

;
; Global Variables
;

first_chunk dd NULL

;
; Initialize Heap
;
; Input: EAX (heap start), EDX (heap size)
; Output: Nothing
;

init_heap:
mov dword [first_chunk], eax

mov dword [eax], CHUNK_MAGIC
mov byte [eax+CHUNK_FREE], TRUE
mov dword [eax+CHUNK_SIZE], edx
mov dword [eax+CHUNK_PREV], NULL
mov dword [eax+CHUNK_NEXT], NULL
ret

;
; Memory Allocation
;
; Input: EDI (size)
; Output: EAX (addr or null)
;

malloc:
;push ebp
;mov ebp, esp
;sub esp, [value]
push ebx
push edx

cmp edi, 0x00000000
je .return_null

mov eax, dword [first_chunk]

.do:
test byte [eax+CHUNK_FREE], 0xff ; BYTE [EAX+CHUNK_FREE] == TRUE?
jz .continue

mov edx, dword [eax+CHUNK_SIZE]

cmp edx, edi
jb .continue
je .dont_split

mov ebx, eax
add ebx, edi
add ebx, SZ_OF_CHUNK

mov dword [ebx], CHUNK_MAGIC
mov byte [ebx+CHUNK_FREE], TRUE

sub edx, edi
mov dword [ebx+CHUNK_SIZE], edx
mov dword [ebx+CHUNK_PREV], eax

mov edx, dword [eax+CHUNK_NEXT]
mov dword [ebx+CHUNK_NEXT], edx

cmp eax, dword [first_chunk]
jne .dont_change_first_chunk

mov dword [first_chunk], ebx

.dont_change_first_chunk:
mov dword [eax+CHUNK_SIZE], edi
mov dword [eax+CHUNK_NEXT], ebx

.dont_split:
mov byte [eax+CHUNK_FREE], FALSE

add eax, SZ_OF_CHUNK
jmp .return

.continue:
mov eax, dword [eax+CHUNK_NEXT]

test eax, 0xffffffff ; EAX == NULL?
jnz .do

.return_null:
xor eax, eax

.return:
pop edx
pop ebx
;leave
ret

;
; Free
;
; Input: EDI (addr)
; Output: Nothing
;

free:
;push ebp
;mov ebp, esp
;sub esp, [value]
pusha
sub edi, SZ_OF_CHUNK

cmp dword [edi], CHUNK_MAGIC
jnz .dont_change_first_chunk

test byte [edi+CHUNK_FREE], 0xff ; BYTE [EDI+CHUNK_FREE] == TRUE?
jnz .dont_change_first_chunk

mov byte [edi+CHUNK_FREE], TRUE

mov eax, dword [edi+CHUNK_NEXT]

test eax, 0xffffffff ; EAX == NULL?
jz .try_prev

test byte [eax+CHUNK_FREE], 0xff ; BYTE [EAX+CHUNK_FREE] == TRUE?
jz .try_prev

call .merge

.try_prev:
mov eax, dword [edi+CHUNK_PREV]

test eax, 0xffffffff ; EAX == NULL?
jz .return

test byte [eax+CHUNK_FREE], 0xff ; BYTE [EAX+CHUNK_FREE] == TRUE?
jz .return

mov edi, eax

call .merge

.return:
cmp dword [first_chunk], edi
jna .dont_change_first_chunk

mov dword [first_chunk], edi

.dont_change_first_chunk:
popa
;leave
ret

.merge:
mov ebx, dword [edi+CHUNK_NEXT]

mov ecx, dword [ebx+CHUNK_SIZE]
add dword [edi+CHUNK_SIZE], ecx

mov ecx, dword [ebx+CHUNK_NEXT]
mov dword [edi+CHUNK_NEXT], ecx
ret

%endif