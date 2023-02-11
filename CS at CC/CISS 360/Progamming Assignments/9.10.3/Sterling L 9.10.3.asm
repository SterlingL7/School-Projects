; Sterling LaBarbera
; 9.10.3
; this program removes n characters from the target string.
; takes n from the user.
; takes the location from user as well.
; sticking with array index convention, starting value can be from 0 to length-1

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc
Str_remove PROTO targetad:DWORD

.data
targetStr BYTE "The quick brown fox jumps over the lazy dog.",0
prompt1 BYTE "How many characters would you like to remove?",0
prompt2 BYTE "Which character is the start of the removal?",0
prompt3 BYTE "Enter and integer value, 0 is the first character: ",0

.code
main proc
	mov edx,OFFSET prompt1
	call WriteString
	call crlf
	call ReadInt
	mov ebx,eax
	
	mov edx,OFFSET prompt2
	call WriteString
	call crlf
	call ReadInt

	mov edx,LENGTHOF targetStr
	invoke Str_remove, ADDR targetStr

	mov edx,OFFSET targetStr
	call WriteString
	call crlf

	call WaitMsg
	invoke ExitProcess,0
main endp

Str_remove proc uses esi edi ecx,
	targetad:DWORD
; removes n values from the target string
; starting from the offset
; offset: EAX
; # of chars: EBX
; length of string: EDX
; target string address passed as a parameter
; if number of characters is greater than characters from offset to end of string
; place null byte at first byte to be removed
	
	mov edi,targetad
	add edi,eax
	mov esi,edi
	add eax,ebx
	cmp eax,edx
	jg L2
	add esi,ebx
	mov ecx,edx
	
	cld
	rep movsb
	jmp L3
	; decided that 2 lines probably better than 7
	;mov ebx,[esi]
	;mov BYTE PTR [edi],bl
	;inc edi
	;inc esi
	;mov ebx,[esi]
	;cmp ebx,0
	;jne L1
L2:
	inc edi
	mov BYTE PTR [esi],0
L3:
	ret
Str_remove endp
END main


cld
rep movsb