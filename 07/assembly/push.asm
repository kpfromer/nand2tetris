// Constant value 10
@10
D=A
@SP
A=M
M=D
@SP
M=M+1
//*SP=i,SP++

// Push temp
@(5+i) // 5 + i done in compiler
D=M
@SP
A=M
M=D
@SP
M=M+1
// addr=5+i,*SP=*addr,SP++

// Generic for pointer LCL at relative address i
@i
D=A
@LCL
A=D+M
D=M
// D=*(SP+i)
@SP
A=M
M=D
@SP
M=M+1