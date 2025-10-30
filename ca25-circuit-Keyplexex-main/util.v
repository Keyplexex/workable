// расширение знака
module sign_extend (
  input [15:0] in,
  output [31:0] out
);
  assign out = {{16{in[15]}}, in};
endmodule

// сдвиг влево на 2 бит
module shl_2 (
  input [31:0] in,
  output [31:0] out
);
  assign out = {in[29:0], 2'b00};
endmodule

// 32-битный сумматор
module adder (
  input [31:0] a,
  input [31:0] b,
  output [31:0] out
);
  assign out = a + b;
endmodule

// 32-битный мультиплексор
module mux2_32 (
  input [31:0] d0,
  input [31:0] d1,
  input a,
  output [31:0] out
);
  assign out = a ? d1 : d0;
endmodule

// 1-битный мультиплексор
module mux2_1 (
  input d0,
  input d1,
  input a,
  output out
);
  assign out = a ? d1 : d0;
endmodule

// 5-битный мультиплексор
module mux2_5 (
  input [4:0] d0,
  input [4:0] d1,
  input a,
  output [4:0] out
);
  assign out = a ? d1 : d0;
endmodule

// 32-битный мультиплексор
module mux4_32 (
  input [31:0] d0,
  input [31:0] d1,
  input [31:0] d2,
  input [31:0] d3,
  input [1:0]  key,
  output reg [31:0] out
);
  always @(key or d0 or d1 or d2 or d3) begin
    case (key)
      2'b00: out = d0;
      2'b01: out = d1;
      2'b10: out = d2;
      2'b11: out = d3;
      default: out = d0; // бежим от latch
    endcase
  end
endmodule

// 5-битный мультиплексор
module mux4_5 (
  input [4:0] d0,
  input [4:0] d1,
  input [4:0] d2,
  input [4:0] d3,
  input [1:0] key,
  output reg [4:0] out
);
  always @(key or d0 or d1 or d2 or d3) begin
    case (key)
      2'b00: out = d0;
      2'b01: out = d1;
      2'b10: out = d2;
      2'b11: out = d3;
      default: out = d0; // все еще бежим от latch
    endcase
  end
endmodule


// управление
module control_unit (
  input [5:0] opcode,
  input [5:0] funct,
  output MemToReg,
  output MemWrite,
  output Branch,
  output ALUSrc,
  output RegDst,
  output RegWrite,
  output Jump,
  output JumpLink,
  output JumpReg,
  output BranchNot,
  output [2:0] ALUControl
);
  wire [1:0] ALUOp;

  main_decoder md (
    .opcode(opcode),
    .MemToReg(MemToReg),
    .MemWrite(MemWrite),
    .Branch(Branch),
    .ALUSrc(ALUSrc),
    .RegDst(RegDst),
    .RegWrite(RegWrite),
    .Jump(Jump),
    .JumpLink(JumpLink),
    .BranchNot(BranchNot),
    .ALUOp(ALUOp)
      );
  ALU_decoder alud (
    .funct(funct),
    .ALUOp(ALUOp),
    .ALUControl(ALUControl),
    .JumpReg(JumpReg)
  );
endmodule

module main_decoder (
  input [5:0] opcode,
  output reg MemToReg,
  output reg MemWrite,
  output reg Branch,
  output reg ALUSrc,
  output reg RegDst,
  output reg RegWrite,
  output reg Jump,
  output reg JumpLink,
  output reg BranchNot,
  output reg [1:0] ALUOp
);
  always @(opcode) begin
    case (opcode)
      // bne
      6'b000101: begin
        RegWrite <= 1'b0;
        RegDst <= 1'b0;
        ALUSrc <= 1'b0;
        Branch <= 1'b1;
        BranchNot <= 1'b1;
        MemWrite <= 1'b0;
        MemToReg <= 1'b0;
        ALUOp <= 2'b00;
        Jump <= 1'b0;
        JumpLink <= 1'b0;
      end
      // add
      6'b000000: begin
        RegWrite <= 1'b1;
        RegDst <= 1'b1;
        ALUSrc <= 1'b0;
        Branch <= 1'b0;
        BranchNot <= 1'b0;
        MemWrite <= 1'b0;
        MemToReg <= 1'b0;
        ALUOp <= 2'b10;
        Jump <= 1'b0;
        JumpLink <= 1'b0;
      end
      // addi
      6'b001000: begin
        RegWrite <= 1'b1;
        RegDst <= 1'b0;
        ALUSrc <= 1'b1;
        Branch <= 1'b0;
        BranchNot <= 1'b0;
        MemWrite <= 1'b0;
        MemToReg <= 1'b0;
        ALUOp <= 2'b00;
        Jump <= 1'b0;
        JumpLink <= 1'b0;
      end
      // andi
      6'b001100: begin
        RegWrite <= 1'b1;
        RegDst <= 1'b0;
        ALUSrc <= 1'b1;
        Branch <= 1'b0;
        BranchNot <= 1'b0;
        MemWrite <= 1'b0;
        MemToReg <= 1'b0;
        ALUOp <= 2'b11;
        Jump <= 1'b0;
        JumpLink <= 1'b0;
      end
      // lw
      6'b100011: begin
        RegWrite <= 1'b1;
        RegDst <= 1'b0;
        ALUSrc <= 1'b1;
        Branch <= 1'b0;
        BranchNot <= 1'b0;
        MemWrite <= 1'b0;
        MemToReg <= 1'b1;
        ALUOp <= 2'b00;
        Jump <= 1'b0;
        JumpLink <= 1'b0;
      end
      // sw
      6'b101011: begin
        RegWrite <= 1'b0;
        RegDst <= 1'b0;
        ALUSrc <= 1'b1;
        Branch <= 1'b0;
        BranchNot <= 1'b0;
        MemWrite <= 1'b1;
        MemToReg <= 1'b0;
        ALUOp <= 2'b00;
        Jump <= 1'b0;
        JumpLink <= 1'b0;
      end
      // beq
      6'b000100: begin
        RegWrite <= 1'b0;
        RegDst <= 1'b0;
        ALUSrc <= 1'b0;
        Branch <= 1'b1;
        BranchNot <= 1'b0;
        MemWrite <= 1'b0;
        MemToReg <= 1'b0;
        ALUOp <= 2'b01;
        Jump <= 1'b0;
        JumpLink <= 1'b0;
      end
      // j
      6'b000010: begin
        RegWrite <= 1'b0;
        RegDst <= 1'b0;
        ALUSrc <= 1'b0;
        Branch <= 1'b0;
        BranchNot <= 1'b0;
        MemWrite <= 1'b0;
        MemToReg <= 1'b0;
        ALUOp <= 2'b00;
        Jump <= 1'b1;
        JumpLink <= 1'b0;
      end
      // jal
      6'b000011: begin
        RegWrite <= 1'b1;
        RegDst <= 1'b0;
        ALUSrc <= 1'b0;
        Branch <= 1'b0;
        BranchNot <= 1'b0;
        MemWrite <= 1'b0;
        MemToReg <= 1'b0;
        ALUOp <= 2'b00;
        Jump <= 1'b1;
        JumpLink <= 1'b1;
      end
    endcase
  end
endmodule

module ALU_decoder (
  input [5:0] funct,
  input [1:0] ALUOp,
  output reg [2:0] ALUControl,
  output reg JumpReg
);
  always @(funct or ALUOp) begin
      JumpReg <= 1'b0;
    case (ALUOp)
      2'b00: ALUControl <= 3'b010; // addi, lw, sw
      2'b01: ALUControl <= 3'b110; // beq
      2'b11: ALUControl <= 3'b000; // andi
      2'b10: begin
        case (funct)
          6'b100000: ALUControl <= 3'b010; // add
          6'b100010: ALUControl <= 3'b110; // sub
          6'b100100: ALUControl <= 3'b000; // and
          6'b100101: ALUControl <= 3'b001; // or
          6'b101010: ALUControl <= 3'b111; // slt
          6'b001000: begin
            JumpReg <= 1'b1;
            ALUControl <= 3'b000;
          end
        endcase
      end
    endcase
  end
endmodule

module alu (
  input [2:0] key,
  input [31:0] a,
  input [31:0] b,
  output reg [31:0] out,
  output reg zero
);
  always @(key or a or b) begin
    zero <= (a == b); // флаг 0
    case(key)
      3'b000: out = a & b;      // AND
      3'b001: out = a | b;      // OR
      3'b010: out = a + b;      // ADD
      3'b011: out = a - b;      // SUB
      3'b100: out = a & (~b);   // AND NOT
      3'b101: out = a | (~b);   // OR NOT
      3'b110: out = a - b;      // SUB
      3'b111: out = (a < b) ? 1 : 0;  // SLT
    endcase
  end
endmodule