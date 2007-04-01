;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³                                COS 2000                                 ³
;³                     http://www.multimania.com/cos2000                     ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³                            D I S K I M A G E                              ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³     Fonction    : Programme permettant de r‚aliser des images dique dur d'³
;³                 une disquettes.                                           ³
;³                                                                           ³
;³     Appel       : Diskimge [Nom du fichier][/r][/s][/h]                   ³
;³                       /r restaure l'image /s sauve l'image /h aide        ³
;³                                                                           ³
;³     Compilation : TASM  boot /m4 /x                                       ³
;³                   TLINK boot /t /x                                        ³
;ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
;³ Programmeur  : Nico                                                       ³
;³ Con‡us le    : 15/07/2001                                                 ³
;³ Modifi‚ le   : 15/07/2001                                                 ³
;³ Site Web     : http://www.multimania.com/cos2000                          ³
;³ Copyright    : libre distribution                                         ³
;³ E-Mail       : COS2000@multimania.com                                     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

;==========================Directive d'assemblages============================
[BITS 16]	;Directive qui autorise la g‚n‚ration de code 16 bits
[ORG 0x100]	;Directive pour indiquer le d‚but du code

section .text


;=============================Debut du programmes=============================

start:                          ;Label du d‚but.
        mov     ah,0x09
        mov     dx,msg   	;
        int     0x21
        mov     ah,'r'
        call    Check
        jnc     restitute
        mov     ah,'s'
        call    Check
        jnc     save
help:
        mov     ah,0x09
        mov     dx,helpmsg
        int     0x21
        mov     ax,0
        int     0x16
        ret
restitute:
        mov     ah,'n'
        call    Check
        jnc     nospl        
        mov     ah,0x09
        mov     dx,msg2
        int     21h
        mov     ax,0x00
        int     16h
nospl:
        mov     ah,0x09
        mov     dx,res
        int     0x21
        call    OpenCmdLine
        jc      error
        mov     si,buffer
        mov     di,si
        xor     cx,cx
restoring:
        call    Read18432
        jc      error          
        call    WriteTrack      ;Ecrit la piste CX sur les 2 tˆtes depuis ds:di
        jc      error 
        call    ShowDot
        inc     cx
        cmp     cx,80
        jb      restoring
        mov     ah,0x09
        mov     dx,msgok
        int     0x21
        ret          
save:     
        mov     ah,'n'
        call    Check
        jnc     nospl2        
        mov     ah,0x09
        mov     dx,msg2
        int     0x21
        mov     ax,0
        int     0x16
nospl2:
        mov     ah,0x09
        mov     dx,sav
        int     0x21
        call    CreateCmdLine
        jc      error
        mov     si,buffer
        mov     di,si
        xor     cx,cx
saving:
        call    ReadTrack      ;Lit la piste CX sur 2 tˆte en es:di
        jc      error
        call    Write18432
        jc      error
        call    ShowDot  
        inc     cx
        cmp     cx,80
        jb      saving  
        mov     ah,09
        mov     dx,msgok
        int     0x21
        ret          
error:
        push    cs
        pop     ds
        call    CloseCmdLine
        mov     ah,09
        mov     dx,errormsg
        int     0x21
        ret

;Affiche un point
ShowDot:
        push    ax
        push	bx
        push	cx
        mov     bx,cx
        shr     bx,4
        mov     al,[bx+dot]
        mov     ah,0x0E
        mov     bx,0x07
        int     0x10
        pop     cx
        pop	bx
        pop	ax
	ret

;Ecrit une piste CX 2 Tˆte, depuis le buffer DS:SI
ReadTrack:
        push    ax
	push	bx 
	push	cx 
	push	dx 
	push	si 
	push	es
        push    ds
        pop     es
        xchg    ch,cl
        xor     dx,dx
        xor     cl,cl
        inc     cl
        mov     bx,si
        mov     si,3
retry:
        mov     ax,0x0212
        int     13h
        jnc     done
        dec     si
        jnz     retry
        jmp     dead
done:
        add     bx,9216
        inc     dh
        cmp     dh,1
        je      retry
dead:
        pop     es
	pop	si
	pop	dx 
	pop	cx 
	pop	bx 
	pop	ax
        ret

;Lit une piste CX sur deux tˆte dans le buffer ES:DI
WriteTrack:
        push    ax 
	push	bx 
	push	cx 
	push	dx 
	push	si
        xchg    ch,cl
        xor     dx,dx
        xor     cl,cl
        inc     cl
        mov     bx,di
        mov     si,3
retry2:
        mov     ax,0x0312
        int     0x13
        jnc     done2
        dec     si
        jnz     retry2
done2:
        add     bx,9216
        inc     dh
        cmp     dh,1
        je      retry2
dead2:
        pop     si
	pop 	dx 
	pop	cx 
	pop	bx 
	pop	ax
        ret

