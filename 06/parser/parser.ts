// I know typescript is slow - but I like it better than java!
import * as fs from 'fs';
import { promisify } from 'util';
import * as path from 'path';

enum CommandType {
  A_COMMAND, C_COMMAND, L_COMMAND
}

class Parser {
  private position = 0;
  constructor (
    private readonly file: string[]
  ) {}

  public hasMoreCommands(): boolean {
    return this.file.length > this.position;
  }

  public advance(): void {
    // if (
    //   this.file.length > this.position + 1 &&
    //   this.file[this.position + 1]
    // )
    this.position += 1;
  }

  public reset(): void {
    this.position = 0;
  }

  public getPosition(): number {
    return this.position;
  }

  public commandType(): CommandType {
    const command = this.file[this.position];
    if (command.startsWith("@")) {
      return CommandType.A_COMMAND;
    } else if (command.startsWith("(") && command.endsWith(")")) {
      return CommandType.L_COMMAND;
    } else {
      return CommandType.C_COMMAND;
    }
  }

  public symbol(): string {
    const command = this.file[this.position];
    const type = this.commandType();
    if (type === CommandType.C_COMMAND) {
      throw new Error("Can not get symbol from c command");
    } else if (type === CommandType.L_COMMAND) {
      return command.replace("(", "").replace(")", "");
    } else {
      return command.replace("@", "");
    }
  }

  public dest(): string {
    const split = this.file[this.position].split('=');
    if (split.length === 1) {
      return '';
    }
    return split[0];
  }

  public comp(): string {
    const split = this.file[this.position].split('=');
    if (split.length === 1) {
      const postJump = split[0].split(';');
      if (postJump.length === 1) {
        return '';
      }
      return postJump[0];
    }
    return split[1].split(';')[0];
  }

  public jump(): string {
    const split = this.file[this.position].split(';');
    if (split.length === 1) {
      return '';
    }
    return split[1];
  }

}

class Code {
  public static dest(dest: string): string {
    switch (dest) {
      case "M":
        return "001";
      case "D":
        return "010";
      case "MD":
        return "011";
      case "A":
        return "100";
      case "AM":
        return "101";
      case "AD":
        return "110";
      case "AMD":
        return "111";
      default: // null
        return "000";
    }
  }
  public static comp(comp: string): string {
    const type = comp.includes("M") ? "M" : "A"; // TODO: input validation
    let value = "";
    switch (comp) {
      case '0':
        value = "101010";
        break;
      case '1':
        value = "111111";
        break;
      case '-1':
        value = "111010";
        break;
      case 'D':
        value = "001100";
        break;
      case type:
        value = "110000";
        break;
      case '!D':
        value = "001101";
        break;
      case `!${type}`:
        value = "110001";
        break;
      case '-D':
        value = "001111";
        break;
      case `-${type}`:
        value = "110011";
        break;
      case 'D+1':
        value = "011111";
        break;
      case `${type}+1`:
        value = "110111";
        break;
      case 'D-1':
        value = "001110";
        break;
      case `${type}-1`:
        value = "110010";
        break;
      case `D+${type}`:
        value = "000010";
        break;
      case `D-${type}`:
        value = "010011";
        break;
      case `${type}-D`:
        value = "000111";
        break;
      case `D&${type}`:
        value = "000000";
        break;
      case `D|${type}`:
        value = "010101";
        break;
      default:
        throw new Error(`Invalid comp value "${comp}"`);
    }
    // aVALUE
    return `${type === "A" ? "0" : "1"}${value}`;
  }
  public static jump(jump: string): string {
    switch (jump) {
      case "JGT":
        return "001";
      case "JEQ":
        return "010";
      case "JGE":
        return "011";
      case "JLT":
        return "100";
      case "JNE":
        return "101";
      case "JLE":
        return "110";
      case "JMP":
        return "111";
      default: // null
        return "000";
    }
  }
}

class SymbolTable {
  private symbolTable: { [symbolName: string]: number } = {};

  public addEntry(symbol: string, address: number): void {
    if (this.contains(symbol)) {
      throw new Error(`Symbol "${symbol}" already exists in the table`);
    }
    this.symbolTable[symbol] = address;
  }

  public contains(symbol: string): boolean {
    return Object.keys(this.symbolTable).indexOf(symbol) > -1;
  }

  public getAddress(symbol: string): number {
    return this.symbolTable[symbol];
  }
}

const writeFile = promisify(fs.writeFile);
const readFile = promisify(fs.readFile);

const removeUnused = (value: string): string => {
  return value
    .split(' ').join('')
    .replace(/\r/g, '')
    .replace(/\/\/.*$/gm, '');
}

const pad = (n: string | string, width: number, z = '0'): string => {
  n = typeof n === 'string' ? n.toString() : n;
  return n.length >= width ? n : new Array(width - n.length + 1).join(z) + n;
}

const toBinary = (decimal: number, width: number): string => {
  return pad(decimal.toString(2), width);
}

const isNotNumber = (value: string | number): boolean => {
  return Number.isNaN(Number(value));
}

const run = async () => {
  const args = process.argv.splice(2);

  const table = new SymbolTable();

  for (let i = 0; i <= 15; i++) {
    table.addEntry(`R${i}`, i);
  }
  table.addEntry("SCREEN", 16384);
  table.addEntry("KBD", 24576);
  table.addEntry("SP", 0);
  table.addEntry("LCL", 1);
  table.addEntry("ARG", 2);
  table.addEntry("THIS", 3);
  table.addEntry("THAT", 4);

  let n = 16;

  // Load file
  const file = (await readFile(args[0])).toString();
  const lines = removeUnused(file)
    .split('\n')
    .filter(line => line !== '');
  const parser = new Parser(lines);
  // First round
  let offset = 0;
  while (parser.hasMoreCommands()) {
    const type = parser.commandType();
    if (type === CommandType.L_COMMAND) {
      table.addEntry(parser.symbol(), parser.getPosition() - offset);
      offset++;
    }
    parser.advance();
  }

  parser.reset();
  console.log(table);
  // Second Round
  const values: string[] = [];
  while (parser.hasMoreCommands()) {
    const type = parser.commandType();
    if (type === CommandType.A_COMMAND) {
      const symbol = parser.symbol();
      if (isNotNumber(symbol)) {
        console.log(symbol);
        console.log(table.getAddress('R0'));
        console.log(!table.contains(symbol));
        if (!table.contains(symbol)) {
          table.addEntry(symbol, n++);
        }
        console.log(table.getAddress(symbol));
        values.push(`0${toBinary(table.getAddress(symbol), 15)}`);
      } else {
        values.push(`0${toBinary(parseInt(symbol), 15)}`);
      }
    } else if (type === CommandType.C_COMMAND) {
      values.push(`111${Code.comp(parser.comp())}${Code.dest(parser.dest())}${Code.jump(parser.jump())}`)
    }
    parser.advance();
  }
  // console.log(values.join('\n'));
  // console.log(lines);
  // console.log(table);

  const filename = path.basename(args[0], path.extname(args[0]))
  const directory = path.dirname(args[0]);
  await writeFile(path.join(directory, `${filename}.hack`), values.join('\n'));
}

run();