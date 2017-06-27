TITLE Composite Numbers     (assn3.asm)

; Author: Aaron Boutin
; CS271/Assignment 3                Date: 24/JUL/16
; Description: Prompts user for a number of composite numbers to print,
;				then prints more in pages until the max number of 400 is reached.

INCLUDE Irvine32.inc

UPPER_LIMIT = 400					; input value range ceiling
LOWER_LIMIT = 1						; input value range floor

.data

introTitle	BYTE	"Composite Numbers             Programmed by Aaron Boutin",0
prompt_1	BYTE	"Enter the number of composite numbers you would like to see.",0
prompt_2	BYTE	"I'll accept orders for up to 400 composites.",0
prompt_3	BYTE	"Enter the number of composites to display [1...400]: ",0
rangeErr	BYTE	"Out of range. Try again.",0
goodbye		BYTE	"Enjoy your results, Goodbye.",0
prompt_4	BYTE	"Press any key if you would like to see more, up to a total of 400 printed composites.",0

extraC_1	BYTE	"**EC: Output is aligned to columns",0
extraC_2	BYTE	"**EC: More Composites are displayed in pages, until max range is reached",0
extraC_3	BYTE	"**EC: Values are checked only against prime divisors, added to an array when found",0

userInput	DWORD	?				; variable to hold user input
valCheck	DWORD	1				; variable used for checking prime calcs against
sCount		DWORD	?				; used in print calculations
maxSpace	DWORD	10				; used in print calculations, max amount of spaces between two values
divisor		DWORD	10				; used in calculating line spacing
counter		DWORD	10				; used to count values printed before moving cursor to next line
printTotal	DWORD	0				; counter that keeps track of number of composites printed
mPCounter	DWORD	?				; used in morePages function for tracking variables printed per page
valPerLine	DWORD	?				; used in print function to change how many values printed per line

pArray		DWORD	70 DUP (0)		; array used to store primes as they are encountered
aSize		DWORD	1				; array size


.code
main PROC

	call	introduction

	call	getUserData

	call	showComposites

	call	farewell

	exit	; exit to operating system
main ENDP

;-------------------------------------------------------------------------------
; introduction
; 
; Procedure that prints program title, author, and extra credit addons.
;		
; Recieves: Nothing.
; Returns: Nothing.
;-------------------------------------------------------------------------------
introduction	PROC	USES edx
	mov		edx, OFFSET introTitle	; print title and author
	call	WriteString
	call	CrLf
	call	CrLf

	mov		edx, OFFSET prompt_1	; prints instruction for user
	call	WriteString
	call	CrLf
	mov		edx, OFFSET prompt_2	; prints instruction for user
	call	WriteString
	call	CrLf
	call	CrLf


	mov		edx, OFFSET extraC_1	; print extra credit done
	call	WriteString
	call	CrLf	
	mov		edx, OFFSET extraC_2	; print extra credit done
	call	WriteString
	call	CrLf	
	mov		edx, OFFSET extraC_3	; print extra credit done
	call	WriteString
	call	CrLf
	call	CrLf

	ret

introduction	ENDP

;-------------------------------------------------------------------------------
;getUserData
; 
; Procedure that prompts user for input, then stores that to a variable.
;	The procedure calls a sub-procedure that validates userInput against a
;	predetermined range of 1 to 400.
;		
; Recieves: Nothing.
; Returns: Nothing.
;-------------------------------------------------------------------------------
getUserData	PROC	USES edx eax
tryAgain:	
	mov		edx, OFFSET prompt_3	; prompts user for desired range of input
	call	WriteString
	call	ReadDec					; reads in user-defined number of desired terms
	mov		userInput, eax
	call	CrLf

	mov		edx, userInput
	call	validate				; calls sub-procedure to validate input value
	cmp		eax, 1					; if out of range, jumps back and prompts again
	jne		tryAgain
	ret		

getUserData	ENDP

;-------------------------------------------------------------------------------
; validate
; 
; Procedure that prints program title, author, and extra credit addons
;		
; Recieves: userInput in EDX.
; Returns: 1 or 0 in edx. 1 for true, 0 for false.
;-------------------------------------------------------------------------------
validate	PROC
; Validate user input

	cmp		edx, UPPER_LIMIT		; compares user input to upper limit
	jbe		UpperValid				; if valid, directs program to test lower limit
	mov		edx, OFFSET rangeErr	
	call	WriteString				; if not valid, displays error message and returns 0
	call	CrLf
	mov		eax, 0
	ret	

UpperValid:
	cmp		edx, LOWER_LIMIT		; compares value to the lower limit if good, procedes to calculations
	jae		BothValid				; if both valid, jump to calculations
	mov		edx, OFFSET rangeErr	
	call	WriteString				; if not valid, displays error message and returns 0
	call	CrLf
	mov		eax, 0
	ret	

BothValid:
	mov		eax, 1					; if valid, returns 1
	ret

validate	ENDP

;-------------------------------------------------------------------------------
; showComposites
; 
; Procedure that calls two sub-procedures, one that checks if a value is composite,
;	the other is a print function that formats the printing of values.
; Recieves: Nothing.
; Returns: Nothing.
;-------------------------------------------------------------------------------
showComposites	PROC
	mov		eax, userInput
	mov		printTotal, eax			;sets value to userInput to determine how many composites to print
	mov		valPerLine, 10
	mov		counter, 10
