@SP
AM=M-1
D=M
// D = M(--SP)
A=A-1
// A = SP - 2
D=M-D
M=-1
@EQ_labelcount
D;JEQ
@SP
A=M-1
M=0
(EQ_labelcount)