; Sterling LaBarbera
; 5.9.8
; this program generates a matrix of characters
; that represents each foreground + background combination
; for the console window.

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc

.data
.code
main proc
	mov ecx,16
	mov eax,0
	mov edx,0
LoopM:
	call FGcolorChg
	call SetTextColor
	
	inc ah
	call Crlf
	loop LoopM
	mov eax,7
	call SetTextColor
	call WaitMsg
	invoke ExitProcess,0
main endp

FGcolorChg proc uses ecx
	mov ecx,16
L1:
	call SetTextColor
	push eax
	mov al,'#'
	call WriteChar
	pop eax
	inc al
	loop L1
	ret
FGcolorChg endp
END main