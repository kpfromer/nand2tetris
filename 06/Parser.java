import java.util.ArrayList;
import java.util.HashMap;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.io.PrintWriter;

enum CommandType {
  A_COMMAND,
  C_COMMAND,
  L_COMMAND
}

class Parser {
  private ArrayList<String> commands;
  private int pos;

  public Parser(String fileLocation) {
    pos = 0;
    commands = new ArrayList<>();

    try {
      Scanner s = new Scanner(new File(fileLocation));
      while (s.hasNextLine()){
        String val = s.nextLine();
        if (!val.equals("") && !val.startsWith("//")) { // Remove whitespace
          String finalV = val
            .replaceAll("\\s+", "");
          int offset = finalV.indexOf("//");

          if (offset != -1) {
            finalV = finalV.substring(0, offset);
          }
          if (!finalV.equals("")) {
            commands.add(finalV);
          }
        }
      }
      s.close();
    } catch (FileNotFoundException e) {
      System.out.println("Unable to find file.");
    }
    // for (int i = 0; i < commands.size(); i++) {
    //   System.out.println(String.format("\"%s\"", commands.get(i)));
    // }
    // System.out.println(commands.toString());
  }

  public boolean hasMoreCommands() {
    return commands.size() > pos;
  }

  public void advance() {
    // if (!hasMoreCommands()) {
    //   throw new Error("Can not advance further");
    // }
    pos++;
    // if (pos + 1 < commands.size() && commands.get(pos + 1).startsWith("//")) {
    //   this.advance();
    // }
  }

  public CommandType commandType() {
    String command = commands.get(pos);
    if (command.startsWith("@")) {
      return CommandType.A_COMMAND;
    } else if (command.startsWith("(") && command.endsWith(")")) {
      return CommandType.L_COMMAND;
    } else {
      return CommandType.C_COMMAND;
    }
  }

  public String symbol() {
    CommandType commandType = commandType();
    if (commandType == CommandType.C_COMMAND) {
      throw new Error("Can not get symbol from c command");
    }
    String command = commands.get(pos);
    if (commandType == CommandType.L_COMMAND) {
      return command.replace("(", "").replace(")", "");
    }
    return command.replace("@", "");
  }

  public String dest() {
    String[] split = commands.get(pos).split("=");
    if (split.length == 1) { // There is no assignement (no equals sign)
      return "";
    }
    return split[0];
  }

  public String comp() {
    String[] split = commands.get(pos).split("=");
    if (split.length == 1) {
      String[] postJump = split[0].split(";");
      if (postJump.length == 1) {
        return "";
      }
      return postJump[0];
    }
    return split[1].split(";")[0];
  }

  public String jump() {
    String[] split = commands.get(pos).split(";");
    // TODO: what if "A=D;"
    if (split.length == 1) { // not jump
      return "";
    }
    return split[1];
  }

  public static String intToBinary(int number, int padding) {
    return String.format("%" + padding +"s", Integer.toBinaryString(number)).replace(' ', '0');
  }

  public static boolean isNumeric(String str) {
    for (char c : str.toCharArray()) {
      if (!Character.isDigit(c)) return false;
    }
    return true;
  }

