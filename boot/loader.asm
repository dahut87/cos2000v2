[BITS 16]
[ORG 0x100]

	SECTION .text
		push 	cs
		push 	cs
		pop 	ds
		pop 	es
		mov   si,msg0
		call showstr
        		
		mov   si,msg1
		call showstr
		call    initfat
call ok	
		
		mov   si,msg2
		call showstr	
        mov si,System_File	
		call    search
call ok	
		
		mov   si,msg3
		call showstr		
        call    EnableA20
call ok	
        	
		mov   si,msg4
		call showstr		
		mov eax,cs		
		shl eax,4
		mov [GDT.Entry1+2],ax
		mov [GDT.Entry2+2],ax
		shr eax,16
		mov [GDT.Entry1+4],al
		mov [GDT.Entry2+4],al
		mov eax,cs		
		shl eax,4
		add eax,gdt0
		mov [GDTR.Address],eax
		lgdt [GDTR]		
call ok	
        
	    mov   si,msg5
		call showstr
		cli
       mov eax,cr0
           or al,1
           mov cr0,eax
           jmp short $+2
           mov bx,SYS_DATA_SEL
           mov fs,bx              
           mov ds,bx
           mov es,bx
           mov gs,bx
           and al,0FEh
           mov cr0,eax
           jmp short $+2
           mov bx,cs
           mov fs,bx              
           mov ds,bx
           mov es,bx
           mov gs,bx           
           sti
call ok	
        
        
        		
		mov   si,msg6
		call showstr		
        mov  ebx,0x300000
		call    load
call ok		
        		
		mov   si,msg7
		call showstr	
		call    initpe
call ok	
		
		
		mov   si,msg8
		call showstr
		cli
		mov al,0x80	
		out 0x70,al
		mov eax,cr0	
		or al,1
		mov cr0,eax
		jmp SYS_CODE_SEL:GoPMode32
		
		
Boot_Error:
		mov   si,error
		call showstr 
        xor ax,ax
        int 16h
        jmp 0xFFFF:0x0 	
        
ok:
        push si 
        push cx
        mov cl,67
        call eol
		mov   si,okay
		call showstr
		pop cx 
        pop si
        ret	
        
;======================INITPE========================
;Initialise le PE de l'adresse EBX 
;-> 
;<- Flag Carry si erreur
;=====================================================              
initpe:
     pusha
     push es
     xor ax,ax
     mov es,ax
     mov [begin],ebx
     cmp word [es:ebx+mzheader.magic],"MZ"  ; bien l'entete .exe dos ?
     jne near errorpe
     add ebx,[es:ebx+mzheader.lfanew]  ; Pointe vers l'entete PE 
     cmp word [es:ebx+peheader.Signature],"PE"  ; bien l'entete PE ?    
     jne near errorpe
     cmp word [es:ebx+peheader.Signature+2],0  ; bien l'entete PE ?    
     jne near errorpe
     mov [p_pe],ebx
     mov esi,[es:ebx+peheader.ImageBase]
     mov [base],esi
     add esi,[es:ebx+peheader.AddressOfEntryPoint]
     mov [entriepoint],esi     
     mov ax,[es:ebx+peheader.NumberOfSections]
     add ebx,peheader.end
     mov [p_sections],ebx
     mov si,info1
     call showstr
readsections:  
     mov si,info2
     call showstr  
     mov esi,ebx 
     mov cx,8
     call showfixstr
     mov cl,17
     call eol   
     mov edx,[es:ebx+sections.VirtualAddress]
     add edx,[base]
     mov cx,32
     call ShowHex
     mov cl,30
     call eol 
     mov si,info3
     call showstr
     mov edx,[es:ebx+sections.VirtualSize]
     mov cx,32
     call ShowHex 
     mov si,info4
     call showstr    
     mov edi,[es:ebx+sections.VirtualAddress]
     add edi,[base]  
     cmp edi,0
     je  noload   
     push eax
     push ecx
     mov esi,[es:ebx+sections.PointerToRawData]
     add esi,[begin]
      mov ecx,[es:ebx+sections.SizeOfRawData]    
     shr cx,2
     cmp cx,0
     je zeroize
