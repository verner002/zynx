;
; 8254A (32-bit) Static Library
;
; Author: Jakub Verner
; Date: 22-10-2022
;

cpu 486
bits 32

%ifndef _PIT32
%define _PIT32

;
; Initialize PIT
;
; Input: Nothing
; Output: Nothing
;

init_pit:
push ax
mov al, 0x36
out 0x43, al ; channel 0, lowbyte / highbyte , square wave gen.

xor al, al
out 0x40, al
;call wait_for_pit
out 0x40, al
;call wait_for_pit

mov al, 0xb6
out 0x43, al ; channel 2, lowbyte / highbyte, square wave gen.

xor al, al
out 0x42, al
;call wait_for_pit
out 0x42, al
;call wait_for_pit
pop ax
ret

;
; Wait For PIT
;
; Input: Nothing
; Output: Nothing
;

;wait_for_pit:
;jmp $+0x02
;ret

%endif