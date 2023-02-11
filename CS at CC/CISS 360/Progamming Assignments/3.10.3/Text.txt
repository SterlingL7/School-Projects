;assignment 3.10 part 3 Sterling LaBarbera
;this program defines a variable of each type in table 3-2
;and intializes an appropriate value to each one

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword

.data
varBYTE BYTE FFh
varSB SBYTE 10110100b
varWORD WORD ff00h
varSW SWORD -0f0fh
varDW DWORD ffffaa11h
varSDW SDWORD -034bc24fh
varFW FWORD ffffffffffffh
varQW QWORD 15389544442134968764d
varTB TBYTE 00135745691ffadfbbbch
varREAL4 REAL4 2135499999d
varREAL8 REAL8 999999999999999999d
varREAL10 REAL10 f000000000000000000fh

.code
main proc

	invoke ExitProcess,0
main endp
end main