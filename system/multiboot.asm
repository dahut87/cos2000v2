;/*******************************************************************************/
;/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
;/*                                                                             */
[BITS 32]
		
SECTION .multiboot

%define MULTIBOOT_TAG_ALIGN 8
%define MULTIBOOT2_HEADER_MAGIC 0xe85250d6
%define MULTIBOOT_ARCHITECTURE_I386 0

%define MULTIBOOT_HEADER_TAG_OPTIONAL 1

%define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS 4

%define MULTIBOOT_TAG_TYPE_FRAMEBUFFER 8

%define MULTIBOOT_HEADER_TAG_END 0

%define MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED 2

%defstr arch ARCH

multiboot_header:
align MULTIBOOT_TAG_ALIGN
    dd MULTIBOOT2_HEADER_MAGIC      ; magic
    dd MULTIBOOT_ARCHITECTURE_I386  ; architecture
    dd multiboot_header_end - multiboot_header
    dd -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (multiboot_header_end - multiboot_header))
align MULTIBOOT_TAG_ALIGN
%if arch = "bits32"
%warning "Avec la console VGA/EGA." 
console_tag_start:
    dw MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS ; type
    dw MULTIBOOT_HEADER_TAG_OPTIONAL    ; flags
    dd console_tag_end - console_tag_start
    dd MULTIBOOT_CONSOLE_FLAGS_EGA_TEXT_SUPPORTED                             
console_tag_end:
%else
align MULTIBOOT_TAG_ALIGN
framebuffer_tag_start:
%warning "Avec le FRAMEBUFFER VESA." 
    dw MULTIBOOT_TAG_TYPE_FRAMEBUFFER ; type
    dw MULTIBOOT_HEADER_TAG_OPTIONAL    ; flags
    dd framebuffer_tag_end - framebuffer_tag_start
    dd 1024                            ; width
    dd 768                             ; height
    dd 32                              ; depth
%endif
align MULTIBOOT_TAG_ALIGN
framebuffer_tag_end: 
    dw MULTIBOOT_HEADER_TAG_END
    dw 0
    dd 8
multiboot_header_end:

SECTION .text

global start
extern main

start:
	push ebx
    push eax
	call main
	hlt
