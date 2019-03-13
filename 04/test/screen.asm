@SCREEN
D=A
// Set addr to screen location
@addr
M=D

//@0 // Get the height from mem address 0
@0
D=M
@n
M=D

@i // Set i to zero
M=0

(LOOP)
@i
D=M
@n
D=D-M
@END
D;JGT

@addr
A=M // Get screen addr
M=-1 // set screen to -1, which is 16 1s

@i
M=M+1
@32
D=A
@addr
M=D+M
@LOOP
0;JMP

(END)
@END
0;JMP
