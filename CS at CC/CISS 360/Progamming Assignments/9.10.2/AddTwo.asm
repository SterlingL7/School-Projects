; Sterling LaBarbera
; 9.10.2
; this program concatenates a source string onto a target string
; the procedure takes addresses of the 2 strings

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc
Str_concat PROTO targetad:DWORD, sourcead:DWORD

.data
targetStr BYTE "add stuff here: ",20 dup(0)
sourceStr BYTE " 0 things to add.",0
target2 BYTE " dddddd 0 000",10 dup(0)
source2 BYTE "00    34t",0

.code
main proc
	mov ecx,LENGTHOF targetStr
	mov ebx,LENGTHOF sourceStr
	invoke Str_concat, ADDR targetStr, ADDR sourceStr
	mov edx,OFFSET targetStr
	call WriteString
	call crlf

	mov ecx,LENGTHOF target2
	mov ebx,LENGTHOF source2
	invoke Str_concat, ADDR target2, ADDR source2
	mov edx,OFFSET target2
	call WriteString
	call crlf

	call WaitMsg
	invoke ExitProcess,0
main endp

Str_concat proc uses esi edi eax,
	targetad:DWORD,
	sourcead:DWORD
; adds a string to the end of another string
; takes addresses of the two strings as parameters
; assumption: target string has enough empty space to hold the source string
	
	mov edi,targetad
	mov esi,sourcead
	CLD
L1:
	mov eax,[edi]
	cmp eax,0
	je L2
	inc edi
	loop L1
L2:
	mov ecx,ebx
	rep movsb
	ret
Str_concat endp
END main