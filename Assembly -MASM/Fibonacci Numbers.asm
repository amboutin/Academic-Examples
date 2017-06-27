TITLE Fibonacci Numbers     (assn2.asm)

; Author: Aaron Boutin
; Course: CS 271 / Project ID: Programming Assignment 2                Date: 09/JUL/16
; Description: Program greets the user, then takes in their desired number of terms and displays that many
;				numbers in the Fibonacci numbers sequence.

INCLUDE Irvine32.inc

; (insert constant definitions here)
UPPER_LIMIT	= 46
LOWER_LIMIT = 1

.data

; (insert variable definitions here)
titleIntro	BYTE	"             Fibonacci numbers          by Aaron Michael Boutin",0
extra_c		BYTE	"**EC: Displays the Fibonacci numbers in aligned columns.",0
greet_1		BYTE	"Hello ",0
userName	BYTE	25	DUP(0)
prompt_1	BYTE	"What is your name? ",0
prompt_2	BYTE	"Please enter the number of desired terms of the Fibonacci Numbers,",0
prompt_3	BYTE	"Please, only enter an integer within the range of 1 through 46, inclusive.",0
outOfBound	BYTE	"The integer you entered is out of range, please select in integer from 1 to 46.",0
prompt_4	BYTE	"How many terms would you like displayed? ",0
userTerms	DWORD	?
goodbye		BYTE	"Thank you and Good-Bye, ",0
counter		DWORD	5									; counter is used for keeping track of how many prints to a single line are made
temp		DWORD	1									; variable used in swapping values for calculations
val1		DWORD	1									; every two values in the sequence are kept in and updated in val1 and val2
val2		DWORD	1
fiveSpaces	BYTE	"          ",0
sCount		DWORD	?									; used in calculating spaces between printed numbers to align columns
maxSpace	DWORD	11									; max starting space for which all spacing is referenced to
divisor		DWORD	10									; used in determining number of spaces that follow a printed number

.code
main PROC

; (insert executable instructions here)
;display title and programmer name
	mov		edx, OFFSET titleIntro
	call	WriteString
	call	CrLf
	call	CrLf

;display extra credit statement
	mov		edx, OFFSET extra_c
	call	WriteString
	call	CrLf
	call	CrLf

;get user's name
	mov		edx, OFFSET prompt_1
	call	WriteString
	mov		edx, OFFSET userName
	mov		ecx, sizeof	userName
	call	ReadString				; prompts user for name

;greet user
	mov		edx, OFFSET greet_1 
	call	WriteString
	mov		edx, OFFSET userName	; includes user's name in greeting
	call	WriteString
	call	CrLf
	call	CrLf

;ask user for number of terms of Fibonacci sequence with limit
	mov		edx, OFFSET prompt_2	; First prompt asks for term #
	call	WriteString
	call	CrLf
	mov		edx, OFFSET prompt_3	; Second prompt gives range of choosable values
	call	WriteString
	call	CrLf
	call	CrLf

;Get User Input
Validate:	
	mov		edx, OFFSET prompt_4
	call	WriteString
	call	ReadDec					; reads in user-defined number of desired terms
	mov		userTerms, eax
	call	CrLf

; Validate user input
	mov		edx, userTerms
	cmp		edx, UPPER_LIMIT		; compares user input to upper limit
	jle		LoopUpperValid			; if valid, directs program to test lower limit
	mov		edx, OFFSET outOfBound	
	call	WriteString				; if not valid, displays error message and redirects to ask for new number
	call	CrLf
	jmp		Validate	
LoopUpperValid:
	mov		edx, userTerms
	cmp		edx, LOWER_LIMIT		; compares value to the lower limit if good, procedes to calculations
	jge		BothValid				; if both valid, jump to calculations
	mov		edx, OFFSET outOfBound	; if invalid, informs user of error and asks for new number
	call	WriteString
	call	CrLf
	jmp		Validate

;Calculate and display user-defined terms
BothValid:

; print first 2 numbers
JFirst2:							;this jump takes two passes, and if the desired terms are one and two, it will 
	mov		eax, 1					;print either and jump to the Goody-Bye. Otherwise, will continue on to calculate the rest
	call	WriteDec

	mov		edx, userTerms	
	cmp		edx, 1					;tests if only the first two numbers are desired
	je		OneOrTwoFirst

	mov		edx, OFFSET fiveSpaces
	call	WriteString
	dec		counter					;the first two prints decrement the print counter,
	dec		userTerms				;which is used to only display 5 numbers per line.
	mov		edx, counter
	cmp		edx, 4
	je		JFirst2			

	mov	ecx, userTerms				;preps ecx for loop counter
LoopMain:
;check print counter
	mov		edx, counter			
	cmp		edx, 0
	jne		J1
	call	CrLf					;if loop counter is down to zero, goes to next line and resets counter
	mov		counter, 5	

; caclulate next two values
J1:
	mov		temp, eax
	mov		eax, val2
	add		eax, val1				; add previous values to get new value
	mov		val2, eax
	mov		eax, temp				; move to val1 the previous value before the new value
	mov		val1, eax

;print next Fibonacci number
	mov		eax, val2
	call	WriteDec

;print spaceing
	call	PrintSpaces	;Separate Proc called to calculate number of spaces for alignment

;decrement number on line counter
	dec		counter					; decrements counter for how many numbers appear on a line
	loop	LoopMain				; directs to loop start to calculate next number

OneOrTwoFirst:
;bid farewell to user, specifying name
	call	CrLf
	call	CrLf
	mov		edx, OFFSET goodbye		; says goodbye
	call	WriteString
	mov		edx, OFFSET userName	; adds user name to goodbye
	call	WriteString
	call	CrLf

	exit	; exit to operating system
main ENDP

; (insert additional procedures here)

;-------------------------------------------------------------------------------
; PrintSpaces
;
; Takes the max amount of spaces desired in a line and calculates the amount of
;	blank spaces that will follow based on the number of digits in number. It achieves
;	this by dividing the value by 10 and incrementing a counter each time, since dividing
;	by 10 removes a digit each time. This is how I achieve column alignment.
; Recieves: EAX as the most recently calculated Fibonacci number printed. 
; Returns: Nothing.
;-------------------------------------------------------------------------------
PrintSpaces		proc
	mov		sCount, 0
MoreDigits:
	inc		sCount
	mov		edx, 0
	div		divisor				; divides by 10 to count number of digits in number
	mov		edx, eax
	cmp		edx, 1				; continues to divide by 10 until the value is less than 1, indicating no more whole digits
	jl		NoMoreDigits
	jmp		MoreDigits
NoMoreDigits:
	push	ecx					;saves outer loop counter
	mov		eax, maxSpace
	sub		eax, sCount			;gets maxSpaces and subtracts the number of counted digits in a number
	mov		ecx, eax			;sets the loop to the calculated difference
LOOP2:							;prints how many spaces were calculated
	mov		al, ' '
	call	WriteChar
	loop	LOOP2
	pop		ecx					;returns outer loop counter
	mov		eax, val2
ret
printSpaces		endp


END main
