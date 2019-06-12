import * as fs from 'fs';
import { promisify } from 'util';
import * as path from 'path';

const readFile = promisify(fs.readFile);
const writeFile = promisify(fs.writeFile);

enum CommandType {
  C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL
}

class Parser {
  public position = 0;
  constructor (private readonly file: string[]) {}

  public hasMoreCommands(): boolean {
    return this.file.length > this.position;
  }

  public advance(): void {
    this.position++;
  }

  private current(): string {
    return this.file[this.position];
  }

  public commandType(): CommandType {
    const firstArg = this.current().split(' ')[0];
    // TODO: make this less lazy
    if ('add sub neg eq gt lt and or not'.split(' ').some(type => type === firstArg)) {
      return CommandType.C_ARITHMETIC;
    } else if (firstArg === 'push') {
      return CommandType.C_PUSH;
    } else if (firstArg === 'pop') {
      return CommandType.C_POP;
    } else if (firstArg === 'label') {
      return CommandType.C_LABEL;
    } else if (firstArg === 'goto') {
      return CommandType.C_GOTO;
    } else if (firstArg === 'if-goto') {
      return CommandType.C_IF;
    } else if (firstArg === 'function') {
      return CommandType.C_FUNCTION;
    } else if (firstArg === 'call') {
      return CommandType.C_CALL;
    } else if (firstArg === 'return') {
      return CommandType.C_RETURN;
    } else {
      throw new Error(`Invalid command type "${firstArg}"`);
    }
  }

  // commandType should not be C_RETURN
  public arg1(): string {
    if (this.commandType() === CommandType.C_ARITHMETIC) {
      return this.current().split(' ')[0];
    } else {
      return this.current().split(' ')[1];
    }
  }

  // should only be used with push/pop
  public arg2(): string {
    return this.current().split(' ')[2];
  }
}

class CodeWriter {
  // constructor (private readonly filepath: string) {}

  public static writeArithmetic(command: string, count: number): string {
    // Single value operations
    if (command === 'not') {
      return `
        @SP
        A=M-1
        M=!M
      `;
    } else if (command === 'neg') {
      return `
        @SP
        AM=M-1
        M=-M
        @SP
        M=M+1
      `;
    } else if (command === 'eq') { // TODO:
      return `
        @SP
        AM=M-1
        D=M
        A=A-1
        D=M-D
        @isEqual.true.${count}
        D;JEQ
        D=0
        @isEqual.done.${count}
        0;JMP
        (isEqual.true.${count})
          D=-1
          @isEqual.done.${count}
          0;JMP
        (isEqual.done.${count})
          @SP
          A=M-1
          M=D
      `;
    } else if (command === 'gt') {
      // first pop = y
      // second pop = x
      // x>y
      return `
        @SP
        AM=M-1
        D=M
        A=A-1
        D=M-D
        @greaterThan.true.${count}
        D;JGT
        D=0
        @greaterThan.done.${count}
        0;JMP
        (greaterThan.true.${count})
          D=-1
        (greaterThan.done.${count})
        @SP
        A=M-1
        M=D
      `;
    } else if (command === 'lt') {
      // first pop = y
      // second pop = x
      // x>y
      return `
        @SP
        AM=M-1
        D=M
        A=A-1
        D=M-D
        @lessThan.true.${count}
        D;JLT
        D=0
        @lessThan.done.${count}
        0;JMP
        (lessThan.true.${count})
          D=-1
        (lessThan.done.${count})
          @SP
          A=M-1
          M=D
      `;
    } else if (command === 'and') {
      return `
        @SP
        AM=M-1
        D=M
        A=A-1
        M=D&M
      `;
    } else if (command === 'or') {
      return `
        @SP
        AM=M-1
        D=M
        A=A-1
        M=D|M
      `;
    }

    // Two value operations
    // Let D = first on stack and M = second on stack
    let operation = ``;

    if (command === 'add') {
      return `
        @SP
        AM=M-1
        D=M
        A=A-1
        M=D+M
      `;
    } else if (command === 'sub') {
      return `
        @SP
        AM=M-1
        D=M
        A=A-1
        M=M-D 
      `;
    } else {
      throw new Error(`Arithmetic Command "${command}" was not found`);
    }
  }

