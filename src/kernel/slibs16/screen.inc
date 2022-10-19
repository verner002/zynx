;
; Screen (16-bit) Static Library
;
; Author: Jakub Verner
; Date: 12-10-2022
;

%ifndef _SCREEN16
%define _SCREEN16

;
; Print String (16-bit)
;
; Input: (DS:)SI (null-term char arr ptr)
; Output: Nothing
;

print_str16:
;push ax
;push bx
;push si
mov ah, 0x0e
xor bh, bh ; mov bx, 0x000f - for graphic mode
jmp .load_char

.print_char:
int 0x10

.load_char:
lodsb
or al, al
jnz .print_char

;pop si
;pop bx
;pop ax
ret


;
; Get Cursor Position
;
; Input: Nothing
; Output: Nothing
;

get_cur_pos:
ret

%endif