;Ecrit 18432 octets depuis DS:SI dans le fichier ouvert par OpenCmdline
Write18432:
        push    ax
	push	bx 
	push	cx
	push	dx
        mov     ah,0x40
        mov     cx,18432
        mov     bx,[handle]
        mov     dx,si
        int     0x21
        pop     dx 
	pop	cx 
	pop	bx 
	pop	ax
        ret

;Lit 18432 octets vers ES:DI depuis le fichier ouvert par OpenCmdline
Read18432:
        push    ax 
	push	bx 
	push	cx 
	push	dx
        mov     ah,0x3F
        mov     cx,18432
        mov     bx,[handle]
        mov     dx,si
        int     0x21
        pop     dx
	pop	cx 
	pop	bx 
	pop	ax
        ret

;Ouvre la ligne de commande
OpenCmdLine:
        push    ax 
	push	bx 
	push	dx
        mov     bl,[0x80]
        xor     bh,bh
        add     bx,0x80
        mov     byte [bx],0
        mov     ax,0x3D00
        mov     dx,0x82
        int     0x21
        mov     [handle],ax
        pop     dx 
	pop	bx 
	pop	ax
        ret

;Cr‚e un fichier du nom de la ligne de commande
CreateCmdLine:
        push    ax 
	push	bx 
	push	cx 
	push	dx
        mov     bl,[0x80]
        xor     bh,bh
        add     bx,0x80
        mov     byte [bx],0
        mov     ah,0x3C
        xor     cx,cx
        mov     dx,0x82
        int     0x21
        mov     [handle],ax
        pop     dx 
	pop	cx 
	pop	bx 
	pop	ax
        ret

;Ferme le fichier pr‚c‚damment ouvert avec OpenCmdline
CloseCmdLine:
        push    ax 
	push	bx
        mov     bx,[handle]
        mov     ah,0x3E
        int     0x21
        pop     bx 
	pop	ax
        ret

;Recherche le commutateur AH dans la ligne de commande si existant retourne FC=True.
Check:
        push    ax
	push	cx
	push	di
        mov     di,0x80
        mov     cl,[di]
        cmp     cl,0
        je      notfound2
        xor     ch,ch
        mov     al,'/'
search:
        cmp     cx,0
        je      notfound
        repne   scasb
        cmp     [di],ah
        jnz     search
        jmp     okfound
notfound:
        sub     ah,'a'-'A'
        mov     di,0x80
        mov     cl,[di] 
search2:
        cmp     cx,0
        je      notfound2
        repne   scasb
        cmp     [di],ah
        jnz     search2
okfound:
        mov     word [di-2],0
        clc
        pop     di 
	pop	cx 
	pop	ax
        ret
notfound2:
        stc
        pop     di 
	pop	cx 
	pop	ax
        ret

res             db "Restauration en cours",0x0A,0x0D,0x0A,0x0D,'$'
sav             db "Sauvegarde en cours",0x0A,0x0D,0x0A,0x0D,'$'
dot             db " °±²Û"
handle          dw 0
msg             db 0x0A,0x0D,"IFlop V1.0",0x0A,0x0D
                db "Ecrit par Nico",0x0A,0x0D
                db "http://www.multimania.com/cos2000",0Ah,0Dh
                db "Copyright 2000",0Ah,0Dh,'$'
msg2            db "Ins‚rez une disquette de 1.44 Mo dans le lecteur et appuyez sur une touche...",0x0A,0x0D,'$'
errormsg        db 0Ah,0Dh,"Une erreur est apparue lors de la copie !",0x0A,0x0D,'$'
msgok           db 0Ah,0Dh,"La copie de l'image a ‚t‚ correctement r‚alis‚e",0x0A,0x0D,'$'

SectorsPerTrack dw 18           ;Nombre de secteur par pistes.
HeadsPerDrive   dw 2            ;Nombre de t‚tes par disque. 

helpmsg:
db "ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿"
db "³                                COS 2000                                      ³"
db "³                     http://www.multimania.com/cos2000                        ³"
db "ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´"
db "³                                I F L O P                                     ³"
db "ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´"
db "³     Fonction    : Programme permettant de r‚aliser des images disque dur     ³"
db "³                 d'une disquettes.                                            ³"
db "³                                                                              ³"
db "³     Appel       : Iflop [Nom du fichier][/r][/s][/h]                         ³"
db "³                       /r restaure l'image /s sauve l'image /h aide           ³"
db "³                                                                              ³"
db "³     Compilation : TASM  boot /m4 /x                                          ³"
db "³                   TLINK boot /t /x                                           ³"
db "ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´"
db "³ Programmeur  : Nico                                                          ³"
db "³ Con‡us le    : 15/07/2001                                                    ³"
db "³ Modifi‚ le   : 15/07/2001                                                    ³"
db "³ Site Web     : http://www.multimania.com/cos2000                             ³"
db "³ Copyright    : libre distribution                                            ³"
db "³ E-Mail       : COS2000@multimania.com                                        ³"
db "ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ$"

section .bss

[org 0x5000]

buffer          resb 512  ;Allocation de 512 octets pour contenir le secteur lu.