  public static void main(String[] args) {
    SymbolTable symbols = new SymbolTable();

    // SYMBOLS
    // R0 = 0, R1 = 1, ... R15 = 15
    for (int i = 0; i <= 15; i++) {
      symbols.addEntry("R"+i, i);
    }
    symbols.addEntry("SCREEN", 16384);
    symbols.addEntry("KBD", 24576);
    symbols.addEntry("SP", 0);
    symbols.addEntry("LCL", 1);
    symbols.addEntry("ARG", 2);
    symbols.addEntry("THIS", 3);
    symbols.addEntry("THAT", 4);

    int n = 16;

    Parser parser = new Parser(args[0]);
    String output = "";

    // First pass to generate label positions
    int offset = 0;
    while (parser.hasMoreCommands()) {
      if (parser.commandType() == CommandType.L_COMMAND) {
        System.out.println(parser.symbol() + " pos " + parser.pos);
        if (!symbols.contains(parser.symbol())) {
          symbols.addEntry(parser.symbol(), parser.pos - offset);
          offset++;
        }
      }
      parser.advance();
    }

    parser.pos = 0;
    while (parser.hasMoreCommands()) {
      CommandType type = parser.commandType();
      String command = "";
      if (type == CommandType.L_COMMAND) {
        // if (!symbols.contains(parser.symbol())) {
        //   symbols.addEntry(parser.symbol(), parser.pos + 1);
        // }
        // TODO: else throw error duplicate labels
      } else if (type == CommandType.A_COMMAND) {
        String symbol = parser.symbol();
        String value = "";
        if (!isNumeric(symbol)) {
          if (!symbols.contains(symbol)) {
            symbols.addEntry(symbol, n++);
          }
          value = intToBinary(symbols.getAddress(symbol), 15);
        } else {
          value = intToBinary(Integer.parseInt(symbol), 15);
        }
        command = "0" + value;
      } else {
        command = "111" + Code.comp(parser.comp()) + Code.dest(parser.dest()) + Code.jump(parser.jump());
      }
      if (command != "") {
        output += command + "\n";
      }
      parser.advance();
    }

    String filename = new File(args[0]).getName();
    int extensionPos = filename.indexOf(".");
    if (extensionPos > 0) {
      filename = filename.substring(0, extensionPos);
    }

    try {
      PrintWriter writer = new PrintWriter(String.format("%s.hack", filename));
      writer.print(output);
      writer.close();
    } catch (FileNotFoundException e) {
      System.out.println("Can not write to file.");
    }

  }
}

class Code {
  public static String dest(String dest) {
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

  public static String comp(String comp) {
    String type = comp.contains("M") ? "M" : "A"; // TODO: input validation
    String value = "";
    // System.out.println("IN COMP");
    // System.out.println("type");
    // System.out.println(type);
    // System.out.println(comp.equals(type));
    if (comp.equals("0")) {
      value = "101010"; 
    } else if (comp.equals("1")) {
      value = "101010";
    } else if (comp.equals("-1")) {
      value = "111010";
    } else if (comp.equals("D")) {
      value = "001100";
    } else if (comp.equals(type)) {
      value = "110000";
    } else if (comp.equals("!D")) {
      value = "001101";
    } else if (comp.equals("!"+type)) {
      value = "110001";
    } else if (comp.equals("-D")) {
      value = "001111";
    } else if (comp.equals("-"+type)) {
      value = "110011";
    } else if (comp.equals("D+1")) {
      value = "011111";
    } else if (comp.equals(type+"+1")) {
      value = "110111";
    } else if (comp.equals("D-1")) {
      value = "001110";
    } else if (comp.equals(type+"-1")) {
      value = "110010";
    } else if (comp.equals("D+"+type)) {
      value = "000010";
    } else if (comp.equals("D-"+type)) {
      value = "010011";
    } else if (comp.equals(type+"-D")) {
      value = "000111";
    } else if (comp.equals("D&"+type)) {
      value = "000000";
    } else if (comp.equals("D|"+type)) { // "D|"+type
      value = "010101";
    } else {
      throw new Error(String.format("Invalid comp value \"%s\" found", comp));
    }
    // aVALUE
    return (type == "A" ? "0" : "1") + value;
  }

  public static String jump(String jump) {
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
  // I could create my own implementation with an ArrayList
  private HashMap<String, Integer> symbolTable;

  SymbolTable() {
    symbolTable = new HashMap<>();
  }

  public void addEntry(String symbol, int address) {
    symbolTable.put(symbol, address);
  }

  public boolean contains(String symbol) {
    return symbolTable.containsKey(symbol);
  }

  public int getAddress(String symbol) {
    return symbolTable.get(symbol);
  }
}