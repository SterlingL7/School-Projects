; Sterling LaBarbera
; 7.10.4
; this program encrypts text with bitwise rotations
; the Author's video is the main reference for this code.

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc

.data
rawData1 BYTE "This needs to be encrypted before reading",0
rawData2 BYTE "Encrypt this after reading",0
key1 BYTE -2,4,1,0,-3,5,2,-4,-4,6
keySize = $ - key1

.code
main proc
	mov esi,OFFSET rawData1
	mov edi,OFFSET key1
	mov ecx,keySize
	call Encrypt
	mov edx,OFFSET rawData1
	call WriteString

	mov esi,OFFSET rawData2
	mov edi,OFFSET key1
	mov ecx,keySize
	call Encrypt
	mov edx,OFFSET rawData2
	call WriteString
	call WaitMsg
	invoke ExitProcess,0
main endp

 Encrypt proc
 ; this process takes offset of data in esi
 ; offset of key in edi
 ; 
	mov edx,ecx
L1:	push ecx
	cmp BYTE PTR[esi],0
	je finish
	mov cl,[edi]
	ror BYTE PTR[esi],cl
	inc edi
	inc esi
	pop ecx
	loop L1
	mov ecx,edx
	mov edi,OFFSET key1
	cmp BYTE PTR[esi],0
	jne L1
finish:
	pop ecx
	ret
Encrypt endp
END main