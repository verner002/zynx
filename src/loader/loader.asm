;
; ZyNX Kernel
;
; Author: Jakub Verner
; Date: 24-11-2022
;

cpu 486
org 0x2000

;%define _DEBUG

%include "defs.inc"

;
; .text16 Section
;

bits 16

main16:
mov si, rodata.enabling_a20
call print_str16

call check_a20
jnc .continue

call enable_a20_bios

call check_a20
jnc .continue

call enable_a20_kbd

call check_a20
jnc .continue

call enable_a20_p92

call check_a20
jc panic16

.continue:
mov si, rodata.ok
call print_str16

mov si, rodata.loading_drivers
call print_str16

mov bx, 0x1000
mov es, bx
xor bx, bx ; ES:BX = 0x00010000

mov si, rodata.fdd_drv
call load_file
jc panic16

mov es, bx

mov si, rodata.ok
call print_str16

mov si, rodata.enabling_pm
call print_str16

mov ax, 0xe801 ; handles mem holes under 16 MiB
xor cx, cx
xor dx, dx
int 0x15
jc panic16

cmp ah, 0x86
jz panic16

cmp ah, 0x80
jz panic16

jcxz .axbx

;mov ax, cx
mov bx, dx

.axbx:
;movzx eax, ax
movzx ebx, bx

shl ebx, 0x06 ; mem size above 16 MiB
jz panic16

add ebx, 0x00004000 ; 16 MiB

cmp ebx, 0x00020000 ; 128 MiB
jb panic16

mov dword [MEM_SIZE_PTR], ebx

sub ebx, 0x00000004 ; subs 4 KiBs cuz of GDT limit

shr ebx, 0x02 ; to pages
mov word [rodata.gdt+0x0008], bx
mov word [rodata.gdt+0x0010], bx

shr ebx, 0x10
or bl, 11000000b
mov byte [rodata.gdt+0x0008+0x0006], bl
mov byte [rodata.gdt+0x0010+0x0006], bl

mov ah, 0x03
xor bh, bh
int 0x10 ; get cur pos

mov word [CUR_POS_PTR], dx

call turn_off_motor

cli

call init_paging

lgdt [rodata.gdt_ptr]
lidt [rodata.idt_ptr]

mov eax, cr0
or eax, 0x80050001 ; enable pg, pe and wr protect, disable align err
and eax, 0xffffffdf ; num exceps are proc by extern int
mov cr0, eax

;
; DX contains cur pos!!!
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
%include "slibs16/disk.inc"
%include "slibs16/paging.inc"

;
; .text32 Section
;

bits 32

main32:
mov ax, 0x0010
mov ds, ax
mov es, ax
mov ss, ax
mov esp, STACK_PTR

call init_vga
call enable_cur

call set_cur_pos

mov bl, 0x07
call set_color

mov esi, rodata.ok
call print_str

mov esi, rodata.setting_isrs
call print_str

mov al, 0x0d
mov bx, 0x0008
mov edx, gp_fault
call set_handler

mov al, 0x0e
mov bx, 0x0008
mov edx, page_fault
call set_handler

mov esi, rodata.ok
call print_str

mov esi, rodata.starting_pfa
call print_str

mov eax, 0x00010000
mov edx, 0x00010000
call init_heap

mov esi, rodata.ok
call print_str

;mov esi, rodata.preparing_mem
;call print_str

;sub ebx, 0x00000400
;shl ebx, 0x0a ; to bytes
;mov ecx, 0x00020000
;call init_mem

;mov esi, rodata.ok
;call print_str

mov esi, rodata.loading_hal
call print_str

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

mov dl, 0x03
call set_scan_kbd
jc panic32

call get_scan_kbd
jc panic32

cmp dl, 0x3f
jz .scan_code_set_ok

cmp dl, 0x03
jnz panic32

.scan_code_set_ok:
mov al, 0x21
mov bx, 0x0008
mov edx, kbd_handler
call set_handler

mov cl, 0x01
call enable_irq

call enable_kbd_scan
jc panic32

call init_dmacs

sti ; it's save now to turn on the ints

mov esi, rodata.ok
call print_str

;mov edi, 0x0000a000 ; curr page dir addr
;call crte_page_tbl

;mov eax, 0x000b8000
;mov ebx, 0x00400000
;mov di, 0x0002
;call map_page