WhileLoop:
	mov		ecx, userInput
	cmp		ecx, 0
	jbe		WhileDone
	inc		valCheck				; allows the sequential check of values for calcs and printing
	mov		eax, valCheck
	call	isComposite				; call to sub-procedure to test if composite
	cmp		edx, 0					; if not composite, skips printing the value
	je		isPrime
	mov		eax, valCheck
	call	PrintFormat				; if composite, calls sub-procedure to print value with formatting
	dec		userInput

isPrime:
	jmp		WhileLoop

WhileDone:
	call	CrLf
	
MORE:								; extra credit section that prints more values in pages
	mov		counter, 10				; resets counter used in printing format
	call	CrLf
	mov		edx, OFFSET prompt_4
	call	WriteString
	call	CrLf

	call	WaitMsg					; waits for user to press any key before processing next instruction
	call	CrLf
	call	CrLf
	call	morepages				; procedure used to print more pages

	mov		eax, printTotal			; this comparison is used to ensure that no more than 400 elements are printed
	cmp		eax, 400				
	jae		DONE
	jmp		MORE

DONE:
	ret

showComposites	ENDP

;-------------------------------------------------------------------------------
; isComposite
; 
; Procedure that uses an array to store prime numbers as they are encountered, then
;	checks the value to be checked against these prime divisors for primality.
;		
; Recieves: Nothing.
; Returns: boolean in EDX, 1 for true and 0 for false.
;-------------------------------------------------------------------------------
isComposite		PROC	USES esi ecx ebx
	mov		esi, OFFSET pArray		; set esi to prime array offset for indirect access
	mov		ecx, aSize				; use array size as loop counter

continueCheck:
	mov		eax, valCheck
	cmp		valCheck, 2				; initialize array to start value of 2
	jne		IsInit					; if initialised, will skip to calculations
	mov		edx, 2
	mov		[esi], edx
	jmp		InitA

IsInit:
	mov		edx, 0					; reset edx for use in division
	mov		ebx, [esi]
	cmp		ebx, 0					; ensure no division by 0
	je		loopEND

	div		ebx
	cmp		edx, 0					; if evenly divides, edx will be 0, indicating no remainder and is composite
	je		SetTrue	
	add		esi,4					; if not evenly divided, will increment esi by a DWORD to access next array element
	jmp		loopEnd

SetTrue:							; if composite, returns 1 for true
	mov		edx, 1
	jmp		tEND


loopEnd:
	loop	continueCheck
	mov		eax, valCheck			
	mov		[esi], eax				; if value is prime, it is added to array
	add		aSize,1					; size of array is incremented
InitA:
	mov		edx, 0
tEND:
	ret

isComposite		ENDP

;-------------------------------------------------------------------------------
; farewell
; 
; Procedure that prints to the user a goodbye string.
;		
; Recieves: Nothing.
; Returns: Nothing.
;-------------------------------------------------------------------------------
farewell	PROC
	call	CrLf
	call	CrLf
	mov		edx, OFFSET goodbye		; says goodbye
	call	WriteString
	call	CrLf
	ret

farewell	ENDP

;-------------------------------------------------------------------------------
; PrintFormat
;
; Takes the max amount of spaces desired in a line and calculates the amount of
;	blank spaces that will follow based on the number of digits in number. It achieves
;	this by dividing the value by 10 and incrementing a counter each time, since dividing
;	by 10 removes a digit each time. This is how I achieve column alignment.
;
; Recieves: EAX as the most recently determined composite number. 
; Returns: Nothing.
;-------------------------------------------------------------------------------
PrintFormat		proc	USES edx ecx ebx

	call	WriteDec				; print value to screen

;check print counter
	dec		counter					; decrements counter for how many numbers appear on a line
	mov		sCount, 0
MoreDigits:
	inc		sCount
	mov		edx, 0
	div		divisor					; divides by 10 to count number of digits in number
	cmp		eax, 1					; continues to divide by 10 until the value is less than 1, indicating no more whole digits
	jl		NoMoreDigits
	jmp		MoreDigits
NoMoreDigits:
	push	ecx						;saves outer loop counter
	mov		eax, maxSpace
	sub		eax, sCount				; gets maxSpaces and subtracts the number of counted digits in a number
	mov		ecx, eax				; sets the loop to the calculated difference
LOOP2:								; prints how many spaces were calculated
	mov		al, ' '
	call	WriteChar
	loop	LOOP2

	mov		edx, counter			
	cmp		edx, 0
	jne		J1
	call	CrLf					; if loop counter is down to zero, goes to next line and resets counter

	mov		ebx, valPerLine	
	mov		counter, ebx	
J1:
	pop		ecx						; returns outer loop counter
	ret

PrintFormat		endp

;-------------------------------------------------------------------------------
; morePages
; 
; Procedure that prints subsequent composite values in pages.
;		
; Recieves: Nothing.
; Returns: Nothing.
;-------------------------------------------------------------------------------
morePages		PROC
	mov		valPerLine, 6			; sets the number of values per line for print function
	mov		counter, 6				; sets the number of values per line for print function
	mov		mPCounter, 84			; number of values to print per page
WhileLoop:
	cmp		mPCounter, 0
	jbe		WhileDone			
	mov		eax, printTotal			
	cmp		eax, UPPER_LIMIT		; compares number of values printed to the ceiling
	jae		WhileDone				; if above ceiling, jumps to end for procedure return
	inc		valCheck
	call	isComposite
	cmp		edx, 1
	jne		WhileLoop				; if not composite, skips printing
	mov		eax, valCheck
	call	PrintFormat
	inc		printTotal
	dec		mPCounter				; only decrements loop counter if composite is printed				
	jmp		WhileLoop

WhileDone:	
	ret	

morePages		ENDP

END main