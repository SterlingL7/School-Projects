; Sterling LaBarbera
; 5.9.6
; this program generates 20 random strings
; of length L and writes them to the console

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc

.data
randstring BYTE 20 dup(?),0
L DWORD LENGTHOF randstring-1

.code
main proc
mov ecx,20
mov edx,0

LoopM:
	mov esi,OFFSET randstring
	mov eax,L
	call randomString
	push edx
	mov edx,OFFSET randstring
	call WriteString
	pop edx
	inc dh 
	call Gotoxy
	loop LoopM
	call WaitMsg
	invoke ExitProcess,0
main endp

; the procedure randomString generates one random string of all capital letters
; EAX = length of the string
; ESI = offset of array to hold the characters

randomString proc USES ecx
	mov ecx,eax
Loop1:
	mov eax,0
	mov al,26d
	call RandomRange
	add al,'A'
	mov [esi],al
	inc esi
	loop Loop1
	ret
randomString endp

END main