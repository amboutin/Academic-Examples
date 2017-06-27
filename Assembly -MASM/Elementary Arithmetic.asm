TITLE Elementary Arithmetic     (assn1.asm)

; Author: Aaron Boutin
; CS271-400 / Assignment 1               Date: 03/JUL/16
; Description: Program prompts the user for two values, then displays their sum, difference, product, and quotient with remainder.

INCLUDE Irvine32.inc

; (insert constant definitions here)

.data

; (insert variable definitions here)
title_1	BYTE	"		Elementary Arithmetic		Aaron Michael Boutin",0	
intro	BYTE	"Please, enter two numbers with which I will calculate and display the sum, difference, product, quotient and remainder.",0
extraC_1	BYTE	"**EC: Program loops until user chooses to quit",0
extraC_2	BYTE	"**EC: Program verifies that the second user-defined input is less than the first input",0
extraC_3	BYTE	"**EC: Program displays division as a floating point value to 3 decimal places.",0 
prompt_1	BYTE	"Please enter a number. ",0
prompt_2	BYTE	"Please enter another number. ",0
prompt_3	BYTE	"Would you like to try again? Enter 1 if so, otherwise enter 0. ",0
err_prompt	BYTE	"The second number must be less than the first number.",0
remainder	BYTE	" remainder ",0
input_1		DWORD	33 DUP(0)	
input_2		DWORD	33 DUP(0)
input_3		DWORD	5 DUP(0)	
sumVal		DWORD	?
diffVal		DWORD	?
multVal		DWORD	?
divVal		DWORD	?
dValRem		DWORD	?
goodbye		BYTE	"Thank you for your time, Good-Bye!",0

r1			DWORD	?
r2			DWORD	?
r3			DWORD	?
d1			DWORD	?
d2			DWORD	?
d3			DWORD	?
d4			DWORD	?
dot			BYTE	".",0

; operators for display
plus	DWORD	" + ",0
minus	DWORD	" - ",0
multi	DWORD	" X ",0
divide	DWORD	" / ",0
equal	DWORD	" = ",0


.code
main PROC
L1:
; introduce title
	mov		edx, OFFSET title_1
	call	WriteString
	call	CrLf

; extra credit done
	mov		edx, OFFSET extraC_1
	call	WriteString
	call	CrLf
	mov		edx, OFFSET extraC_2
	call	WriteString
	call	CrLf
	mov		edx, OFFSET extraC_3
	call	WriteString
	call	CrLf
	call	CrLf

; introduce purpose
	mov		edx, OFFSET intro
	call	WriteString
	call	CrLf
	call	CrLf

;get user input
	mov		edx, OFFSET prompt_1
	call	WriteString
	call	ReadDec
	mov		input_1, eax
	call	CrLf

	mov		edx, OFFSET prompt_2
	call	WriteString
	call	ReadDec
	mov		input_2, eax
	call	CrLf

; test if second number is less than first number
	mov		edx, input_2	
	cmp		edx, input_1
	jb		L2
	mov		edx, OFFSET err_prompt
	call	WriteString
	call	CrLf
	call	CrLf
	jmp		L3

	L2:
; calculate results
	mov		eax, input_1
	add		eax, input_2
	mov		sumVal, eax

	mov		eax, input_1
	sub		eax, input_2
	mov		diffVal, eax

	mov		eax, input_1
	mov		ebx, input_2
	mul		ebx
	mov		multVal, eax

	mov		edx, 0
	mov		eax, input_1
	mov		ebx, input_2
	div		ebx
	mov		divVal, eax
	mov		dValRem, edx
; d1
	mov		eax, dValRem
	mov		ebx, 10
	mul		ebx
	mov		edx, 0
	mov		ebx, input_2
	div		ebx
	mov		d1, eax
	mov		r1, edx
; d2
	mov		eax, r1
	mov		ebx, 10
	mul		ebx
	mov		edx, 0
	mov		ebx, input_2
	div		ebx
	mov		d2, eax
	mov		r2, edx
; d3
	mov		eax, r2
	mov		ebx, 10
	mul		ebx
	mov		edx, 0
	mov		ebx, input_2
	div		ebx
	mov		d3, eax
	mov		r3, edx
; d4
	mov		eax, r3
	mov		ebx, 10
	mul		ebx
	mov		edx, 0
	mov		ebx, input_2
	div		ebx
	mov		d4, eax




; display results

; display sum
	mov		eax, input_1
	call	WriteDec
	mov		edx, OFFSET plus
	call	WriteString
	mov		eax, input_2
	call	WriteDec
	mov		edx, OFFSET equal
	call	WriteString
	mov		eax, sumVal
	call	WriteDec
	call	CrLf

; display difference
	mov		eax, input_1
	call	WriteDec
	mov		edx, OFFSET minus
	call	WriteString
	mov		eax, input_2
	call	WriteDec
	mov		edx, OFFSET equal
	call	WriteString
	mov		eax, diffVal
	call	WriteDec
	call	CrLf

; display multiplication
	mov		eax, input_1
	call	WriteDec
	mov		edx, OFFSET multi
	call	WriteString
	mov		eax, input_2
	call	WriteDec
	mov		edx, OFFSET equal
	call	WriteString
	mov		eax, multVal
	call	WriteDec
	call	CrLf

; display division
	mov		eax, input_1
	call	WriteDec
	mov		edx, OFFSET divide
	call	WriteString
	mov		eax, input_2
	call	WriteDec
	mov		edx, OFFSET equal
	call	WriteString
	mov		eax, divVal
	call	WriteDec
	mov		edx, OFFSET remainder
	call	WriteString
	mov		eax, dValRem
	call	WriteDec
	call	CrLf


		mov		eax, divVal
	call	WriteDec
	mov		edx, OFFSET dot
	call	WriteString
	mov		eax, d1
	call	WriteDec
	mov		eax, d2
	call	WriteDec
	mov		eax, d4
	cmp		eax, 5
	jb		L5
	inc		d4
L5:		
	mov		eax, d4
	call	WriteDec
	call	CrLf
	call	CrLf

L3:
	mov		edx, OFFSET prompt_3
	call	WriteString
	call	ReadDec
	mov		input_3, eax
	call	CrLf
	cmp		input_3,1
	je		L1

; say "Good-bye"
	mov		edx, OFFSET goodbye
	call	WriteString
	call	CrLf	

	exit	; exit to operating system
main ENDP

; (insert additional procedures here)

END main
