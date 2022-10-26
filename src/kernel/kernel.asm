;
; ZyNX Kernel
;
; Author: Jakub Verner
; Date: 22-10-2022
;

cpu 486
bits 16
org 0x9000

;
; .head
;

db "ZeXE"       ; Magic number
db 0x19         ; Type: 16-bit executable (0x29 = 32-bit executable)
dw 0x0386       ; Machine: Intel 80386
dw 0x0000       ; Version: 0.0.0.0
dw main16       ; Entry point: main16

;
; .text16 Section
;

main16:
mov si, rodata.enabling_a20
call print_str16

call check_a20
jnc .continue

call enable_a20
call check_a20
jc panic16

.continue:
mov si, rodata.ok
call print_str16

mov si, rodata.enabling_pm
call print_str16

mov ah, 0x03
xor bh, bh
int 0x10 ; get cur pos

cli

call init_paging

lgdt [rodata.gdt_ptr]

mov eax, cr0
or eax, 0x80000001 ; enable pg and pe
mov cr0, eax

jmp 0x0008:main32

;
; Panic
;

panic16:
mov si, rodata.panic
call print_str16
;jmp halt16

halt16:
cli
hlt
jmp halt16

%include "slibs16/screen.inc"
%include "slibs16/mem.inc"
%include "slibs16/paging.inc"

bits 32

;
; .text32 Section
;

main32:
mov ax, 0x0010
mov ds, ax
mov es, ax
mov ss, ax
mov esp, 0x00007c00

call init_vga
call enable_cur

call set_cur_pos32

mov esi, rodata.ok
call print_str32

mov esi, rodata.initing_heap
call print_str32

mov eax, 0x00007c00
mov edx, 0x00000010
call init_heap

mov esi, rodata.ok
call print_str32

mov esi, rodata.loading_hal
call print_str32

mov dx, 0x2820
call init_pics
call disable_irqs

call init_pit

mov esi, rodata.ok
call print_str32

halt32:
cli
hlt
jmp halt32

%include "hal32/vga.inc"
%include "hal32/pic.inc"
%include "hal32/pit.inc"

%include "slibs32/screen.inc"
%include "slibs32/mem.inc"

;
; .rodata Section
;

rodata:
.enabling_a20 db "Enabling A20... ", 0x00
.enabling_pm db "Enabling PM... ", 0x00
.initing_heap db "Preparing heap... ", 0x00
.loading_hal db "Loading HAL... ", 0x00
.ok db "ok", 0x0a, 0x0d, 0x00
.panic db "panic", 0x0a, 0x0d, 0x00
.gdt:
; null desc
dw 0x0000       ; limit
dw 0x0000       ; base
db 0x00         ; base
db 00000000b    ; access byte
db 00000000b    ; flags and limit
db 0x00         ; base

; code desc
dw 0xffff       ; limit
dw 0x0000       ; base
db 0x00         ; base
db 10011010b    ; access byte
db 11001111b    ; flags and limit
db 0x00         ; base

; data desc
dw 0xffff       ; limit
dw 0x0000       ; base
db 0x00         ; base
db 10010010b    ; access byte
db 11001111b    ; flags and limit
db 0x00         ; base
.gdt_ptr:
dw $-.gdt-0x0001
dd .gdt

;align 0x1000, db 0x00