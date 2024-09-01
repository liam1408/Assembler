.entry LIST 
.extern W
.define sz = 2./
MAIN:	mov r3, LIST[l]
	LOOP: 	jmp W
	prn #-5,7
	mov STR[5], STR[2],STR[1] 
	mcr macro
	sub r1, r4 .
	cmp K, #l
	bne W ,
	endmcr
	L1: 	inc L3 =
	.entry LOOP
	bne LOOP , HELLO
	macro
END: hlto
.define len = 
STR: .string "abcdef"ssa               
LIST: .data 6, -9, len,l 
K: .data 22 
.extern L3 

