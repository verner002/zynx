;
; Definitions
;
; Author: Jakub Verner
; Date: 22-11-2022
;

%ifndef NULL
    %define NULL 0x00000000
%endif

%ifndef FALSE
    %define FALSE 0x00
%endif

%ifndef TRUE
    %define TRUE !FALSE
%endif

%ifndef SDA_PTR
    %define SDA_PTR 0x00000500

    ;%ifndef DRV_NUM_PTR
    %define DRV_NUM_PTR SDA_PTR+0x00000000
    ;%endif

    ;%ifndef SECTS_TRACK_PTR
    %define SECTS_TRACK_PTR SDA_PTR+0x00000001
    ;%endif

    ;%ifndef HEADS_CYLD_PTR
    %define HEADS_CYLD_PTR SDA_PTR+0x00000003
    ;%endif

    ;%ifndef SECTS_TOTAL_PTR
    %define SECTS_TOTAL_PTR SDA_PTR+0x00000004
    ;%endif

    ;%ifndef SECTS_CLUST_PTR
    %define SECTS_CLUST_PTR SDA_PTR+0x00000006
    ;%endif

    ;%ifndef SECTS_RES_PTR
    %define SECTS_RES_PTR SDA_PTR+0x00000007
    ;%endif

    ;%ifndef SECTS_FAT_PTR
    %define SECTS_FAT_PTR SDA_PTR+0x00000008
    ;%endif

    ;%ifndef MEM_SIZE_PTR
    %define MEM_SIZE_PTR SDA_PTR+0x00000009
    ;%endif

    %ifndef CUR_POS_PTR
        %define CUR_POS_PTR SDA_PTR+0x0000000d
        
        ;%ifndef CUR_X_PTR
        %define CUR_X_PTR CUR_POS_PTR+0x00000000
        ;%endif

        ;%ifndef CUR_Y_PTR
        %define CUR_Y_PTR CUR_POS_PTR+0x00000001
        ;%endif
    %endif

    ;%ifndef COLOR_PTR
    %define COLOR_PTR SDA_PTR+0x0000000f
    ;%endif
%endif

%ifndef STACK_PTR
    %define STACK_PTR 0x00000e00
%endif

%ifndef FAT_PTR
    %define FAT_PTR 0x00000e00
%endif

%ifndef KBD_LED_SCROLL
    %define KBD_LED_SCROLL 0x01
%endif

%ifndef KBD_LED_NUM
    %define KBD_LED_NUM 0x02
%endif

%ifndef KBD_LED_CAPS
    %define KBD_LED_CAPS 0x04
%endif