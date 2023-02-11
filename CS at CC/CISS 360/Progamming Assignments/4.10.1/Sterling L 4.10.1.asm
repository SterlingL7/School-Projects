; Sterling LaBarbera
; 4.10.1
; this program reverses the order of bytes in an array and puts them in a DWORD

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
.data
bigEndian BYTE 12h,34h,56h,78h
littleEndian DWORD ?

.code
main proc
	mov al,bigEndian[0]
	mov BYTE ptr littleEndian+3,al
	mov al,bigEndian[1]
	mov BYTE ptr littleEndian+2,al
	mov al,bigEndian[2]
	mov BYTE ptr littleEndian+1,al
	mov al,bigEndian[3]
	mov BYTE ptr littleEndian,al
	invoke ExitProcess,0
main endp
end main