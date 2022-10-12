;
; Screen (32-bit) Static Library
;
; Author: Jakub Verner
; Date: 12-10-2022
;

%ifndef _SCREEN32
%define _SCREEN32

cur_pos:
.cur_x db 0x00
.cur_y db 0x00

;
; Get VGA Memory Size
;
; Input: Nothing
; Output: AX (vga mem sz in kib)
;

get_vga_mem_sz32:
ret

;
; Set Cursor Position
;
; Input: DH (row), DL (column)
; Output: Nothing
;

set_cur_pos32:
ret

;
; Print String (32-bit)
;
; Input: ESI (null-term char arr ptr)
; Output: Nothing
;

print_str32:
ret

%endif