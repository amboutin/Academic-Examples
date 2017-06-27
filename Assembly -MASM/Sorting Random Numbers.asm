;TITLE Sorting Random Numbers     (assn4.asm)

; Author: Aaron Boutin
; CS 271 / Assignment 4                Date: 01 AUG 16
; Description: Fills an array with user defined number of random values and prints it.
;				Then Bubble sorts the array from largest to smallest, and prints the median and the whole array.

INCLUDE Irvine32.inc

; (insert constant definitions here)
MAX = 200					; input value range ceiling
MIN = 10					; input value range floor
LO = 100					; floor for values to add to array
HIGH_VAL = 999				; ceiling for values to add to array	
MAX_SIZE = 191				; constant for array initialization
VPL = 10					; numbers of values printed per line
.data

; (insert variable definitions here)
introTitle	BYTE	"Sorting Random Numbers             Programmed by Aaron Boutin",0
intro_1		BYTE	"This program generates random numbers in the range [100 .. 999],",0
intro_2		BYTE	"displays the original list, sorts the list, and calculates the",0
intro_3		BYTE	"median value. Finally, it displays the list sorted in descending order.",0
prompt_1	BYTE	"How many numbers should be generated? [10 ... 200]: ",0
rangeErr	BYTE	"Out of range. Please try again.",0
statement_1	BYTE	"The unsorted random numbers:",0
statement_2	BYTE	"The median is ",0
statement_3	BYTE	"The sorted list:",0
goodbye		BYTE	"Enjoy your results, Goodbye.",0
request		DWORD	?
spaces		DWORD	"   ",0									

array		DWORD	MAX_SIZE DUP(?)
count		DWORD	0

.code
main PROC

; (insert executable instructions here)

	call	Randomize
	
	call	introduction
	
	call	getUserData

	push	request						; argument for fill_array
	push	OFFSET array				; argument for fill_array
	call	fill_array

	mov		edx, OFFSET statement_1		; Indicates following values are unsorted
	call	WriteString
	call	CrLf
	push	OFFSET array				; argument for display_list
	push	count						; argument for display_list
	call	display_list

	push	OFFSET	array				; argument for sort
	push	count						; argument for sort
	call	sort

	push	OFFSET array				; argument for display_median
	push	count						; argument for display_median
	call	display_median

	mov		edx, OFFSET statement_3		; Indicates following values are sorted
	call	WriteString
	call	CrLf
	push	OFFSET array				; argument for display_list
	push	count						; argument for display_list
	call	display_list

	call	farewell

	exit	; exit to operating system
main ENDP

; (insert additional procedures here)
;-------------------------------------------------------------------------------
; introduction
; 
; Procedure that prints program title, and author, as well as instructions.
;		
; Recieves: Nothing.
; Returns: Nothing.
;-------------------------------------------------------------------------------
introduction	PROC	USES edx
	mov		edx, OFFSET introTitle	; print title and author
	call	WriteString
	call	CrLf
	call	CrLf

	mov		edx, OFFSET intro_1		; prints instruction for user
	call	WriteString
	call	CrLf
	mov		edx, OFFSET intro_2		; prints instruction for user
	call	WriteString
	call	CrLf
	mov		edx, OFFSET intro_3		; prints instruction for user
	call	WriteString
	call	CrLf
	call	CrLf

	ret
introduction	ENDP

;-------------------------------------------------------------------------------
;getUserData
; 
; Procedure that prompts user for input, then stores that to a variable.
;	The procedure calls a sub-procedure that validates request against a
;	predetermined range.
;		
; Recieves: Nothing.
; Returns: Nothing.
;-------------------------------------------------------------------------------
getUserData	PROC	USES edx eax
tryAgain:	
	mov		edx, OFFSET prompt_1	; prompts user for desired range of input
	call	WriteString
	call	ReadDec					; reads in user-defined number of desired terms
	mov		request, eax
	call	CrLf

	push	request
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
; Recieves: User request by reference on stack.
; Returns: 1 or 0 in edx. 1 for true, 0 for false.
;-------------------------------------------------------------------------------
validate	PROC
; Validate user input
	push	ebp
	mov		ebp, esp
	mov		edx, [ebp+8]
	cmp		edx, MAX				; compares user input to upper limit
	jbe		UpperValid				; if valid, directs program to test lower limit
	mov		edx, OFFSET rangeErr	
	call	WriteString				; if not valid, displays error message and returns 0
	call	CrLf
	mov		eax, 0
	jmp		ProgRet

UpperValid:
	cmp		edx, MIN				; compares value to the lower limit if good, procedes to calculations
	jae		BothValid				; if both valid, jump to calculations
	mov		edx, OFFSET rangeErr	
	call	WriteString				; if not valid, displays error message and returns 0
	call	CrLf
	mov		eax, 0
	jmp		ProgRet

BothValid:
	mov		eax, 1					; if valid, returns 1
	jmp		ProgRet

ProgRet:
	pop		ebp
	ret		4

validate	ENDP

;-------------------------------------------------------------------------------
; fill_array
; 
; Procedure that fills an array with numbers. In this case, they are randomized.
;		
; Recieves: A value that represents the number of values to be put in the array, and an array passed by reference,
;			with both being on the stack.
; Returns: Nothing.
;-------------------------------------------------------------------------------
fill_array	PROC
	push	ebp
	mov		ebp, esp
	mov		ecx, [ebp+12]		; number of elements in array
	mov		count, ecx
	mov		esi, [ebp+8]		; array offset
