;	TITL	"EEPROM Boot Loader for Elf Memebership Card"
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
		
; R0   program counter
; R1   subroutine pc
; R2   stack pointer
; R5.0 byte (read/write)
; R6.0 bit counter
; R6.1 carry bits
; R7   destination address 
; R8   length


BOOTLOADER
		LDI	080H		; set destination address
		PHI	R7
		LDI	080H		; set length
		PHI	R8
		GHI	R0		; D = 00H
		PLO	R7		; destination address begins at page
		PLO	R8		; lenght in pages
		PHI	R1		; high byte subroutine
		PLO	R2		; stack pointer = 0100H 
		LDI	01H
		PHI	R2
		LDI	LOW WRITEBYTE 	; low byte subroutine
		PLO	R1
		OUT	P1		; deactivate CS to start operation
		BYTE	00H
		
		SEX	R1		; for immediate OUT in subroutine 
		LDI	03H		; EEPROM read command
		SEP	R1		; CALL WRITEBYTE
		GHI	R0		; address bit 17 to 24 = 0
		SEP	R1		; CALL WRITEBYTE
		GHI	R0		; address bit 8 to 16 = 0
		SEP	R1		; CALL WRITEBYTE
		GHI	R0		; address bit 0 to 7 = 0
		SEP	R1		; CALL WRITEBYTE

		SEX	R6		; Rx for OUT
BLOCKLOOP	GHI	R0		; D = 0
		PLO	R5 		; reset all bits
		LDI	0FFH		; for the carry bits
		PHI	R6          
		LDI	0FFH - 8	; counting up 8 times
		PLO	R6		; bit counter
RDBITLOOP	OUT	P2		; CLK for SPI, INC Rx
		GHI	R6		; set CARRY
		SHRC
		GLO	R5
		B2	SETBIT		; branch if bit set
		SHL			; bit not set
		BR	SAVEBIT      
SETBIT  	SHLC
SAVEBIT  	PLO	R5
		GLO	R6
		BNZ	RDBITLOOP
		GLO	R5		; get byte
		STR	R7		; save byte		
		DEC	R8
		GLO	R8
		BNZ	BLOCKLOOP
		SEX	R2		; one page finished
		GHI	R8
		STR	R2
		OUT	P4		; show page count on LEDs
		DEC	R2
		BNZ	BLOCKLOOP-1
		OUT	P1		; deactivate CS to stop operation
		LBR	08000H

		SEP	R0
WRITEBYTE	PLO	R5		; save transmit byte
		LDI	8
		PLO	R6
WRBITLOOP	GLO	R5		; get the next bit
		SHLC			; next bit is in the carry
		PLO	R5
		LSNF
		OUT	P2
		BYTE	00000000B	; CLK for SPI with data bit cleared
		LSDF
		OUT	P2
		BYTE	00000001B	; CLK for SPI with data bit set 
		DEC	R6
		GLO	R6
		BNZ	WRBITLOOP
		BR	WRITEBYTE-1


		END



