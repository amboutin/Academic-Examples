TITLE Designing low-level I/O Procedures     (Assn5.asm)

; Author: Aaron Boutin
; CS 271 / Assignment 5A                 Date: 08/AUG/16
; Description: Program that gets ten values, max of 32-bits, from user and displays them, their sum, and their
;				average. In doing this, it takes in values as strings, convert to integers for arithmetic operations,
;				and then converts them back into strings for printing to screen.

INCLUDE Irvine32.inc

; (insert constant definitions here)
	MAX = 4294967295
	MIN = 0

;---------------------------------------------------------------------
;mgetString
;
; Macro that prompts user for desired input, then moves it to memory location
;		
; Recieves: three variables, the first is the offset of the prompt to be displayed, 
;			the second is the offset of the memory location, and the third is 
;			the offset of the count of array.
;	
; Returns: Nothing
;---------------------------------------------------------------------
mgetString	MACRO prompt, inputVar, arrayCount
	push	ecx
	push	edx
	push	eax
	mov		edx, prompt
	call	WriteString
	mov		edx, OFFSET inputVar
	mov		ecx, 20
	call	ReadString
	mov		edx, arrayCount
	mov		[edx], eax
	pop		eax
	pop		edx
	pop		ecx
ENDM

;---------------------------------------------------------------------
;mdisplayString
;
; Macro that takes the offset of a string and prints it to the screen.
;		
; Recieves: OFfset of string to be printed.
; Returns: Nothing
;---------------------------------------------------------------------
mdisplayString	MACRO string
	push	edx
	mov		edx, string
	call	WriteString
	pop		edx
ENDM


.data
; (insert variable definitions here)
title_1			BYTE	"Designing low-level I/O procedures             Programmed by Aaron Boutin",0
prompt_1_1		BYTE	"Please provide 10 unsigned decimal integers.",0
prompt_2		BYTE	"Please enter an unsigned number: ",0
prompt_1_2		BYTE	"Each number needs to be small enough to fit inside a 32 bit register",0
prompt_1_3		BYTE	"After you have finished inputting the raw numbers I will display a list",0
prompt_1_4		BYTE	"of the integersm, their sum, and their average value.",0
errorVal		BYTE	"ERROR: You did not enter an unsigned number or your number was too big.",0
prompt_3		BYTE	"You entered the following numbers:",0
prompt_4		BYTE	"The sum of these numbers is: ",0
prompt_5		BYTE	"The average is: ",0
goodbye			BYTE	"Thank you, Good-Bye!",0
comspa			BYTE	", ",0

userInput		BYTE	20 DUP (?)						; array holds user-defined string
userValidVal	DWORD	?								; variable to hold the last valid value extracted from the user
uIbyteCount		DWORD	?								; count of values in array
tenValues		DWORD	11 DUP(?)						; array to hodl ten valid values
tenValCount		DWORD	0								; size of valid values array
intToSt			BYTE	15 DUP(?)						; array used for int to string conversion
intToSt2		BYTE	15 DUP(?)						; array used for int to string conversion
intToSt3		BYTE	15 DUP(?)						; array used for int to string conversion
sumVal			DWORD	?								; variable to hold sum of array values
aveVal			DWORD	?								; variable to hold average of array values

.code
main PROC

; (insert executable instructions here)

	mdisplayString	OFFSET title_1			; Displays title 
	call	CrLf
	call	CrLf	
	mdisplayString	OFFSET prompt_1_1		; displays instructions
	call	CrLf
	mdisplayString	OFFSET prompt_1_2		; displays instructions
	call	CrLf
	mdisplayString	OFFSET prompt_1_3		; displays instructions
	call	CrLf
	mdisplayString	OFFSET prompt_1_4		; displays instructions
	call	CrLf
	call	CrLf

NeedMore:

	push	OFFSET uIbyteCount
	push	OFFSET userInput
	push	OFFSET userValidVal
	call	ReadVal							; take in user string and convert to integer

	push	OFFSET userValidVal
	push	OFFSET tenValues
	push	OFFSET tenValCount
	call	arrayStore						; stores integer value into array
	cmp		tenValCount, 10
	jb		NeedMore						; loops until 10 valid integers have been stored in array

	call	CrLf
	mdisplayString	OFFSET prompt_3			; displays explanation
	call	CrLf

	push	OFFSET tenValues
	push	tenValCount
	call	display_list					; displays values in valid array to screen, modified to omit last value for next function call to WriteVal
	
	push	OFFSET userValidVal
	push	OFFSET intToSt 
	call	WriteVal						; prints the last value in the valid array as a string, just a personal test

	push	OFFSET aveVal
	push	OFFSET sumVal
	push	OFFSET tenValues
	call	sumAndAveVals					; calculates the sum and average of values in array

	mdisplayString	OFFSET prompt_4			; displays explanation

	push	OFFSET sumVal
	push	OFFSET intToSt2
	call	WriteVal						; displays sum

	mdisplayString	OFFSET prompt_5			; displays explanation

	push	OFFSET aveVal
	push	OFFSET intToSt3
	call	WriteVal						; displays average

	mdisplayString	OFFSET goodbye
	call	CrLf

	exit	; exit to operating system
