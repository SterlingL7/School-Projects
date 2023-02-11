; Sterling LaBarbera
; 8.11.4
; this program checks an array of integers for a squence of 3 3s
; prints 1 if there is, and 0 if not

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc
FindThrees PROTO, ArrayPtr:PTR DWORD, ArraySize:DWORD
.data
array1 DWORD 1,2,3,4,5,6,7,8,9,0,0ffh,5,343,7ah,43
array2 DWORD 1,1,1,2,2,2,3,4,5,6,6,6,6,6,7,7,9,4,4,2,2,25,14,14,14,5,6,3,3,5,3,3,3,7,7,7
array3 DWORD 3,3,1,3,3,2,3,3,3,3,3,4,3,3,5,3,3,6,3,3,7,3,3,8,3,3
array4 DWORD -3,-3,-3,2,2,3,3,-3,3,3,-3,3,-3
.code
main proc
	invoke FindThrees, ADDR array1, LENGTHOF array1
	mov eax,edx
	call WriteInt
	call crlf

	invoke FindThrees, ADDR array2, LENGTHOF array2
	mov eax,edx
	call WriteInt
	call crlf

	invoke FindThrees, ADDR array3, LENGTHOF array3
	mov eax,edx
	call WriteInt
	call crlf

	invoke FindThrees, ADDR array4, LENGTHOF array4
	mov eax,edx
	call WriteInt
	call crlf

	call WaitMsg
	invoke ExitProcess,0
main endp

; FindThrees determines whether an array has 3 3s in a row.
; returns 1 if yes 0 if no. returns in EDX
; takes size and pointer to array as parameters

FindThrees proc uses ebx ecx esi,
	ArrayPtr:PTR DWORD,
	ArraySize:DWORD
	
	mov esi,ArrayPtr
	mov ecx,ArraySize
	mov edx,0
L1:
	mov eax,[esi]
	cmp eax,3
	je L2
	mov ebx,0
	jmp L3
L2:
	inc ebx
	cmp ebx,3
	jne L3
	mov edx,1
	ret
L3: add esi,4
	loop L1
	ret
FindThrees endp

END main