copietomem:
       mov eax,[es:esi]
       mov [es:edi],eax
       add edi,4
       add esi,4       
       dec cx
       jnz copietomem       
     pop ecx
     pop eax
    jmp nextsymb
zeroize:
      mov ecx,[es:ebx+sections.VirtualSize] 
      shr cx,2
      mov eax,0
zerotomem:
      mov [es:edi],eax
      add edi,4
       dec cx
       jnz zerotomem          
     pop ecx
     pop eax
     jmp  nextsymb    
noload:
     mov si,info5
     call showstr
nextsymb:
     mov si,return
     call showstr
    add ebx,sections.end
     dec ax
     jnz readsections
     mov si,info6
     call showstr
   mov edx,[entriepoint]
     mov cx,32
     call ShowHex   
     clc
     pop es
     popa
     ret     
errorpe:
    stc
    pop es
     popa
     ret
     
entriepoint dd 0
begin   dd 0    
base   dd 0    
p_pe       dd 0
p_sections dd 0		

;==========SHOWHEX===========
;Affiche un nombre hexadécimal EDX de taille CX aprés le curseur
;-> AH=10, EDX un entier, CX la taille
;<- 
;===========================================
ShowHex:
       push  ax
       push bx 
       push cx 
       push edx
       mov   ax,cx
	 shr   ax,2
       sub   cx,32
       neg   cx
       shl   edx,cl
       xor cx,cx
       inc cx
Hexaize:
       rol   edx,4
       mov   bx,dx
       and   bx,0fh
       push ax
       mov   al,[cs:bx+Tab]
       xor  bx,bx  
        mov	ah,0x09
        mov	bl,[cs:thecolor]
        int	0x10
        mov	ah,0x0E
        int	0x10 
        pop ax
       dec   al
       jnz   Hexaize
       pop   edx 
       pop cx 
       pop bx 
       pop ax
       ret
Tab db '0123456789ABCDEF'

;======================EOL========================
;Va en colonne CX
;-> 
;<- Flag Carry si erreur
;=====================================================              
eol:
  pusha
  mov bp,cx
    mov ah,03
    xor bx,bx
    int 10h
    mov ah,02
    mov cx,bp
    mov dl,cl
    int 10h
    popa
    ret


;======================LOAD========================
;Charge le groupe en mémoire en ebx
;-> 
;<- Flag Carry si erreur
;=====================================================              
load:
     push ax
     push ebx
     push ecx 
     push edx
     push esi 
     push edi
     push fs
      xor edx,edx
      xor ax,ax
      mov fs,ax
       mov	si,The_Dot
	   mov dx,[Sectors_Size]
Resume_Loading:
        cmp	cx,0x0FF0
        jae	Finish_Loading
        push	cx
        add	cx,word [data]
        mov edi,Buffer
        call	ReadSector
        pop	cx
        jc	near Boot_Error
	    call	showstr
        push esi 
        push ebx
        push cx
        mov esi,Buffer
        mov cx,dx
        shr cx,2
copie:
       mov eax,[ds:esi]
       mov [fs:ebx],eax
       add ebx,4
       add esi,4       
       dec cx
       jnz copie
        pop  cx
        pop  ebx
        pop  esi 
        add	ebx,edx 
        call	NextFatGroup
        jc	near Boot_Error
        jmp	Resume_Loading
Finish_Loading:
   pop fs
   pop edi
   pop esi
   pop edx
   pop ecx
   pop ebx
   pop ax
   ret

;======================INITFAT========================
;Initialise les variables de la fat
;-> 
;<- Flag Carry si erreur
;=====================================================
initfat:
        pusha
        push ds
        xor ax,ax
        mov ds,ax
        mov si,0x7C0B
        mov di,bootsector
        mov cx, 512
        rep movsb
        pop ds
	    mov	cx,[Reserved_Sectors]
        add	cx,[Sectors_Hidden]
        adc	cx,[Sectors_Hidden+2]
        mov	bx,[Sectors_Per_Fat]
        mov di,Fat_Buffer
        push bx
        push cx
