; Sterling LaBarbera
; 8.11.1
; this program writes alternating light and dark squares to the screen
; in the arrangement of a Chess board
; I had to copy the color setting from the video becuase I could not get it to work
; I originally wrote this all as 1 procedure, then saw that the book asks for it to be done as 
; multiple small ones with parameters

.386
.model flat,stdcall
.stack 4096
ExitProcess proto,dwExitCode:dword
include Irvine32.inc
ChessSqare PROTO, color1:BYTE, color2:BYTE
ChessRow PROTO, color1:BYTE, color2:BYTE
ChessBoard PROTO, color1:BYTE, color2:BYTE

.data
.code
main proc
	invoke ChessBoard, lightgray, black
	mov eax,lightgray
	call SetTextColor
	call WaitMsg
	invoke ExitProcess,0
main endp

; ChessSquare and ChessSqareD write a single square of color to the screen
; each square is 2 characters in width

ChessSquare proc uses eax ebx ecx edx,
	color1:BYTE,
	color2:BYTE

	movzx eax,color1
	shl eax,4
	or al,color1
	call SetTextColor
	mov eax,0
	call WriteInt
	ret
ChessSquare endp

;ChessRow calls ChessSquare 8 times to make one row of the board

ChessRow proc uses eax ebx ecx edx,
	color1:BYTE,
	color2:BYTE
	mov ecx,4
L1:
	invoke ChessSquare, color1, color2
	invoke ChessSquare, color2, color1
	loop L1
	ret
ChessRow endp

; ChessBoard calls ChessRow 4x2 times to generate the full board

ChessBoard proc uses eax ebx ecx edx,
	color1:BYTE,
	color2:BYTE
	mov edx,0
	mov ecx,4

L1:
	invoke ChessRow, color2, color1
	inc dh
	call Gotoxy
	invoke ChessRow, color1, color2
	inc dh
	call Gotoxy
	loop L1
	ret
ChessBoard endp
END main