main ENDP

; (insert additional procedures here)
;---------------------------------------------------------------------
;ReadVal
;
; Procedure that takes in a string that represents a decimal value and converts it to integers.
;		It uses the method outlined in Lecture 23.
;		
; Recieves: The offset of the array size for the user-defined array value, the offset of the
;			variable to hold user-input, and the offset of a variable to hold valid values
; Returns:
;---------------------------------------------------------------------
ReadVal		PROC	USES EAX ECX EBX EDX EBP

	mov		EBP, ESP
	sub		ESP, 4

RePrompt:
	mgetString OFFSET prompt_2, OFFSET userInput, OFFSET uIbyteCount
	mov		EAX, [EBP+32]									; offset of size of user-defined array
	mov		ECX, [EAX]
	cmp		ECX, 11											; quick check to see if string holds more numbers than 32-bit reg can hold
	ja		NotValid
	mov		ESI, [EBP+28]									; size of array from user input is used to control while loop
	mov		eax, 0
	mov		DWORD PTR [EBP-4], eax
	 
WhileL:	
	mov		EAX, DWORD PTR [EBP-4]							; temp variable holds value as it is modified
	mov		EBX, 10
	mul		EBX												; multiply temp by 10
	cmp		EDX, 0		
	jne		NotValid										; Checks if the muliplication operation has created a value larger than 32 bits
	mov		DWORD PTR [EBP-4], eax							; value is saved back to temp
	mov		eax, 0
	CLD
	LODSB													; element in array is loaded into AL
	cmp		AL, 48											; during string conversion value validation
	jb		NotValid
	cmp		AL, 57											; during string conversion value validation
	ja		NotValid
	sub		AL, 48											; value is turned from character to integer representation
	add		DWORD PTR [EBP-4], eax							; value is added to temp
	jc		NotValid										; Checks if add operation causes a carry, indicating a value larger than 32 bits
	loop	WhileL											; iterates through array, converting each char to int.

	cmp		DWORD PTR [EBP-4], MAX
	ja		NotValid
	jmp		ProcEnd

NotValid:
	mdisplayString	OFFSET errorVal							; if not valid, error is displayed
	call	CrLf
	jmp		RePrompt										; jumps back to prompt user again

ProcEnd:
	mov		eax, DWORD PTR [EBP-4]							; final converted word is checked as a whole
	push	eax
	call	validate										; this check is against MIN and MAX
	cmp		eax, 1
	jne		RePrompt										; if not valid, will start process again and let user know of error
	mov		eax, DWORD PTR [EBP-4]
	mov		EBX, [EBP+24]									; if valid, value is stored in variable passed by reference
	mov		[EBX], EAX
	
	mov		ESP, EBP
	ret		12

ReadVal		ENDP

;-------------------------------------------------------------------------------
; validate
; 
; Procedure 
;		
; Recieves: Takes variable to be checked on stack.
; Returns: 1 or 0 in eax. 1 for true, 0 for false.
;-------------------------------------------------------------------------------
validate	PROC USES EDX EBP
; Validate user input

	mov		ebp, esp
	mov		edx, [ebp+12]
	cmp		edx, MAX				; compares user input to upper limit
	jbe		UpperValid				; if valid, directs program to test lower limit
	mov		eax, 0
	jmp		ProgRet

UpperValid:
	cmp		edx, MIN				; compares value to the lower limit if good, procedes to calculations
	jae		BothValid				; if both valid, jump to calculations
	mov		eax, 0
	jmp		ProgRet

BothValid:
	mov		eax, 1					; if valid, returns 1
	jmp		ProgRet

ProgRet:

	ret		4

validate	ENDP

;-------------------------------------------------------------------------------
; arrayStore
; 
; Procedure that stores a value to an array. It uses the count of the array to know where to place the next value
;		
; Recieves: Uses EAX EBX, ESI, EDX. Recieves three arguments, first is the OFFSET of Value to add to
;	the array, the second is the OFFSET of the array to be added to, and the third is the OFFSET of
;	the size holder of the array.
; Returns: Nothing
;-------------------------------------------------------------------------------
arrayStore	PROC USES EAX EBX ESI EDX EBP

	mov		EBP, ESP

	mov		EBX, [EBP+24]		; size of array is put in
	mov		EAX, [EBX]			; eax now holds array size
	mov		EBX, 4
	mul		EBX					; array size is multiplied by 4 (DWORD) to find index in array
	mov		ESI, [EBP+28]		; array that is added to
	add		ESI, EAX			; array is incremented to next open index
	mov		EAX, [EBP+32]		; offset of value to add is in EAX
	mov		EDX, [EAX]			; EDX now has actual value to be added
	mov		[ESI], EDX			; value is added to array
	mov		EAX, [EBP+24]		; array count is loaded
	mov		EBX, 1
	add		[EAX], EBX			; array count is incremented

	ret		12