.wait_for_key:
test byte [pressed], 0xff
jz .wait_for_key

movzx eax, byte [scan_code]

cmp al, 0x5a
jz .proc_enter

call print_hex32

.key_read:
mov byte [pressed], 0x00
jmp .wait_for_key

.proc_enter:
call print_nl
jmp .key_read

;
; General Protection Fault
;

gp_fault:
mov esi, rodata.gp_fault
call print_str
jmp panic32

;
; Page Fault
;

page_fault:
mov esi, rodata.page_fault
call print_str
jmp panic32

;
; Keyboard Handler
;
; Scan Code Set 3
;

kbd_handler:
pushad
cli
call read_kbd_ans
jc .return

cmp al, 0xf0 ; break code
jz .breakcode

cmp al, 0x5f
jz .togg_scroll

cmp al, 0x76
jz .togg_num

cmp al, 0x14
jz .togg_caps

cmp al, 0x12
jz .enable_shift

cmp al, 0x11
jz .enable_ctrl

cmp al, 0x19
jz .enable_alt

mov byte [scan_code], al
mov byte [pressed], 0xff

.return:
mov cl, 0x01
call send_eoi
sti
popad
iretd

.enable_shift:
mov byte [shift], 0xff
jmp .return

.enable_ctrl:
mov byte [ctrl], 0xff
jmp .return

.enable_alt:
mov byte [alt], 0xff
jmp .return

.breakcode:
call read_kbd_ans
jc .return

cmp al, 0x12
jz .disable_shift

cmp al, 0x11
jz .disable_ctrl

cmp al, 0x19
jz .disable_alt
jmp .return

.disable_shift:
mov byte [shift], 0x00
jmp .return

.disable_ctrl:
mov byte [ctrl], 0x00
jmp .return

.disable_alt:
mov byte [alt], 0x00
jmp .return

.togg_scroll:
not byte [scroll]
jmp .set_leds

.togg_num:
not byte [num]
jmp .set_leds

.togg_caps:
not byte [caps]
;jmp .set_leds

.set_leds:
mov dl, byte [caps]
shl dl, 0x02

mov bl, byte [num]
or bl, bl
jz .check_scroll

bts dx, 0x01

.check_scroll:
mov bl, byte [scroll]
or bl, bl
jz .set_them

bts dx, 0x00

.set_them:
call set_leds_kbd
jmp .return

scan_code db 0x00
pressed db 0x00

scroll db 0x00
num db 0xff
caps db 0x00

shift db 0x00
ctrl db 0x00
alt db 0x00

;
; Panic
;

panic32:
%ifdef _DEBUG
push esi
%endif
mov esi, rodata.panic
call print_str
%ifdef _DEBUG
pop esi
%endif

%ifdef _DEBUG
call print_hex32

call print_nl

mov eax, ebx
call print_hex32

call print_nl

mov eax, ecx
call print_hex32

call print_nl

mov eax, edx
call print_hex32

call print_nl

mov eax, esi
call print_hex32

call print_nl

mov eax, edi
call print_hex32

call print_nl

mov eax, ebp
call print_hex32

call print_nl

mov eax, esp
call print_hex32

call print_nl
%endif

halt32:
cli
hlt
jmp halt32

%include "hal32/vga.inc"
%include "hal32/idt.inc"
%include "hal32/pic.inc"
%include "hal32/pit.inc"
%include "hal32/ps2.inc"
%include "hal32/kbd.inc"
%include "hal32/dma.inc"

%include "slibs32/screen.inc"
%include "slibs32/heap.inc"
%include "slibs32/mem.inc"
;%include "slibs32/paging.inc"

;
; .rodata Section
;

rodata:
.enabling_a20 db "Enabling A20... ", 0x00
.loading_drivers db "Loading drivers... ", 0x00

.fdd_drv db "kernel  exe" ;"82077aa drv"

.enabling_pm db "Enabling PM... ", 0x00
.setting_isrs db "Setting ISRs... ", 0x00
.starting_pfa db "Starting PMA... ", 0x00
;.starting_mem db "Starting PFA... ", 0x00
.loading_hal db "Loading HAL... ", 0x00
.gp_fault db "General Protection Fault! ", 0x00
.page_fault db "Page Fault! ", 0x00
.ok db "Ok", 0x0a, 0x0d, 0x00
.panic db "Panic!", 0x00

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