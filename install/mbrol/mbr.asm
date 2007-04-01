[BITS 16]
[ORG 0x7C00]
                                
start:
        jmp     short falsereal  ;vers debut programme
        db      "MBROL"         ;Signature MBROL
falsereal:
        cli                     ;inhibation interruptions
        xor     ax,ax           
        mov     ss,ax           ;Reloge pile en 0000h:9000h
        mov     sp,0x9000
        push    ax
        pop     ds              ;DS=0000h
        add     ah,0x10          
        push    ax
        pop     es              ;ES=1000h
        sti                     ;Reactivation des interruptions
	cld
        mov    	[sitemp],si     ;sauvegarde si (adresse partition active)
        mov     di,0x7C00
        mov     si,di
        mov     cx,512/4
        rep     movsd           ;On reloge le programme en 1000h:7C00h
        push    es
        pop     ds              ;DS=1000h
	jmp	0x1000:RealStart

RealStart:                      
        xor     dh,dh
        mov     bp,dx           ;R‚cupŠre en BP le nølecteur de d‚marrage
        call    initdisk        ;Initialisation du p‚riph‚rique
        ;call    cls             ;Efface l'‚cran
        mov     si, msg
        call    showstr         ;Affiche le splatch de boot
        call    getkey          ;R‚cupŠre la touche press‚ par l'utilisateur
        cmp     al,' '          ;touche = espace
        jne     normal          ;si autre touche on démarre normalement
        mov     si, msgmbrol
        call    showstr         ;Message de chargement de Cos loader
        xor     ecx,ecx
        inc     ecx             
        mov     di,0x0100        
readload:
        inc     ecx    
        call    readsector      ;Charge le secteur LBA ecx en es:di
        add     di,512          
        cmp     ecx,7
        jb      readload        ;Charge les secteurs LBA 2 … 6 en 1000h:0100h
        
	jmp	0x1000:0x0100   ;Execute Cos loader
				;saut FAR vers 1000h:0100h

normal:
        mov     si,msgnorm
        call    showstr         ;Affiche le d‚marrage normal
        mov     ecx,1
        xor     ax,ax
        mov     es,ax
        mov     ds,ax
        mov     di,0x7C00       ;Charge le secteur LBA 1 en 0000h:7C00h
        call    readsector      ;qui est le MBR d'origine sauvegard‚ par MBROL
        mov     dx,bp           ;Renvoie dans DL le lecteur de d‚marrage
        mov     si,[sitemp]     ;restaure si (adresse de la partition)
        jmp	0x0000:0x7C00	;Une op‚ration normalement r‚alis‚e par le BIOS
				;Saut vers le MBR en 0000h:7C00h  
  
temp            db 0
sitemp          dw 0
HeadsPerDrive   dw 16
SectorsPerTrack dw 38
msg             db "Cos2000 MBROL V1.0",0x0A,0x0D,"Press [SPACE] to execute Cos Loader",0x0A,0x0D,0 
point           db '.',0
msgmbrol        db "Cos Loader is loading",0
msgnorm         db "Booting...",0

;=============INITDISK===============
;Initialise le p‚riph‚rique BP pour une utilisation ult‚rieure
;-> BP
;<- 
;====================================
initdisk:
        push 	ax 
	push	bx 
	push	cx 
	push	dx 
	push	di 
	push	es
        mov     dx,bp
        mov     ah,8
        int     0x13
        and     cx,111111b
        mov     [cs:SectorsPerTrack],cx
        mov     cl,dh
        inc     cl
        mov     [cs:HeadsPerDrive],cx
        pop     es
	pop	di 
	pop	dx 
	pop	cx 
	pop	bx 
	pop	ax
        ret    

;=============READSECTOR (Fonction 01H)===============
;Lit le secteur ECX du disque BP et le met en es:di
;-> AH=1
;<- Flag Carry si erreur
;=====================================================
readsector:
        push    ax 
	push	bx 
	push	ecx 
	push	dx 
	push	si
        mov 	ax,bp
        mov 	[cs:temp],al
	mov 	ax, cx
        ror 	ecx, 16
	mov 	dx, cx
        rol 	ecx,16
        div 	word [cs: SectorsPerTrack]
	inc 	dl
	mov	bl, dl
        xor     dx,dx                  
        div 	word [cs: HeadsPerDrive]
        mov     dh, [cs:temp]
	xchg 	dl, dh
	mov 	cx, ax
	xchg 	cl, ch
	shl 	cl, 6
        or      cl, bl      
	mov 	bx, di
	mov 	si, 4
	mov 	al, 1
TryAgain:
	mov 	ah, 2
        int     13h
        push 	si
        mov 	si,point
        call 	showstr
        pop si
        jnc Done
	dec si
        jnz TryAgain
Done:
        pop     si
	pop	dx
	pop	ecx
	pop	bx
	pop	ax
        ret

;============CLS==============
;Efface l'‚cran
;-> 
;<-
;=============================
cls:
        push 	ax
        mov 	ax,0x0003
        int 	0x10
        pop 	ax
        ret

;==========SHOWSTR============
;Affiche une chaine de caractŠre point‚ par SI
;-> SI pointe une chaine
;<-
;=============================
showstr:
        push 	ax 
	push	bx 
	push	si
again:
	lodsb
        or 	al,al
	jz 	fin
	CALL 	showchar
	jmp 	again
fin:
        pop 	si
        pop	bx
        pop	ax
	ret

;===========GETKEY=============
;N'Attend l'appuie d'une touche et
;renvoie en AL la touche appuyer
;-> 
;<- AL
;==============================
getkey:
        mov     ah,0x01
        int     0x16
        ret

;==========SHOWCHAR============
;Affiche un caractŠre point‚ dans AL
;-> AL
;<-
;==============================
showchar:
        push 	ax 
	push	bx
        mov 	ah,0x0E
        mov 	bx,0x07
        int 	0x10
        pop 	bx
	pop	ax
	ret

times 510-($-$$) db 0

	 dw	0xAA55