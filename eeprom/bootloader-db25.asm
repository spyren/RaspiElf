;	TITL	"EEPROM Boot Loader for Elf Memebership Card, DB25"
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
; R4.0 carry bits
; R7   destination address 
; R8   length
; R9   start address

START
		LBR	BOOTLOADER
BOOTLOADER
		LDI	080H		; set destination address
		PHI	R7
		PHI	R9		; start address
		LDI	080H		; set length
		PHI	R8
		GHI	R0		; D = 00H
		PLO	R7		; destination address page boundry
		PLO	R8		; length in pages
		PLO	R9
		PHI	R1		; high byte subroutine
		PLO	R2		; stack pointer = 0100H 
		LDI	01H
		PHI	R2
		PLO	R4		; for the carry          
		LDI	LOW WRITEBYTE 	; low byte subroutine
		PLO	R1
		OUT	P4		; deactivate CS to cancel operation
		BYTE	00100000b
		OUT	P4		; activate CS to start operation
		BYTE	00000000b
		
		SEX	R1		; for immediate OUT in subroutine 
		LDI	03H		; EEPROM read command
		SEP	R1		; CALL WRITEBYTE
		GHI	R0		; address bit 16 to 23 = 0
		SEP	R1		; CALL WRITEBYTE, replace by NOP for
					; 1 to 512 Kibit EEPROMs
		GHI	R0		; address bit 8 to 15 = 0
		SEP	R1		; CALL WRITEBYTE, replace by NOP for
					; 1 to 4 Kibit EEPROMs
		GHI	R0		; address bit 0 to 7 = 0
		SEP	R1		; CALL WRITEBYTE

		SEX	R0		; Rx for OUT
BLOCKLOOP	GHI	R0		; D = 0
		PLO	R5 		; reset all bits
		LDI	8		; counting down 8 times
		PLO	R6		; bit counter
RDBITLOOP	GLO	R4		; set CARRY
		SHR
		GLO	R5		; get bits
		BN4	SETBIT		; branch if bit set (EF4 == 0)
		SHL			; bit not set
		SKP			; BR SAVEBIT
SETBIT  	SHLC			; set bit
SAVEBIT 	OUT	P4		; CLK on for SPI
 		BYTE	01000000B
		OUT	P4		; CLK off
		BYTE	00000000b
		PLO	R5		; save bits
		DEC	R6
		GLO	R6
		BNZ	RDBITLOOP
		GLO	R5		; get byte
		STR	R7		; save byte
		INC	R7		
		DEC	R8
		GLO	R8
		BNZ	BLOCKLOOP
		GHI	R8
		BNZ	BLOCKLOOP
		OUT	P4		; deactivate CS to stop operation
		BYTE	00100000B
WAIT		BR	WAIT

		SEX	R0
		INC	R8		; set R8 to 0001h
		GHI	R9		; store start address
		STR	R8
		INC	R8
		GLO	R9
		STR	R8
		BR	START

		SEP	R0
WRITEBYTE	PLO	R5		; save transmit byte
		LDI	8		; counter 8 bits
		PLO	R6
WRBITLOOP	GLO	R5		; get the next bit
		SHL			; next bit is in the carry
		PLO	R5
		BDF	BITSET
		OUT	P4		; bit cleared
		BYTE	00000000B
		OUT	P4	  	; clock on
		BYTE	01000000B 	
		OUT	P4		; clock off
		BYTE	00000000B
		BR	WRTEST
BITSET		OUT	P4		; bit set
		BYTE	10000000B
		OUT	P4		; clock on
		BYTE	11000000B
		OUT	P4		; clock off
		BYTE	10000000B 
WRTEST		DEC	R6
		GLO	R6
		BNZ	WRBITLOOP
		BR	WRITEBYTE-1


		END

