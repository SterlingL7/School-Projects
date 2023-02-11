; Sterling LaBarbera
; numlist
; this is a program for filtering all non prime numbers from a list
; it compares the index of an array of n elements to each value not already filtered out
; and sets the value of the element to 1 if to indicate non prime

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc
PrintP PROTO, count:DWORD
FPRIME = 2
LPRIME = 1000

.data
primelist BYTE 1000 DUP(0)

.code
main proc
	mov ecx,LPRIME
	mov edi,OFFSET primelist
	mov al,0
	mov esi,FPRIME
L1:
	cmp esi,LPRIME
	jge L2
	cmp primelist[esi*TYPE primelist],0
	jne L3
	call Sieve
L3:
	inc esi
	jmp L1
L2:
	mov esi,OFFSET primelist
	INVOKE PrintP, LPRIME
	call WaitMsg
	invoke ExitProcess,0
main endp

; sieve puts a 1 in every element with an index divisible by the esi
;takes the current value in esi

Sieve proc
	push eax
	push esi
	mov eax,esi
	add esi,eax
L1:	cmp esi,LPRIME
	ja L2
	mov primelist[esi*TYPE primelist],1
	add esi,eax
	jmp L1
L2:
	pop esi
	pop eax
	ret
sieve endp

; PrintP prints all the primes in the range

PrintP proc,
	count:DWORD
	mov eax,0
	mov esi,1
	mov ecx,count
L1:
	cmp primelist[esi*TYPE primelist],1
	je L2
	mov eax,esi
	call WriteDec
	call Crlf
L2:	inc esi
	loop L1
	ret
PrintP endp
END main