arrayStore	ENDP


;-------------------------------------------------------------------------------
; display_list
; 
; Procedure that displays the values of a list.
;		
; Recieves: The offset of the array to be printed, and the array size, both passed in on the stack.
; Returns: Nothing.
;-------------------------------------------------------------------------------
display_list	PROC	USES ECX EAX ESI EDX EBP

	mov		ebp, esp
	mov		esi, [ebp+28]			; array offset	
	mov		ecx, [ebp+24]			; number of elements in array
	sub		ecx, 1
	sub		esp, 4
	mov		DWORD PTR[ebp-4], 10	; set local variable to numbers to be printed per line

more:
	mov		eax, DWORD PTR[ebp-4]
	cmp		eax, 0
	jne		NextLine				; tests to see if the max values per line have been printed
	call	CrLf
	mov		DWORD PTR[ebp-4], 10	; resets local variable

NextLine:
	mov		eax, [esi]
	call	WriteDec
	mov		edx, OFFSET comspa		; prints spacing
	call	WriteString
	dec		DWORD PTR[ebp-4]		; decrement values per line counter
	add		esi, 4					; increments array to next index
	loop	more

endMore:
	mov		esp, ebp				; remove local variable

	ret		8

display_list	ENDP


;-------------------------------------------------------------------------------
; WriteVal
; 
; Procedure that converts string decimal values into integer decimal values.
;		
; Recieves: Offset of string to be printed and offset of array to work in, both on the stack
; Returns: Nothing
;-------------------------------------------------------------------------------
WriteVal	PROC	USES EAX EDI EBX ECX EBP

	mov		EBP, ESP
	sub		ESP, 8
	mov		EDI, [EBP+24]				; puts offset of array into EDI for use with STOSB
	mov		EAX, 0
	mov		DWORD PTR [EBP-4], EAX
	mov		EDX, [EBP+28]
	mov		EAX, [EDX]

Cont:
	mov		EDX, 0
	mov		EBX, 10
	div		EBX							; we divide by 10 to determine place
	mov		EBX, 1
	add		DWORD PTR [EBP-4], EBX		; temp variable holds the number of times divisible by 10
	cmp		EAX, 10
	jae		Cont						; if the number is still large, it is reprocessed
	mov		DWORD PTR [EBP-8], EAX		; this is the stripped down value, it's place kept in the other temp
	add		EAX, 48						; this converts the integer to a character
	CLD
	STOSB								; character is stored in first index of array

	mov		ECX, DWORD PTR [EBP-4]		; the temp variable is used to control a loop that creates a value based on the assesed placement
	mov		EAX, 1
	mov		EBX, 10
WL_1:
	mul		EBX							; each multiplication increased the placement from 1, to 10s, to 100s etc
	loop	WL_1

	mov		EBX, DWORD PTR [EBP-8]		
	mul		EBX							; the stripped value is multiplied by its placement
	mov		EBX, [EBP+28]
	sub		[EBX], EAX					; the value with weight is subtracted from original value, removing it from the originial number
	mov		EAX, 0
	mov		DWORD PTR [EBP-4], EAX		; the placement temp counter is reset
	mov		EAX, [EBX]					
	cmp		EAX, 9						; the remaining value is checked to see if it is divisible by 10 again
	ja		Cont						; if so, the process starts again with the next integer in the original number
	cmp		EAX, 0						
	jb		Display						; if the value is no longer divisible but greater than 0, it is the last integer in our one's place
	add		EAX, 48						; it is turned into a character
	CLD
	STOSB								; added to array

Display:
	mdisplayString	[EBP+24]			; the array is then passed to a macro that prints strings, printing the decimal value using its ASCII equivalent
	call	CrLf
	mov		ESP, EBP

	ret		8

WriteVal	ENDP


;-------------------------------------------------------------------------------
; sumAndAveVals
; 
; Procedure that takes an array and calculates its sum and average values.
;		
; Recieves: Offset of variable to hold average value, offset of variable to hold sum value,
;			and offset of array with values to be calculated from.
; Returns: Nothing
;-------------------------------------------------------------------------------
sumAndAveVals	PROC USES ESI ECX EBX EAX EDX EBP

	mov		EBP, ESP
	mov		ESI, [EBP+28]		; ESI is set to array start
	mov		ECX, 10				; the counter is set to the number of variables in array
	mov		EDX, 0
lAgain:
	mov		EBX, [ESI]			; value is taken from array
	add		ESI, 4				; array is incremented
	add		EDX, EBX			; every extracted value is added to EDX, naturally summing it up
	loop	lAgain				; iterates through array
	mov		EBX, [EBP+32]		
	mov		[EBX], EDX			; value is stored in sum variable container
	mov		EAX, EDX
	mov		EDX, 0
	mov		EBX, 10				; summed value is divided by 10 to get average value
	div		EBX
	mov		EDX, [EBP+36]
	mov		[EDX], EAX			; value is put in average variable container

	ret 12

sumAndAveVals	ENDP

END main