LOOP1:
	call	randomGen			
	mov		[esi], eax			; places random number in array
	add		esi, 4				; increments array to next index
	loop	LOOP1				; repeats for the size of array passed in
	pop		ebp
	ret		8					
fill_array ENDP

;-------------------------------------------------------------------------------
; display_list
; 
; Procedure that fills an array with numbers.
;		
; Recieves: The offset of the array to be printed, and the array size, both passed in on the stack.
; Returns: Nothing.
;-------------------------------------------------------------------------------
display_list	PROC
	push	ebp
	mov		ebp, esp
	mov		esi, [ebp+12]			; array offset	
	mov		ecx, [ebp+8]			; number of elements in array
	sub		esp, 4
	mov		DWORD PTR[ebp-4], VPL	; set local variable to numbers to be printed per line

more:
	mov		eax, DWORD PTR[ebp-4]
	cmp		eax, 0
	jne		NextLine				; tests to see if the max values per line have been printed
	call	CrLf
	mov		DWORD PTR[ebp-4], VPL	; resets local variable

NextLine:
	mov		eax, [esi]
	call	WriteDec
	mov		edx, OFFSET spaces		; prints spacing
	call	WriteString
	dec		DWORD PTR[ebp-4]		; decrement values per line counter
	add		esi, 4					; increments array to next index
	loop	more
endMore:
	mov		esp, ebp				; remove local variable
	pop		ebp
	ret		8

display_list	ENDP

;-------------------------------------------------------------------------------
; sort
; 
; Procedure that sorts as a Bubble Sort. Uses local variable to determine if a swap
;		occured during one pass down the array.
;		
; Recieves: Offset of array and array size as arguments on the stack.
; Returns: Nothing.
;-------------------------------------------------------------------------------
sort	PROC

	push	ebp
	mov		ebp, esp
	sub		esp, 4					; create local variable

Whileloop:
	mov		ecx, [ebp+8]			; array size
	sub		ecx, 1
	mov		esi, [ebp+12]			; array offset


	mov		DWORD PTR [ebp-4], 0	; local variable
Loop2:
	mov		eax, [esi]				; these two values are compared,
	mov		edx, [esi+4]			; to see if they need to be swapped
	cmp		eax, edx
	jae		NoSwap
	push	esi						; argument for swap function
	call	exchange_elements		; internal swap function
	mov		DWORD PTR [ebp-4], 1

NoSwap:
	add		esi, 4					; move down array indecies
	loop	Loop2
	cmp		DWORD PTR [ebp-4], 1	; check to see if swap occured
	je		Whileloop				; if swap occured, iterate through again
	mov		esp, ebp
	pop		ebp
	ret		8

sort	ENDP

;-------------------------------------------------------------------------------
; exchange_elements
; 
; Procedure that exchanges two elements. Creates a local variable to use in the swap.
;		
; Recieves: Offsets of the first of two variables to be exchanged on the stack.
; Returns: Nothing.
;-------------------------------------------------------------------------------
exchange_elements	PROC
	push	ebp
	mov		ebp, esp
	sub		esp, 4

	mov		esi, [ebp+8]
	mov		eax, [esi]
	mov		DWORD PTR[ebp-4], eax	; put left val in temp variable

	mov		eax, [esi+4]
	mov		[esi], eax				; put right val in left val

	mov		eax, DWORD PTR[ebp-4]
	mov		[esi+4], eax			; put temp (orig left val) in right val

	mov		esp, ebp				; remove local variable
	pop		ebp
	ret		4

exchange_elements	ENDP

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
; randomGen
; 
; Procedure that calls randomrange procedure using a global constant defined range.
;	The procedure then adds the low constant value to the result to ensure it is in range.
;
; Recieves: Nothing.
; Returns: Randomized number within specified range in EAX.
;-------------------------------------------------------------------------------
randomGen	PROC
	mov		eax, HIGH_VAL
	sub		eax, LO
	add		eax, 1
	call	randomrange
	add		eax, LO				; ensures random value is larger than lowest allowable value
	ret

randomGen	ENDP

;-------------------------------------------------------------------------------
; display_median
; 
; Procedure that calculates the median of an array and displays it. 
;		
; Recieves: The offset of the array, and the array size, both passed in on the stack.
; Returns: Nothing.
;-------------------------------------------------------------------------------
display_median	PROC

	push	ebp
	mov		ebp, esp

	call	CrLf
	call	CrLf
	mov		edx, OFFSET statement_2		; indicates that the value is the median
	call	WriteString

	mov		edx, 0
	mov		eax, [ebp+8]				; array size
	mov		ebx, 2						; will get middle of array index
	div		ebx
	cmp		edx, 0
	jne		oddArray					; if the array has odd number of values, jumps to print middle
	mov		esi, [ebp+12]				; offset of array
	mov		edx, 4						; data size
	mul		edx							; multiply data size by index at middle of array
	add		esi, eax					; esi is incremented to the middle of the array index
	mov		eax, [esi]
	add		eax, [esi-4]				; two middle values in array are combined
	mov		edx, 0
	div		ebx							; combined value is divide in two for median value
	call	WriteDec
	jmp		pComplete					; jumps to end after printing has completed

oddArray:								; if array has odd number of values, middle value is simply printed
	mov		esi, [ebp+12]				; offset of array
	mov		ebx, 4
	mul		ebx							; eax has divided array size, which is middle index, and is muiltiplied by data size
	add		esi, eax
	mov		eax, [esi]					; value at middle index is moved to eax for printing
	call	WriteDec
pComplete:
	call	CrLf
	call	CrLf
	pop		ebp
	ret		8

display_median	ENDP

END	main