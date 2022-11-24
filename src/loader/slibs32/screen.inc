;
; Screen (32-bit) Static Library
;
; Author: Jakub Verner
; Date: 22-11-2022
;

%ifndef _SCREEN32
%define _SCREEN32

;
; Global Variables
;

_80x25_text:
; misc
db  0x67
; seq
db  0x03, 0x00, 0x03, 0x00, 0x02
; crtc
db  0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f
db  0x00, 0x4f, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x50
db  0x9c, 0x0e, 0x8f, 0x28, 0x1f, 0x96, 0xb9, 0xa3
db  0xff
; gc
db  0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0e, 0x00
db  0xff
; ac
db  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07
db  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
db  0x0c, 0x00, 0x0f, 0x08, 0x00

;_90x60_text:
; misc
;db  0xe7
; seq
;db  0x03, 0x01, 0x03, 0x00, 0x02
; crtc
;db  0x6b, 0x59, 0x5a, 0x82, 0x60, 0x8d, 0x0b, 0x3e
;db  0x00, 0x47, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00
;db  0xea, 0x0C, 0xdf, 0x2d, 0x08, 0xe8, 0x05, 0xa3
;db  0xff
; gc
;db  0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0e, 0x00
;db  0xff
; ac
;db  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07
;db  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
;db  0x0c, 0x00, 0x0f, 0x08, 0x00

;
; Set Cursor Position
;
; Input: DH (row), DL (column)
; Output: Nothing
;

set_cur_pos:
mov word [CUR_POS_PTR], dx
call write_cur_loc_reg
ret

;
; Get Cursor Position
;
; Input: Nothing
; Output: DH (row), DL (column)
;

;get_cur_pos:
;mov dx, word [CUR_POS_PTR]
;ret

;
; Set Foreground & Background Color
;
; Input: BL (fb col)
; Output: Nothing
;

set_color:
mov byte [COLOR_PTR], BL
ret

;
; Get Foreground & Background Color
;
; Input: Nothing
; Output: BL (fb col)
;

;get_color:
;mov bl, byte [COLOR_PTR]
;ret

;
; Scroll Down
;
; Input: Nothing
; Output: Nothing
;

scroll_down:
pushad
mov edi, 0x000b8000
mov esi, edi
add esi, 0x000000a0

mov cx, 0x0780
rep movsw

mov ah, byte [COLOR_PTR]
mov al, 0x20
mov cx, 0x50
rep stosw
popad
ret

;
; Print Char
;
; Input: AL (char ascii code)
; Output: Nothing
;

print_char:
push eax
push ebx
push dx
mov dx, word [CUR_POS_PTR]

cmp al, 0x20
jb .special_char

cmp al, 0x7e
ja .special_char

push ax
movzx eax, dh
mov ebx, 0x00000050
mul bl
mov bl, dl
add ebx, eax
shl ebx, 0x01
add ebx, 0x000b8000
pop ax

mov ah, byte [COLOR_PTR]
mov word [ebx], ax

cmp dl, 0x4f
jnb .print_nlcr

inc dl

.set_cur_pos:
call set_cur_pos

.return:
pop dx
pop ebx
pop eax
ret

.special_char:
cmp al, 0x09
jz .print_tab

cmp al, 0x0a
jz .print_nl

cmp al, 0x0d
jnz .return

xor dl, dl
jmp .set_cur_pos

.print_tab:
mov al, 0x20

call print_char
call print_char
call print_char

jmp .return

.print_nlcr:
xor dl, dl

.print_nl:
cmp dh, 0x18
jnb .scroll_down

inc dh
jmp .set_cur_pos

.scroll_down:
call scroll_down
jmp .set_cur_pos

;
; Print String
;
; Input: ESI (null-term char arr ptr)
; Output: Nothing
;

print_str:
push ax
push esi
jmp .load_char

.print_char:
call print_char

.load_char:
lodsb
or al, al
jnz .print_char

pop esi
pop ax
ret

;
; Print New Line
;
; Input: Nothing
; Output: Nothing
;

print_nl:
push ax
mov al, 0x0a
call print_char

mov al, 0x0d
call print_char
pop ax
ret

;
; Print Hexadecimal 32
;
; Input: EAX (num)
; Output: Nothing
;

print_hex32:
push ecx
mov ecx, 0x00000008

.print_digit:
rol eax, 0x04

push ax
and al, 0x0f

cmp al, 0x0a
jb .num

add al, 'a'-'0'-0x0a

.num:
add al, '0'
call print_char
pop ax

loop .print_digit
pop ecx
ret

%endif