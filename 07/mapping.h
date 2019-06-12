#ifndef __MAPPING_H_
#define __MAPPING_H_

// PUSH

#define ASM_PUSH_CONSTANT \
  "@%d\n"                 \
  "D=A\n"                 \
  "@SP\n"                 \
  "A=M\n"                 \
  "M=D\n"                 \
  "@SP\n"                 \
  "M=M+1\n"

// %d = val + 5
#define ASM_PUSH_TEMP \
  "@%s\n"             \
  "D=M\n"             \
  "@SP\n"             \
  "A=M\n"             \
  "M=D\n"             \
  "@SP\n"             \
  "M=M+1\n"

#define ASM_PUSH_GENERIC \
  "@%d\n"                \
  "D=A\n"                \
  "@%s\n"                \
  "A=D+M\n"              \
  "D=M\n"                \
  "@SP\n"                \
  "A=M\n"                \
  "M=D\n"                \
  "@SP\n"                \
  "M=M+1\n"

// Pop

// %s is the pointer address
#define ASM_POP_PREDEFINED \
  "@%s\n"                  \
  "D=A\n"                  \
  "@R13\n"                 \
  "M=D\n"                  \
  "@SP\n"                  \
  "AM=M-1\n"               \
  "D=M\n"                  \
  "@R13\n"                 \
  "A=M\n"                  \
  "M=D\n"

#define ASM_POP_GENERIC \
  "@%d\n"               \
  "D=A\n"               \
  "@%s\n"               \
  "D=M+D\n"             \
  "@R13\n"              \
  "M=D\n"               \
  "@SP\n"               \
  "AM=M-1\n"            \
  "D=M\n"               \
  "@R13\n"              \
  "A=M\n"               \
  "M=D\n"

// Operators
#define ASM_ADD \
  "@SP\n"       \
  "AM=M-1\n"    \
  "D=M\n"       \
  "A=A-1\n"     \
  "M=D+M\n"

#define ASM_SUB \
  "@SP\n"       \
  "AM=M-1\n"    \
  "D=M\n"       \
  "A=A-1\n"     \
  "M=M-D\n"

#define ASM_NOT \
  "@SP\n"       \
  "A=M-1\n"     \
  "M=!M\n"

#define ASM_NEG \
  "@SP\n"       \
  "A=M-1\n"     \
  "M=-M\n"

#define ASM_AND \
  "@SP\n"       \
  "AM=M-1\n"    \
  "D=M\n"       \
  "A=A-1\n"     \
  "M=M&D\n"

#define ASM_OR \
  "@SP\n"      \
  "AM=M-1\n"   \
  "D=M\n"      \
  "A=A-1\n"    \
  "M=M|D\n"

#define ASM_EQ \
  "@SP\n"      \
  "AM=M-1\n"   \
  "D=M\n"      \
  "A=A-1\n"    \
  "D=M-D\n"    \
  "M=-1\n"     \
  "@EQ_%d\n"   \
  "D;JEQ\n"    \
  "@SP\n"      \
  "A=M-1\n"    \
  "M=0\n"      \
  "(EQ_%d)\n"

#define ASM_GT \
  "@SP\n"      \
  "AM=M-1\n"   \
  "D=M\n"      \
  "A=A-1\n"    \
  "D=M-D\n"    \
  "M=-1\n"     \
  "@GT_%d\n"   \
  "D;JGT\n"    \
  "@SP\n"      \
  "A=M-1\n"    \
  "M=0\n"      \
  "(GT_%d)\n"

#define ASM_LT \
  "@SP\n"      \
  "AM=M-1\n"   \
  "D=M\n"      \
  "A=A-1\n"    \
  "D=M-D\n"    \
  "M=-1\n"     \
  "@LT_%d\n"   \
  "D;JLT\n"    \
  "@SP\n"      \
  "A=M-1\n"    \
  "M=0\n"      \
  "(LT_%d)\n"

#endif