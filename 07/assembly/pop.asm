// Pop from stack to address at LCL+i
@1 // i
D=A
@LCL
D=M+D
// If I had more registers I could avoid storing to memory
@R13
M=D
@SP
AM=M-1
D=M
@R13
A=M
M=D