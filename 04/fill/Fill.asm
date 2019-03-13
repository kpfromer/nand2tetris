// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed.
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

// TODO: DRY with the loops?
// for example instead of having the same loop code to write to addr
// there would only be jumps to set addr write value!

(LOOP)
@SCREEN
D=A
@addr
M=D

// I set this to 20 so I could actually run the program it should be set to 8000 for fullscreen black
@20
D=A
@addrend
M=D

@i
M=0

// If not press then go to NOPRESS else PRESS
@KBD
D=M
@NOPRESS
D;JEQ
@PRESS
D;JNE

(PRESS)
// if i>addrend then restart loop
@i
D=M
@addrend
D=M-D
@LOOP
D;JLE

// Set addr=-1 black
@addr
A=M
M=-1

// addr++
@addr
M=M+1

// i++
@i
M=M+1

@PRESS
0;JMP

(NOPRESS)
// if i>addrend then restart loop
@i
D=M
@addrend
D=M-D
@LOOP
D;JLE

// Set addr=0 white
@addr
A=M
M=0

// addr++
@addr
M=M+1

// i++
@i
M=M+1

@NOPRESS
0;JMP