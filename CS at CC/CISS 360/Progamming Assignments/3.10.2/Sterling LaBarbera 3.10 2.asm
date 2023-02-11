;assignment 3.10 part 2 Sterling LaBarbera
;this program assigns an int value to each day of the week: 1 = Sunday ... 7 = Saturday
;

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword

Sunday EQU 1
Monday EQU 2
Tuesday EQU 3
Wednesday EQU 4
Thursday EQU 5
Friday EQU 6
Saturday EQU 7
	
.data
	daysofweek BYTE Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday
	ListSize = ($ - daysofweek)

.code
main proc
	add eax, daysofweek


	invoke ExitProcess,0
main endp
end main