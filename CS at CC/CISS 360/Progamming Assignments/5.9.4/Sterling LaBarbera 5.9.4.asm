; Sterling LaBarbera
; 5.9.4
; this program clears the screen,
; locates the cursor near the center,
; and gets two integer inputs to add together
; the sum is returned in eax
; repeats 3 times

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc

.data
int1 DWORD 00000000h
int2 DWORD 00000000h

.code
main proc
	mov ecx,3

	L1:
		call Clrscr
		mov dh,15
		mov dl,60
		call Gotoxy
		call ReadInt
		mov int1,eax
		inc dh
		call Gotoxy
		call ReadInt
		add eax,int1
		inc dh
		call Gotoxy
		call WriteInt
		call WaitMsg
		mov eax,0
	loop L1
	invoke ExitProcess,0
main endp
end main