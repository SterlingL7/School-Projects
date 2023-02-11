; Sterling LaBarbera
; 5.9.8
; this program generates a matrix of characters
; that represents each foreground + background combination
; for the console window.
; this is pretty much the same as what Irvine has in the video, not really sure there is much I could do differently
; added functionality to accept inputs until user is done

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc

.data
prompt1 BYTE "Bitwise Multiplier of unsigned integers",0
prompt2 BYTE "Enter the first number: ",0
prompt3 BYTE "Enter the second number: ",0
last BYTE "Product: ",0
cont BYTE "Continue? y/n: ",0

.code
main proc
	mov edx,OFFSET prompt1
	call WriteString
LC:
	call Crlf
	mov edx,OFFSET prompt2
	call WriteString
	call ReadDec
	mov ebx,eax
	
	mov edx,OFFSET prompt3
	call WriteString
	call ReadDec
	call Crlf

	call BitwiseMultiply
	mov edx,OFFSET last
	call WriteString
	call WriteDec
	call Crlf

	mov edx,OFFSET cont
	call WriteString
	call ReadChar
	cmp al,'y'
	je LC
	cmp al,'Y'
	je LC
	call WaitMsg
	invoke ExitProcess,0
main endp

; BitwiseMultiply takes 2 32bit decimal ints in eax and ebx
; performs multiplication by shifting bits
; returns value in eax

BitwiseMultiply proc
	mov edx,eax
	mov eax,0
	mov cl,0

L1:
	shr edx,1
	jnc L2
	mov esi,ebx
	shl esi,cl
	add eax,esi
L2:
	inc cl
	cmp cl,32
	jb L1

	ret
BitwiseMultiply endp
END main