// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.
// Use RAM[0] as base

// TODO: instead of statically adding mem[1] over and over again, it should add the biggest number from mem[0] or mem[1] over again in order to reduce iterations

// Clear previous result, mem[2]=0
@2
M=0

// i=0
@0
D=A
@i
M=D

// for(int i=0; i < mem[1]; i++ )
(LOOP)
// if (i >= mem[1]) then break
@i
D=M
@0
D=D-M // index-times, where times=mem[0]
@END
D;JGE

//mem[2]=mem[2]+mem[1]
@1
D=M
@2
M=M+D
// i++
@i
M=M+1
@LOOP
0;JMP

(END)
@END
0;JMP
