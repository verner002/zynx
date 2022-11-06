;
; ZyNX Loader
;
; Author: Jakub Verner
; Date: 06-11-2022
;

cpu 486
bits 16
org 0x9000

%include "../defs.inc"

;
; .head
;

db "zexe"       ; Magic number
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

mov ax, 0xe801
xor cx, cx
xor dx, dx
int 0x15
jc panic16

cmp ah, 0x86
jz panic16

cmp ah, 0x80
jz panic16

jcxz .axbx

mov ax, cx
mov bx, dx

.axbx:
movzx eax, ax
movzx ebx, bx

shl ebx, 0x06

add ebx, eax
add ebx, 0x00000400

cmp ebx, 0x00001000 ; 4 MiB
jb panic16

push bx
mov ah, 0x03
xor bh, bh
int 0x10 ; get cur pos
pop bx

cli

call init_paging

lgdt [rodata.gdt_ptr]
lidt [rodata.idt_ptr]

mov eax, cr0
or eax, 0x80000001 ; enable pg and pe
mov cr0, eax

;
; EAX = CR0 | 0x80000001
; EBX = Memory size in KiB
; ECX = UNKNOWN
; EDX = UNKNOWN | Cursor position
; ESI = UKNOWN
; EDI = UKNOWN
;

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

mov esi, rodata.setting_handlers
call print_str32

push bx
mov al, 0x0d
mov bx, 0x0008
mov edx, gp_fault
call set_handler

mov al, 0x0e
mov bx, 0x0008
mov edx, page_fault
call set_handler
pop bx

mov esi, rodata.ok
call print_str32

mov esi, rodata.preparing_heap
call print_str32

mov eax, 0x00010000
mov edx, 0x00010000
call init_heap

mov esi, rodata.ok
call print_str32

mov esi, rodata.preparing_mem
call print_str32

sub ebx, 0x00000400
shl ebx, 0x0a ; to bytes
mov ecx, 0x00020000
call init_mem

mov esi, rodata.ok
call print_str32

mov esi, rodata.loading_hal
call print_str32

mov dx, 0x2820
call init_pics

call disable_irqs

call init_pit

call init_ps2

call disable_kbd_scan
jc panic32

call reset_kbd
jc panic32

call echo_kbd
jc panic32

mov dl, KBD_LED_NUM
call set_leds_kbd
jc panic32

mov al, 0x21
mov bx, 0x0008
mov edx, kbd_handler
call set_handler

mov cl, 0x01
call enable_irq

call enable_kbd_scan
jc panic32

mov esi, rodata.ok
call print_str32

sti ; it's save now to turn on the ints

;mov edi, 0x0000a000 ; curr page dir addr
;call crte_page_tbl

;mov eax, 0x000b8000
;mov ebx, 0x00400000
;mov di, 0x0002
;call map_page

jmp $

;
; General Protection Fault
;

gp_fault:
mov esi, rodata.gp_fault
call print_str32
jmp panic32

;
; Page Fault
;

page_fault:
mov esi, rodata.page_fault
call print_str32
jmp panic32

;
; Keyboard Handler
;
; Scan Code Set 2
;

kbd_handler:
pushad
cli
call read_kbd_ans
jc .return

cmp al, 0xf0 ; break code
jnz .make

call read_kbd_ans
jc .return
jmp .return

.make:
cmp al, 0xe7
jz .togg_scroll

cmp al, 0x77
jz .togg_num

cmp al, 0x58
jz .togg_caps

.return:
mov cl, 0x01
call send_eoi
sti
popad
iretd

.togg_scroll:
not byte [.scroll]
jmp .set_leds

.togg_num:
not byte [.num]
jmp .set_leds

.togg_caps:
not byte [.caps]
;jmp .set_leds

.set_leds:
mov dl, byte [.caps]
shl dl, 0x01
or dl, byte [.num]
shl dl, 0x01
or dl, byte [.scroll]
call set_leds_kbd
jmp .return

.scroll db 0x00
.num db 0x00
.caps db 0x00

;
; Panic
;

panic32:
mov esi, rodata.panic
call print_str32

halt32:
cli
hlt
jmp halt32

%include "hal32/vga.inc"
%include "hal32/pic.inc"
%include "hal32/pit.inc"
%include "hal32/ps2.inc"
%include "hal32/kbd.inc"
%include "hal32/idt.inc"

%include "slibs32/screen.inc"
%include "slibs32/heap.inc"
%include "slibs32/mem.inc"
;%include "slibs32/paging.inc"

;
; .rodata Section
;

rodata:
.enabling_a20 db "enabling a20... ", 0x00
.enabling_pm db "enabling pm... ", 0x00
.setting_handlers db "setting hndlrs... ", 0x00
.preparing_heap db "preparing heap... ", 0x00
.preparing_mem db "preparing mem... ", 0x00
.loading_hal db "loading hal... ", 0x00
.gp_fault db "#gp! ", 0x00
.page_fault db "#pf! ", 0x00
.ok db "ok", 0x0a, 0x0d, 0x00
.panic db "panic!", 0x0a, 0x0d, 0x00
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
.idt_ptr:
dw 0x07ff
dd 0x00020000

;align 0x1000, db 0x00