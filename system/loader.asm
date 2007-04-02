[BITS 16]
[ORG 0x100]

	SECTION .text
		push 	cs
		push 	cs
		pop 	ds
		pop 	es
  call EnableA20
  mov si,msgpmode
		call showstr
		mov ax,cs	
		mov [RealCS],ax
		lea ax,[Real]
		mov [RealIP],ax
		
		xor eax,eax	
		mov ax,cs		
		shl eax,4
		mov [GDT.Entry1+2],ax
		mov [GDT.Entry2+2],ax
		mov [GDT.Entry4+2],ax
		mov [GDT.Entry5+2],ax
		shr eax,16
		mov [GDT.Entry1+4],al
		mov [GDT.Entry2+4],al
		mov [GDT.Entry4+4],al
		mov [GDT.Entry5+4],al

		xor eax,eax
		mov ax,cs		
		shl eax,4
		add eax,gdt0
		mov [GDTR.Address],eax
		lgdt [GDTR]

		cli
		;mov al,0xFF
		;out 0x21,al
		mov al,0x80	
		out 0x70,al

		mov eax,cr0	
		or al,1
		mov cr0,eax

		jmp SYS_CODE_SEL:GoPMode32

GoPMode32:
[BITS 32]
		mov ax,SYS_DATA_SEL
		mov ds,ax
                mov ecx,50000/4
                cld
		mov esi,kernel
                mov edi,0x100000
                mov ax,ALL_DATA_SEL
                mov es,ax
		mov gs,ax
		mov fs,ax
		mov ss,ax
		rep movsd
		mov ds,ax
		mov esp,0x3fffff
		call KERNEL_SEL:0x100000
		jmp REAL_CODE_SEL:GoReal

GoReal:
[BITS 16]
		mov ax,REAL_DATA_SEL
		mov ds,ax
		mov es,ax
		mov ss,ax
		mov gs,ax
		mov fs,ax
		mov esp,0xffff

		mov eax,cr0
		and al,0xFE
		mov cr0,eax

		jmp far [RealIP]

Real:
		jmp Real

msgpmode	db 'Pmode loader is loading',0

;======================SHOWSTR========================
;Affiche la chaine de caractère pointé par ds:si à l'écran
;-> DS, SI
;<- Flag Carry si erreur
;=====================================================
showstr:
	pusha
Next_Char:
        lodsb
        or	al,al
        jz	End_Show
        mov	ah,0x0E
        mov	bx,0x07
        int	0x10
        jmp	Next_Char
End_Show:
        popa
	ret
	

;***********************EnableA20*******************************
;-> 
;<-
;Ouvre l'autoroute A20	
;***************************************************************
EnableA20:
		cli
		call ClearKeybBuffer
		call WaitKeybCommand
		mov al,0xd1
		out 0x64,al
		call WaitKeybCommand
		mov al,0xdf
		out 0x60,al
		call WaitKeybCommand
		jmp A20Enabled
WaitKeybCommand:
		in al,0x64
		test al,0x02
		jnz WaitKeybCommand
		ret
ClearKeybBuffer:
		in al,0x64
		test al,0x01
		jnz ReadKeyb
		ret
ReadKeyb:	
		in al,0x60
		jmp ClearKeybBuffer
A20Enabled:
		sti
		ret
		
section .data

RealIP:	dw 0
RealCS:	dw 0

GDTR:
.Size:		dw GDT_END 
.Address:	dd 0 

	

gdt0 equ $			; null entry
GDT:
.Entry0:	dw 0		; limit 15:0
		dw 0		; base 15:0
		db 0		; base 23:16
		db 0		; type
		db 0		; limit 19:16, flags
		db 0		; base 31:24


SYS_CODE_SEL equ $-gdt0		; code segment descriptor

.Entry1:	dw 0xFFFF
		dw 0x0		; base
		db 0x0         	; base
		db 0x9A         ; present, ring 0, code, non-conforming, readable
		db 0x40   	; 32 bit
		db 0


SYS_DATA_SEL equ $-gdt0		; data segment descriptor

.Entry2:	dw 0xFFFF
		dw 0x0    	; base
		db 0x0	  	; base
		db 0x92         ; present, ring 0, data, expand-up, writable
		db 0x40		; 32 bit
		db 0


ALL_DATA_SEL equ $-gdt0	; 4meg data segment descriptor

.Entry3:	dw 0x03ff
		dw 0x0          ; base
		db 0x0          ; base
		db 0x92         ; present, ring 0, data, expand-up, writable
		db 0xcf		; 4k pages, 32 bit
		db 0


REAL_CODE_SEL equ $-gdt0	; code segment descriptor for 16 bit mode

.Entry4:	dw 0xFFFF
		dw 0x0   	; base
		db 0x0          ; base
		db 0x9A         ; present, ring 0, code, non-conforming, readable
		db 0x00   	; 16 bit
		db 0


REAL_DATA_SEL equ $-gdt0	; data segment descriptor for 16 bit mode

.Entry5:	dw 0xFFFF
		dw 0x0          ; base
		db 0x0          ; base
		db 0x92         ; present, ring 0, data, expand-up, writable
		db 0x00		; 16 bit
		db 0


KERNEL_SEL equ $-gdt0	; 4meg code segment descriptor

.Entry6:	dw 0x03ff
		dw 0x0          ; base
		db 0x0          ; base
		db 0x9A         ; present, ring 0, code, non-conforming, readable
		db 0xcf		; 4k pages, 32 bit
		db 0

GDT_END equ $-gdt0 -1

        	kernel:
        	
	SECTION .bss

