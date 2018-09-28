;/*******************************************************************************/
;/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
;/*                                                                             */
struc echs
Sizes    	resb 1
Reserve 	resb 1
NumSectors 	resw 1
Adressoff 	resw 1
Adressseg 	resw 1
SectorLow 	resw 1
SectorHigh 	resw 1
Dummy 		resq 1
endstuc
