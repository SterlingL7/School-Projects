; Sterling LaBarbera
; 6.11.2 #1
; this is modified from 5.9.6
; this program fills an array of DWORDs with N random ints
; j <= (value of each int) <= k
; this program should execute 2 times using different values for j and k each time
; 

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc

.data
arrayInts DWORD 20 dup(?)
N DWORD LENGTHOF arrayInts
j DWORD 0
k DWORD 0

.code
main proc
mov ecx,2
mov edx,0
mov eax,100h

LoopM:
	call RandomRange		; this part generates 2 values from 1-100h
	mov j,eax				
	mov eax,100h
	call RandomRange
	mov k,eax
	mov eax,j
	mov ebx,k
	cmp eax,ebx				; this checks the 2 values and swaps if k < j
	jbe J1
	mov k,eax
	mov j,ebx
J1:
	mov esi,OFFSET arrayInts
	mov eax,N
	mov ebx,j
	mov edx,k
	call randomInts
	push ecx
	mov edx,0
	cmp ecx,2
	je J2
	mov dx,0032h
J2:
	mov ecx,N
LoopS:					; this loop prints the values of arrayInts from 19-0
	call Gotoxy	
	sub esi,4			; it pauses before doing the 2nd runthrough, since it overwrites 
	mov eax,[esi]		; the values on the console
	call WriteInt
	inc dh
	loop LoopS
	call WaitMsg
	pop ecx
	loop LoopM
	invoke ExitProcess,0
main endp

; the procedure randomInts generates N random values between j and k
; and stores them in an array
; EAX = N, size of array
; EBX = j
; EDX = k
; ESI = offset of array to hold the characters 

randomInts proc USES ecx
	mov ecx,eax
Loop1:
	mov eax,0
	mov eax,edx
	sub eax,ebx
	call RandomRange
	add eax,ebx
	mov [esi],eax
	add esi,4
	loop Loop1
	ret
randomInts endp

END main