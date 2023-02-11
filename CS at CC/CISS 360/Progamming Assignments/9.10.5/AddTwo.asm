; Sterling LaBarbera
; 9.10.5
; this program searches a string for a specified character and returns the pointer to the next character in EAX
; after inserting a null byte where the specified character was
; Zero Flag is set if character is found
; target string taken from en.wikipedia.org/wiki/Psi_Orionis 
; I modified it slightly so it would have a period 
; unfortunately could not use the greek letter psi in the strings

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc
Str_nextWord PROTO targetad:DWORD, delimiter:BYTE

.data
target BYTE "The Bayer designation is Psi Orionis (Ori/Orionis). It is shared by two stars in the constellation Orion: ",0
prompt1 BYTE "Enter a character to find: ",0
prompt2 BYTE "Character was not found in the target string.",0
prompt3 BYTE "The second string: ",0
prompt4 BYTE "The first string: ",0
delimit BYTE "0"
.code
main proc
	mov edx,OFFSET target
	call WriteString
	call crlf

	mov edx,OFFSET prompt1
	call WriteString
	call ReadChar
	call crlf

	mov delimit,al
	invoke Str_NextWord, ADDR target, delimit
	jnz notFound
	mov edx,OFFSET prompt4
	call WriteString
	mov edx,OFFSET target
	call WriteString
	call crlf

	mov edx,OFFSET prompt3
	call WriteString
	mov edx,eax
	call WriteString

	jmp final
notFound:
	mov edx,OFFSET prompt2
	call WriteString
	
final:
	call crlf
	call WaitMsg
	invoke ExitProcess,0
main endp

Str_nextWord proc uses esi,
	targetad:DWORD,
	delimiter:BYTE

	movzx eax,delimiter
	mov esi,targetad
L1:
	cmp al,[esi]
	je L2
	inc esi
	loop L1
	jmp L3
L2:
	inc esi
	mov eax,esi
	mov BYTE PTR [esi - 1],0
	mov ebx,1
	sub ebx,1
L3:	ret
Str_nextWord endp
END main