`include "util.v"

module mips_cpu (
  input clk,
  inout [31:0] pc,
  output [31:0] pc_new,
  output [31:0] instruction_memory_a,
  inout [31:0] instruction_memory_rd,
  output [31:0] data_memory_a,
  inout [31:0] data_memory_rd,
  output data_memory_we,
  output [31:0] data_memory_wd,
  output [4:0] register_a1,
  output [4:0] register_a2,
  output [4:0] register_a3,
  output register_we3,
  output [31:0] register_wd3,
  inout [31:0] register_rd1,
  inout [31:0] register_rd2
);

  wire [31:0] step = 32'd4; // шаг инкремента PC
  wire [31:0] instruction; // тек инструкция
  wire [5:0] opcode; // код операции
  wire [5:0] funct; // код функции 
  wire [4:0] rs; // регистр-источник 
  wire MemToReg, MemWrite, Branch, ALUSrc, RegDst, RegWrite, Jump, JumpLink, JumpReg, BranchNot; // упр сигналы
  wire [2:0] ALUControl; // сигналы управления ALU
  wire [15:0] constant; // операнд
  wire [31:0] extended_constant; // расширенный знаковый операнд
  wire [31:0] pc_plus_4; // PC + 4
  wire [31:0] constant_multiplied; // константа, сдвинутая на 2 бита влево
  wire [31:0] pc_branch; // PC + branch offset
  wire [31:0] src_b; // 2ой операнд для ALU
  wire [31:0] alu_result; // Результат работы ALU
  wire zero; // 0 от ALU
  wire inv_zero; // инвертированный 0
  wire zero_src; // источник 0
  wire branch_src; // разрешение перехода
  wire [1:0]  pc_src; // источник адреса для PC
  wire [25:0] jump_constant; // часть адреса для перехода
  wire [31:0] new_jump_constant; // полный адрес для перехода
  wire [31:0] write_data; // для записи в регистры
  wire [1:0] ch1;
  wire [1:0] ch2;

  // присваиваем адреса памяти команд
  assign instruction_memory_a = pc;
  assign instruction = instruction_memory_rd;

  // извлекаем поля из инструкции
  assign opcode = instruction[31:26];
  assign funct = instruction[5:0];
  assign rs = 5'b11111;

  // создаем управляющий блок
  control_unit cu (
    .opcode(opcode),
    .funct(funct),
    .MemToReg(MemToReg),
    .MemWrite(MemWrite),
    .Branch(Branch),
    .ALUSrc(ALUSrc),
    .RegDst(RegDst),
    .RegWrite(RegWrite),
    .Jump(Jump),
    .JumpLink(JumpLink),
    .JumpReg(JumpReg),
    .BranchNot(BranchNot),
        .ALUControl(ALUControl)
  );

  // извлекаем регистры-источники
  assign register_a1 = instruction[25:21];
  assign register_a2 = instruction[20:16];

  assign ch1 = {JumpLink, RegDst};

  // запись в рег файл (разрешение)
  assign register_we3 = RegWrite;

  // выбираем регистр назначения
  mux4_5 write_mux (
    .d0(instruction[20:16]),
    .d1(instruction[15:11]),
    .d2(rs),
    .d3(rs),
    .key(ch1),
    .out(register_a3)
  );

  // извлекаем + расширяем константу
  assign constant = instruction[15:0];
  sign_extend ext (.in(constant), .out(extended_constant));

  // PC + 4
  adder add_step (.a(step), .b(pc), .out(pc_plus_4));

  // смещение для перехода
  shl_2 shift2 (.in(extended_constant), .out(constant_multiplied));
  adder add_constant_steps (.a(constant_multiplied), .b(pc_plus_4), .out(pc_branch));

  // выбор второго операнда для ALU
  mux2_32 mux_src_b (.d0(register_rd2), .d1(extended_constant), .a(ALUSrc), .out(src_b));

  // операции ALU
  alu alu1 (.key(ALUControl), .a(register_rd1), .b(src_b), .out(alu_result), .zero(zero));

  assign inv_zero = ~zero;
  // выбор источника 0
  mux2_1 mux_to_zero (.d0(zero), .d1(inv_zero), .a(BranchNot), .out(zero_src));

  // делать ли переход
  assign branch_src = zero_src & Branch;

  // формируем управление для PC
  assign pc_src = {Jump | JumpReg, branch_src | JumpReg};

  // извлекаем адреса перехода
  assign jump_constant = instruction[25:0];
  assign new_jump_constant = {pc_plus_4[31:28], jump_constant, 2'b00};

  // выбираем следующее значение PC
  mux4_32 mux_pc_new (
    .d0(pc_plus_4),
    .d1(pc_branch),
    .d2(new_jump_constant),
    .d3(register_rd1),
    .key(pc_src),
    .out(pc_new)
  );

  // запись в память
  assign data_memory_a = alu_result;
  assign data_memory_wd = register_rd2;
  assign data_memory_we = MemWrite;

  assign ch2 = {JumpLink, MemToReg};
  // выбор для записи в регистры
  mux4_32 mux_reg_mem (
    .d0(alu_result),
    .d1(data_memory_rd),
    .d2(pc_plus_4),
    .d3(pc_plus_4),
    .key(ch2),
    .out(write_data)
  );

  // запись в регистровый файл
  assign register_wd3 = write_data;

endmodule