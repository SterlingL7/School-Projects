; AddTwo.asm - adds two 32-bit integers.
; Chapter 3 example

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
.data
testArray DWORD 0h,4h,5h,5h,7h,8h,11h,15h
.code
main proc
	mov esi,OFFSET testArray
	mov eax,0
	mov ebx,0
	mov ecx,LENGTHOF testArray
	mov edx,0
	L1:
		mov eax,[esi]
		add esi,4
		mov ebx,[esi]
		sub ebx,eax
		add edx,ebx
		loop L1
	invoke ExitProcess,0
main endp
end main