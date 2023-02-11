; Sterling LaBarbera
; 6.11.2.6
; this program does basic logical operations on 2 hex values
; the user inputs the values, and selects the operation from a table 
; examples in 6.5.4 used for reference

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc

.data
CaseTable BYTE 1
	DWORD AND_op
EntrySize = ($ - CaseTable)
	Byte 2
	DWORD OR_op
	Byte 3
	DWORD NOT_op
	Byte 4
	DWORD XOR_op
NoEntries = ($ - CaseTable)/EntrySize

menu BYTE "Boolean calculator",0
menu2 BYTE "You may choose the following operations: ",0
menu3 BYTE "1 x AND y. ",0
menu4 BYTE "2 x OR y. ",0
menu5 BYTE "3 NOT x. ",0
menu6 BYTE "4 x XOR y. ",0
menuE BYTE "5 exit. ",0
menu7 BYTE "Enter 1, 2, 3, 4, or 5 to choose: ",0
prompt1 BYTE "Enter a hexadecimal value: ",0
prompt2 BYTE "Enter a second Hexadecimal Value: ",0
operation BYTE 0
opmsg1 BYTE "Now ANDing the two values: ",0
opmsg2 BYTE "Now ORing the two values: ",0
opmsg3 BYTE "Now inverting the value: ",0
opmsg4 BYTE "Now XORing the two values: ",0

x DWORD 0h
y DWORD 0h

.code
main proc
	mov edx,OFFSET menu
	call WriteString
	mov edx,OFFSET menu2
	call WriteString
	mov edx,OFFSET menu3
	call WriteString
	mov edx,OFFSET menu4
	call WriteString
	mov edx,OFFSET menu5
	call WriteString
	mov edx,OFFSET menu6
	call WriteString
	mov edx,OFFSET menuE
	call Writestring
L0:
	mov edx,OFFSET menu7
	call WriteString
	mov eax,0
	call ReadDec
	mov ebx,OFFSET casetable
	mov operation,al
	mov ecx,NoEntries
L1:
	cmp al,[ebx]
	jne L2
	call NEAR PTR [ebx + 1]
	jmp L3
L2:
	add ebx,EntrySize
	loop L1
L3:
	cmp al,5
	je finish
	jmp L0
finish:
	call WaitMsg
	invoke ExitProcess,0
main endp

;performs a logical AND of 2 hex values
;returns result in eax
AND_op proc
	mov edx,OFFSET prompt1
	call Writestring
	call ReadHex
	mov x,eax
	mov edx,OFFSET prompt2
	call Writestring
	call ReadHex
	mov y,eax
	mov edx,OFFSET opmsg1
	call WriteString
	mov eax,x
	mov ebx,y
	and eax,ebx
	call WriteHex
	ret
AND_op endp

;performs a logical OR of 2 hex values
;returns result in eax
OR_op proc
	mov edx,OFFSET prompt1
	call Writestring
	call ReadHex
	mov x,eax
	mov edx,OFFSET prompt2
	call Writestring
	call ReadHex
	mov y,eax
	mov edx,OFFSET opmsg2
	call WriteString
	mov eax,x
	mov ebx,y
	or eax,ebx
	call WriteHex
	ret
OR_op endp

;performs a logical NOT of a hex value
;returns result in eax
NOT_op proc
	mov edx,OFFSET prompt1
	call Writestring
	call ReadHex
	mov x,eax
	mov edx,OFFSET opmsg3
	call WriteString
	mov eax,x
	not eax
	call WriteHex
	ret
NOT_op endp

;performs a logical XOR of 2 hex values
;returns result in eax
XOR_op proc
	mov edx,OFFSET prompt1
	call Writestring
	call ReadHex
	mov x,eax
	mov edx,OFFSET prompt2
	call Writestring
	call ReadHex
	mov y,eax
	mov edx,OFFSET opmsg4
	call WriteString
	mov eax,x
	mov ebx,y
	xor eax,ebx
	call WriteHex
ret
XOR_op endp
END main