; Sterling LaBarbera
; 4.10.7
; this program reverses the character order in a string

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
.data
source BYTE "This is the source string",0
target BYTE SIZEOF source DUP('#')
.code
main proc
	mov esi,0
	mov eax,0
	mov ebx,0
	mov ecx,SIZEOF source
	mov edx,0
	L1:
		mov al,source[esi]
		mov target[ecx],al
		inc esi
		loop L1
	invoke ExitProcess,0
main endp
end main