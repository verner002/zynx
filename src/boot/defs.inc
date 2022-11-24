;
; Definitions
;
; Author: Jakub Verner
; Date: 22-11-2022
;

%ifndef BOOT_ADDR
    %define BOOT_ADDR 0x7c00
%endif

%ifndef RELOC_ADDR
    %define RELOC_ADDR 0x0800
%endif

%ifndef HEAD_PTR
    %define HEAD_PTR sects_track-RELOC_ADDR+BOOT_ADDR
%endif

%ifndef HEAD_SIZE
    %define HEAD_SIZE copy_main-RELOC_ADDR-(HEAD_PTR-BOOT_ADDR)
%endif

%ifndef BOOT_SIZE
    %define BOOT_SIZE void-RELOC_ADDR
%endif

%ifndef BOOT_MAIN
    %define BOOT_MAIN boot_main+RELOC_ADDR-BOOT_ADDR
%endif

%ifndef SDA_PTR
    %define SDA_PTR 0x0500

    ;%ifndef DRV_NUM_PTR
    %define DRV_NUM_PTR SDA_PTR+0x0000
    ;%endif

    ;%ifndef SECTS_TRACK_PTR
    %define SECTS_TRACK_PTR SDA_PTR+0x0001
    ;%endif

    ;%ifndef HEADS_CYLD_PTR
    %define HEADS_CYLD_PTR SDA_PTR+0x0003
    ;%endif

    ;%ifndef SECTS_TOTAL_PTR
    %define SECTS_TOTAL_PTR SDA_PTR+0x0004
    ;%endif

    ;%ifndef SECTS_CLUST_PTR
    %define SECTS_CLUST_PTR SDA_PTR+0x0006
    ;%endif

    ;%ifndef SECTS_RES_PTR
    %define SECTS_RES_PTR SDA_PTR+0x0007
    ;%endif

    ;%ifndef SECTS_FAT_PTR
    %define SECTS_FAT_PTR SDA_PTR+0x0008
    ;%endif

    ;%ifndef LABEL_PTR
    %define LABEL_PTR SDA_PTR+0x0009
    ;%endif
%endif

%ifndef STACK_PTR
    %define STACK_PTR 0x0800
%endif

%ifndef FAT_PTR
    %define FAT_PTR 0x0e00
%endif

%ifndef KERNEL_PTR
    %define KERNEL_PTR 0x2000
%endif