readfat:
	    call     ReadSector
        jc       Boot_Error
	    inc      cx
	    add      di,[Sectors_Size]
	    dec      bx
	   jnz      readfat          
        pop cx
        pop bx
        xor	ax,ax
        mov	al,[Fats_Number]
        mul	bx
        add	cx,ax
        mov [entries],cx
        mov	ax,32
        mul	word [Fits_Number]
        div	word [Sectors_Size]
        add	cx,ax
        sub	cx,2
        mov	word [data],cx    
        popa
        ret
        
;======================SEARCH========================
;Recherche le groupe d'un fichier
;-> si nom du fichier
;<- Flag Carry si erreur CX
;=====================================================
search:
        push bx
        push dx 
        push di
        mov cx,[entries]
        mov	di,Buffer
        call ReadSector
        jc Boot_Error
        xor	bx,bx
Next_Root_Entrie:
        cmp	byte [di],0
        je	Boot_Error
        push    si
        push	di
        push	cx
        mov	cx,11
        rep	cmpsb
        pop	cx
        pop	di
        pop si
        je	Found
        add	di,32
        add	bx,32
        inc	dx
        cmp	dx,[Fits_Number]
        ja	Boot_Error
        cmp	bx,[Sectors_Size]
        jb	Next_Root_Entrie
        inc	cx
Found:
        mov	cx,[di+26]
       pop di 
       pop dx 
       pop bx
       ret
    


;======================SHOWSTR========================
;Affiche la chaine de caractère pointé par ds:si à l'écran
;-> DS, SI
;<- Flag Carry si erreur
;=====================================================
showstr:
        pusha
        xor  bh,bh
        xor cx,cx
        inc cx
nextchar:
        lodsb
        or	al,al
        jz	endshow
         cmp al,' '
        jb justchar       
        cmp al,'#'
        jne nocolor
        lodsb
        sub al,'0'
        shl al,3
        mov [cs:thecolor],al
        shr al,2
        add [cs:thecolor],al
        lodsb
        sub al,'0'
        add [cs:thecolor],al
        jmp	nextchar
nocolor:
        xor  bx,bx  
        mov	ah,0x09
        mov	bl,[cs:thecolor]
        int	0x10
justchar:
        mov	ah,0x0E
        int	0x10 
        jmp	nextchar
endshow:
        popa
	    ret
	    
;======================SHOWFIXEDSTR========================
;Affiche la chaine de caractère pointé par esi à l'écran pour CX caractères
;-> DS, SI
;<- Flag Carry si erreur
;=====================================================
showfixstr:
        pusha
        push ds
        xor ax,ax
        mov ds,ax
        xor  bh,bh
        mov dx,cx
        xor cx,cx
        inc cx
nextchars:
        mov al,[ds:esi]
        inc esi
        or	al,al
        jz	endshows
         cmp al,' '
        jb justchars       
        xor  bx,bx  
        mov	ah,0x09
        mov	bl,[cs:thecolor]
        int	0x10
justchars:
        mov	ah,0x0E
        int	0x10 
        dec dx
        jnz	nextchars
endshows:
pop ds
        popa
	    ret
	    
	    
;====================READSECTOR=======================
;Lit le secteur logique LBA CX et le met en es:di
;-> CX (limité à 65536 secteurs, soit 32 Mo avec secteur 512 octets)
;<- Flag Carry si erreur
;=====================================================
ReadSector:
	pusha
	mov	ax,cx
	xor	dx,dx
	div	word [Sectors_Per_Track]
	inc	dl
	mov	bl,dl
	xor	dx,dx
	div	word [Heads_Number]
	mov	dh, [Boot_Drive]
	xchg	dl,dh
	mov	cx,ax
	xchg	cl,ch
	shl	cl,6
	or	cl, bl
	mov	bx,di
	mov	si, 4
	mov	al, 1
Read_Again:
  	mov	ah, 2
  	int	0x13
  	jnc	Read_Done
  	dec	si
  	jnz	Read_Again
Read_Done:
  	popa
	ret
	
;===================NEXTFATGROUP======================
;Renvoie en CX le groupe qui succède dans la FAT le groupe CX
;-> CX
;<-
;=====================================================
NextFatGroup:
	push	bx
	push	dx
	push	di
	mov	ax,cx
	mov	bx,ax
	and	bx,0000000000000001b
	shr	ax,1
	mov	cx,3
	mul	cx
	mov	di,Fat_Buffer
	add	di,ax
	cmp	bx,0
	jnz	Even_Group
