;
; Definitions
;
; Author: Jakub Verner
; Date: 01-11-2022
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

%ifndef KBD_LED_SCROLL
%define KBD_LED_SCROLL 0x01
%endif

%ifndef KBD_LED_NUM
%define KBD_LED_NUM 0x02
%endif

%ifndef KBD_LED_CAPS
%define KBD_LED_CAPS 0x04
%endif