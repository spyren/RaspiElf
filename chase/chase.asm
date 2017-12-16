;	TITL	"Chase Lighting for Elf Memebership Card"
;		EJCT	60

		CPU	1802

NUMBER		EQU	0

;
; Register Definitions:
;
R0		EQU	0
R1		EQU	1
R2		EQU	2
R3		EQU	3
R4		EQU	4
R5		EQU	5
R6		EQU	6
R7		EQU	7
R8		EQU	8
R9		EQU	9
R10		EQU	10
R11		EQU	11
R12		EQU	12
R13		EQU	13
R14		EQU	14
R15		EQU	15

;
; I/O Port Definitions:
;
P1		EQU	1
P2		EQU	2
P3		EQU	3
P4		EQU	4
P5		EQU	5
P6		EQU	6
P7		EQU	7	

		ORG	0H
		
		LBR 	MOVE
	
MOVE
		SEX 	R3		; index register R3 for bit pattern point to 30H
		GHI 	R0		; PC high byte is 0
		PHI 	R3
		LDI 	30H
		PLO 	R3

		LDI	00000001B	; bit pattern to move in (0030H)
		STR 	R3
LOOP
		OUT	P4		; Rx = 0030H, INC Rx
		INP	P4		; get switches Rx =0031H
		DEC 	R3		; Rx = 0030H
		ANI 	11111110B
		BNZ 	LEFT
RIGHT
		LDX			; get pattern
		SHR
		LSNF
		LDI 	10000000B   
		BR  	STORE
LEFT
		LDX			; get pattern
		SHL
		LSNF
		LDI 	00000001B
STORE
		STR 	R3

		LDI 	20
		PHI 	R2		; R2 wait counter
WAIT
		DEC 	R2		; wait loop about 0.5 s
		GHI 	R2
		BNZ 	WAIT

		LSNQ      		; toggle Q
		REQ
		NBR	7bH		; SEQ 
		;SEQ
		BR 	LOOP

		END
