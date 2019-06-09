

@250
D=A
@SP
M=D

@500
D=A
@LCL
M=D

@1
D=A
@SP
A=M
M=D
@SP
M=M+1


@SP
AM=M-1
D=M
@R15
M=D

@LCL
D=M
@0
D=D+A
@R14 
M=D
@R15 
D=M
@R14
A=M
M=D



@2
D=A
@SP
A=M
M=D
@SP
M=M+1


@SP
AM=M-1
D=M
@R15
M=D

@LCL
D=M
@1
D=D+A
@R14 
M=D
@R15 
D=M
@R14
A=M
M=D