  public static writePushPop(type: CommandType.C_PUSH | CommandType.C_POP, segment: string, position: number): string {
    let segmentString;
    if (segment === 'local') {
      segmentString = 'LCL';
    } else if (segment === 'argument') {
      segmentString = 'ARG';
    } else if (segment === 'this') {
      segmentString = 'THIS';
    } else if (segment === 'that') {
      segmentString = 'THAT';
    }
    if (type === CommandType.C_PUSH) {
      const push = `
        //Push
        @SP
        A=M
        M=D
        @SP
        M=M+1 
      `;
      if (segment === 'constant') {
        return `
          @${position}
          D=A
          @SP
          A=M
          M=D
          @SP
          M=M+1
        `;
      } else if ('local argument this that'.split(' ').some(segmentType => segmentType === segment)) {
        return `
          @LCL
          D=M
          @${position}
          A=D+A
          D=M
          ${push}  
        `;
          // // Set D=${segmentString}+${position}
          // @${segmentString}
          // D=M
          // @${position}
          // D=D+A
          // // R15 = ${segmentString}+${position}
          // @R15
          // M=D
          // // SP--
          // @SP
          // M=M-1
          // // set D=*SP
          // @SP
          // A=M
          // D=M
          // // Set *addr (A=R15) to *SP (D)
          // @R15
          // A=M
          // M=D 
      } else if (segment === 'static') {
        return `
          @Foo.${position}
          D=M
          ${push}
        `;
      } else if (segment === 'temp') {
        console.log({ segment, position, real: 5+position });
        return `
          @${5 + position}
          D=M
          ${push}
        `;
      } else if (segment === 'pointer') {
        let pointer = '';
        if (position === 0) {
          pointer = 'THIS';
        } else if (position === 1) {
          pointer = 'THAT';
        } else {
          throw new Error(`Invalid position "${position}" for pointer pop`);
        }
        return `
          @${pointer}
          D=M
          ${push}
        `;
      } else {
        throw new Error(`Invalid pop segment "${segment}"`)
      }
    } else if (type === CommandType.C_POP) {
      const pop = `
        //Pop
        @R15
        M=D
        @SP
        AM=M-1
        D=M
        @R15
        A=M
        M=D
      `;
      if ('local argument this that'.split(' ').some(value => value === segment)) {
        return `
          @${segmentString}
          D=M
          @${position}
          D=D+A
          ${pop}
        `;
      } else if (segment === 'static') {
        return `
          @Foo.${position}
          D=A
          ${pop}
        `;
      } else if (segment === 'temp') {
        return `
          @${5 + position}
          D=A
          ${pop}
        `;
      } else if (segment === 'pointer') {
        let pointer = '';
        if (position === 0) {
          pointer = 'THIS';
        } else if (position === 1) {
          pointer = 'THAT';
        } else {
          throw new Error(`Invalid position "${position}" for pointer pop`);
        }
        return `
          @${pointer}
          D=A
          ${pop}
        `;
      } else {
        throw new Error(`Invalid pop segment "${segment}"`)
      }
    }
  }
}

const removeUnused = (value: string): string => {
  return value
    .split('  ').join('') // Remove more than one spaces
    .replace(/\r/g, '') // remove end lines
    .replace(/\/\/.*$/gm, ''); // Remove comments
}

const convertToNumber = (x: string): number => {
  if (isNaN(Number(x))) {
    throw new Error(`Value "${x}" is not a number.`);
  }
  return parseInt(x, 10);
}

const run = async () => {
  const args = process.argv.slice(2);
  const file = (await readFile(args[0])).toString();

  const lines = removeUnused(file).split('\n').filter(line => line !== '');
  const values = [];
  const parser = new Parser(lines);

  while (parser.hasMoreCommands()) {
    const type = parser.commandType();
    let value = '';
    if (type === CommandType.C_PUSH || type === CommandType.C_POP) {
      // TODO: move number creation to arg2?
      const segment = parser.arg1();
      const position = convertToNumber(parser.arg2());
      value = CodeWriter.writePushPop(type, segment, position);
    } else if (type === CommandType.C_ARITHMETIC) {
      value = CodeWriter.writeArithmetic(parser.arg1(), parser.position);
    }
    values.push(value);
    parser.advance();
  }

  const details = values
    // .map(line => removeUnused(line))
    .filter(line => line != '')
    .join('\n');
  
  const filename = path.basename(args[0], path.extname(args[0]))
  const directory = path.dirname(args[0]);
  await writeFile(path.join(directory, `${filename}.asm`), details);
}

run();