Odd_Group:
	mov	dx,[di]
	and	dx,0x0FFF
	mov	cx,dx
	jmp	Next_Group_Found
Even_Group:
	mov	dx,[di+1]
	and	dx,0xFFF0
	shr	dx,4
	mov	cx,dx
Next_Group_Found:
	pop	di
	pop	dx
	pop	bx
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
			

GoPMode32:
[BITS 32]
        mov ax,ALL_DATA_SEL
        mov es,ax
		mov gs,ax
		mov fs,ax
		mov ss,ax
		mov ds,ax
		mov esp,0x3fffff
		push KERNEL_SEL
		push dword [cs:entriepoint]
		retf
	;	jmp KERNEL_SEL:0x401020
		
section .data

thecolor db 0x07
msg0 db '#12-=< Lancement du Chargeur PE >=-',0x0A,0x0D,0x0A,0x0D,0
msg1 db '#07Initialisation de la FAT',0
msg2 db '#07Recherche du systeme',0
msg3 db '#07Activation adressage 24 bits',0
msg4 db '#07Chargement des descripteurs',0
msg5 db '#07Passage en mode Flat real',0
msg6 db '#07Chargement du systeme',0
msg7 db '#07Mise en place image PE',0
msg8 db '#07Passage en mode protege',0
msg9 db '#07Execution du noyau',0


info1 db 0x0A,0x0D,'#08Sections :',0x0A,0x0D,0
info2 db '    -',0
info3 db ' (',0
info4 db ') ',0
info5 db ' #12Non projete#08',0

info6 db 'Point entree en ',0

return db 0x0A,0x0D,0

okay db '    #15[  #10OK  #15]',0x0A,0x0D,0
error db '     #15[#12Erreur#15]',0x0A,0x0D,0x0A,0x0D,'    <Appuyez une touche pour redemarrer>',0
The_Dot db '.',0
System_File		db "SYSTEM  SYS"
entries dw 0
data dw 0
xy dw 0

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
		db 0xCF   	; 32 bit
		db 0


SYS_DATA_SEL equ $-gdt0		; data segment descriptor

.Entry2:	dw 0xFFFF
		dw 0x0    	; base
		db 0x0	  	; base
		db 0x92         ; present, ring 0, data, expand-up, writable
		db 0xCF		; 32 bit
		db 0


ALL_DATA_SEL equ $-gdt0	; 4meg data segment descriptor

.Entry3:	dw 0xFFFF
		dw 0x0          ; base
		db 0x0          ; base
		db 0x92         ; present, ring 0, data, expand-up, writable
		db 0xCF		; 4k pages, 32 bit
		db 0


KERNEL_SEL equ $-gdt0	; 4g code segment descriptor

.Entry4:	dw 0xffff
		dw 0x0          ; base
		db 0x0          ; base
		db 0x9A         ; present, ring 0, code, non-conforming, readable
		db 0xCF		; 4k pages, 32 bit
		db 0

GDT_END equ $-gdt0 -1

bootsector equ $
Sectors_Size		dw	0		;Nombre d'octets/secteur
Sectors_Per_Cluster	db	0		;Nombre de secteurs/cluster
Reserved_Sectors	dw	0		;Nombre de secteurs réservé
Fats_Number		    db	0		;Nombre de copies de la FAT
Fits_Number		    dw	0		;Taille du répertoire racine
Sectors_Per_Disk	dw	0		;Nombre secteurs du volume si < 32 Mo
Media_Descriptor	db	0		;Descripteur de média
Sectors_Per_Fat		dw	0		;Nombre secteurs/FAT
Sectors_Per_Track	dw	0		;Nombre secteurs/piste
Heads_Number		dw	0		;Nombre de tete de lecture/écriture
Sectors_Hidden		dd	0		;Nombre de secteurs cachés
Sectors_Per_Disk2	dd	0		;Nombre secteurs du volume si > 32 Mo
Boot_Drive		    db	0		;Lecteur de démarrage
Reserved		    db	0		;NA (pour NT seulement)
Extended_Boot_ID	db	0		;Signature Boot étendu 29h

Buffer            equ $
Fat_Buffer        equ $+512
        	
	SECTION .bss

%include "pe.h"

