; Sterling LaBarbera
; 8.11.1
; this program finds the largest value in 3 different signed integer arrays

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc
FindLargest PROTO, arrayPtr:PTR DWORD, arraySize:DWORD
.data
array1 DWORD 1,-1,2,-3,3,4,4,-4,5,99,8,8,7,51,6,-21,12,14,-5
array2 DWORD -1,-2,-3,-4,-5
array3 DWORD 77,-77,50,-50,45,-45,54,-54,0,-0,9,-9,78

.code
main proc
	invoke FindLargest, ADDR array1, LENGTHOF array1
	call WriteInt
	call crlf

	invoke FindLargest,	ADDR array2, LENGTHOF array2
	call WriteInt
	call crlf

	invoke FindLargest, ADDR array3, LENGTHOF array3
	call WriteInt
	call crlf

	call WaitMsg
	invoke ExitProcess,0
main endp

; FindLargest finds the largest value in an array 
; and returns it in EAX
; takes length of array and the pointer to it in parameters.

FindLargest proc USES ebx ecx esi,
	arrayPtr:PTR DWORD,
	arraySize:DWORD

	mov eax,0
	mov esi,arrayPtr
	mov ecx,arraySize
	dec ecx
	mov ebx,[esi]
	add esi,4
L1:
	mov eax,[esi]
	cmp eax,ebx
	jg L2
	jmp L3
L2:
	mov edx,eax
	jmp L4
L3:
	mov edx,ebx
L4:	add esi,4
	mov ebx,edx
	loop L1
	mov eax,edx
	ret
FindLargest endp
END main