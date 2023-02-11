; Sterling LaBarbera
; 6.11.2 #4
; this program is mostly from the text example from 6.7.3
; it is modified to use cmp/jmp instead of .IF directives
; checks credit range
; prompts for grade average and credit values
; shows a msg of whether the student can or cant register

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc

.data
TRUE = 1
FALSE = 0
gradeAverage DWORD ?
credits DWORD ?
OkToRegister BYTE ?
prompt1 BYTE "Enter your grade average ",0
prompt2 BYTE "Enter your number of credits ",0
error BYTE "You may not register",0
OKmsg BYTE "You may register",0
creditError BYTE "That is not an acceptable value for credits. It must be between 1 and 30",0

.code
main proc
	mov edx,OFFSET prompt1
	call WriteString
	call ReadInt
	mov gradeAverage,eax
credIn:
	mov edx,OFFSET prompt2
	call WriteString
	call ReadInt
	mov credits,eax
	cmp word ptr credits,30
	jb c1
	mov edx,OFFSET creditError
	call WriteString
	jmp credIn
c1:
	cmp word ptr credits,1
	jae check
	mov edx,OFFSET creditError
	call WriteString
	jmp credIn
check:
	mov OkToRegister,FALSE
	cmp word ptr gradeAverage,350
	jbe NO1
	jmp goodEnd
NO1:
	cmp word ptr gradeAverage,250
	jbe NO2
	cmp word ptr credits,16
	ja badEnd
	jmp goodEnd
NO2:
	cmp word ptr credits,12
	jbe goodEnd
	jmp badEnd
goodEnd:
	mov OkToRegister,TRUE
	mov edx,OFFSET OKmsg
	call WriteString
	jmp done
badEnd:
	mov edx,OFFSET error
	call WriteString
done:
	call WaitMsg
	invoke ExitProcess,0
main endp
END main