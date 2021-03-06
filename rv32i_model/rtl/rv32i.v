`default_nettype none
`timescale 1ns / 1ps

// this takes the low two bits of the address bus and the memory access
// width and produces a storage mask.
module store_mask_t(input wire [1:0] addr,
                    input wire [2:0] width,
                    output reg [3:0] mask
                    );
  always @* begin
  case (addr)
  0: case (width)
    1:       mask = 4'b0001;
    2:       mask = 4'b0011;
    default: mask = 4'b1111;
    endcase
  1: case (width)
    1:       mask = 4'b0010;
    default: mask = 4'b0000; // illegal
    endcase
  2: case (width)
    1:       mask = 4'b0100;
    2:       mask = 4'b1100;
    default: mask = 4'b0000; // illegal
    endcase
  3: case (width)
    1:       mask = 4'b1000;
    default: mask = 4'b0000; // illegal
    endcase
  endcase
  end
endmodule

// this takes an aligned 32bit input value and correctly shifts the contents
// to reflect the address and memory access width. mirrors in_shifter_t.
module out_shifter_t(input wire [1:0] addr,
                     input wire [2:0] width,
                     input wire [31:0] in_data,
                     output reg [31:0] out_data
                     );
  always @* begin
  case (addr)
  0: case (width)
    1:       out_data = {8'd0, 8'd0, 8'd0, in_data[ 7: 0]};
    2:       out_data = {8'd0, 8'd0, in_data[15: 0]      };
    default: out_data = in_data;
    endcase
  1: case (width)
    1:       out_data = {8'd0, 8'd0, in_data[ 7: 0], 8'd0};
    default: out_data = 32'd0; // illegal
    endcase
  2: case (width)
    1:       out_data = {8'd0, in_data[ 7: 0], 8'd0, 8'd0};
    2:       out_data = {in_data[15: 0],       8'd0, 8'd0};
    default: out_data = 32'd0; // illegal
    endcase
  3: case (width)
    1:       out_data = {in_data[ 7: 0], 8'd0, 8'd0, 8'd0};
    default: out_data = 32'd0; // illegal
    endcase
  endcase
  end
endmodule

// this takes an aligned 32bit input value and correctly shifts the contents
// to reflect the address and memory access width. mirrors out_shifter_t.
module in_shifter_t(input wire [1:0] addr,
                    input wire [2:0] width,
                    input wire [31:0] in_data,
                    output reg [31:0] out_data
                    );
  always @* begin
  case (addr)
  0: case (width)
    1:       out_data = {24'd0, in_data[ 7: 0]};
    2:       out_data = {16'd0, in_data[15: 0]};
    default: out_data = in_data;
    endcase
  1: case (width)
    1:       out_data = {24'd0, in_data[15: 8]};
    default: out_data = 32'd0; // illegal
    endcase
  2: case (width)
    1:       out_data = {24'd0, in_data[23:16]};
    2:       out_data = {16'd0, in_data[31:16]};
    default: out_data = 32'd0; // illegal
    endcase
  3: case (width)
    1:       out_data = {24'd0, in_data[31:24]};
    default: out_data = 32'd0; // illegal
    endcase
  endcase
  end
endmodule

// this is a RV32I soft core CPU with a 32bit memory interface.

module rv32i_cpu_t(
    input wire clk,                   // clock
    input wire reset,                 // high to reset processor
    input wire hold,                  // high to stall cpu
    input wire [31:0] in_data,        // (word aligned) word read from memory
    output wire [3:0] out_write_mask, // 4 bit write mask
    output wire [31:0] out_mem_addr,  // memory access address
    output wire [31:0] out_data       // (word aligned) word to store in memory
    );

  localparam REG_ZERO = 0;
  localparam REG_SP   = 2;

  // note: defaults to entry point of the compliance tests
  parameter RESET_VECTOR  = 32'h00010074;
  // default stack pointer to use
  parameter STACK_POINTER = 32'hffffffff;

  reg mem_write;
  reg mem_read;
  reg [2:0] mem_width; // width of memory access (1, 2 or 4)

  // input output data shifters
  wire [31:0] mem_in;
  in_shifter_t  in_shift (out_mem_addr[1:0], mem_width, in_data, mem_in);
  // note: for store instructions the data always comes from X[rs2].
  out_shifter_t out_shift(out_mem_addr[1:0], mem_width, X[rs2], out_data);

  // write mask generator, a bit corresponds to one byte in the 32bit word
  wire [3:0] mask;
  store_mask_t mask_gen(out_mem_addr[1:0], mem_width, mask);
  assign out_write_mask = mem_write ? mask : 4'b0;

  reg [31:0] pc;    // program counter
  reg [31:0] inst;  // instruction latch
  reg [31:0] X[32]; // register file

  // instruction decode
  wire [4:0]  group  = inst[ 6: 2];
  wire [4:0]  rd     = inst[11: 7];
  wire [2:0]  funct3 = inst[14:12];
  wire [4:0]  rs1    = inst[19:15];
  wire [4:0]  rs2    = inst[24:20];
  wire [6:0]  funct7 = inst[31:25];
  wire [31:0] immi   = { {21{inst[31]}}, inst[30:20] };
  wire [31:0] immb   = { {20{inst[31]}}, inst[7], inst[30:25], inst[11:8], 1'b0 };
  wire [31:0] immu   = { inst[31:12], 12'b0 };
  wire [31:0] immj   = { {13{inst[31]}}, inst[19:12], inst[30:21], 1'b0 };
  wire [31:0] imms   = { {21{inst[31]}}, inst[30:25], inst[11:7] };

  // bit 30 differentiates some instructions (e.g. add/sub, ...)
  wire bit30 = inst[30];

  // width of memory access (1 byte, 3 half, 7 word)
  wire [2:0] access_width =  (funct3 == 0 || funct3 == 4) ? 1 :  // byte
                            ((funct3 == 1 || funct3 == 5) ? 2 :  // half
                                                            4);  // word
  // memory access address
  wire [31:0] ld_addr  = X[rs1] + immi;
  wire [31:0] st_addr  = X[rs1] + imms;
  assign out_mem_addr  = (phi == STAGE_IFETCH) ? pc :         // instruction fetch
                         (group == 5'b00000) ? ld_addr :      // load
                                               st_addr;       // store

  // signal memory read
  wire will_read  = (group == 5'b00000);                      // load
  // signal memory write
  wire will_store = (group == 5'b01000);                      // store

  // propose what the next pc should be
  wire [31:0] pc_step   = pc + 4;
  wire [31:0] pc_branch = pc + immb;
  reg [31:0] next_pc;
  always @* begin
    casez({funct3, group})
    8'b000_11000: next_pc = (X[rs1] == X[rs2])                   ? pc_branch : pc_step; // BEQ
    8'b001_11000: next_pc = (X[rs1] != X[rs2])                   ? pc_branch : pc_step; // BNE
    8'b100_11000: next_pc = ($signed(X[rs1]) <  $signed(X[rs2])) ? pc_branch : pc_step; // BLT
    8'b101_11000: next_pc = ($signed(X[rs1]) >= $signed(X[rs2])) ? pc_branch : pc_step; // BGE
    8'b110_11000: next_pc = (X[rs1] <  X[rs2])                   ? pc_branch : pc_step; // BLTU
    8'b111_11000: next_pc = (X[rs1] >= X[rs2])                   ? pc_branch : pc_step; // BGEU
    8'b???_11001: next_pc = (immi + X[rs1]) & 32'hfffffffe;                             // JALR
    8'b???_11011: next_pc = pc + immj;                                                  // JAL
    default:      next_pc = pc_step;
    endcase
  end

  // ALU
  wire [31:0] rhs = (group == 'b01100) ? X[rs2] : immi;
  reg [31:0] res_alu;
  always @* begin
    casez({bit30, funct3, group})
    9'b?_???_01101: res_alu = immu;                                       // LUI
    9'b?_???_00101: res_alu = immu + pc;                                  // AUIPC
    9'b?_???_110?1: res_alu = pc_step;                                    // JAL,  JALR
    9'b0_000_01100: res_alu = X[rs1] +    rhs;                            // ADD
    9'b?_000_00100: res_alu = X[rs1] +    rhs;                            // ADDI
    9'b1_000_01100: res_alu = X[rs1] -    rhs;                            // SUB
    9'b?_001_0?100: res_alu = X[rs1] <<  (rhs & 31);                      // SLL,  SLLI
    9'b?_010_0?100: res_alu = { 31'b0, $signed(X[rs1]) < $signed(rhs)};   // SLT,  SLTI
    9'b?_011_0?100: res_alu = { 31'b0,         X[rs1]  <         rhs };   // SLTU, SLTUI
    9'b?_100_0?100: res_alu = X[rs1] ^    rhs;                            // XOR,  XORI
    9'b0_101_0?100: res_alu = X[rs1] >>  (rhs & 31);                      // SRL,  SRLI
    9'b1_101_0?100: res_alu = $signed(X[rs1]) >>> (rhs & 31);             // SRA,  SRAI
    9'b?_110_0?100: res_alu = X[rs1] |    rhs;                            // OR,   ORI
    9'b?_111_0?100: res_alu = X[rs1] &    rhs;                            // AND,  ANDI
    9'b?_000_00000: res_alu = { {24{mem_in[ 7]}}, mem_in[ 7:0] };         // LB
    9'b?_001_00000: res_alu = { {16{mem_in[15]}}, mem_in[15:0] };         // LH
    9'b?_010_00000: res_alu = mem_in;                                     // LW
    9'b?_100_00000: res_alu = { 24'b0, mem_in[ 7:0] };                    // LBU
    9'b?_101_00000: res_alu = { 16'b0, mem_in[15:0] };                    // LHU
    default:        res_alu = 32'b0;
    endcase
  end

  // decide if the rd register will be written to
  wire rdnz = (rd != REG_ZERO);
  reg write_rd;
  always @* begin
    casez({group})
    5'b00000: write_rd = rdnz; // LD
    5'b00100: write_rd = rdnz; // OP-IMM
    5'b00101: write_rd = rdnz; // AUIPC
    5'b01100: write_rd = rdnz; // OP
    5'b01101: write_rd = rdnz; // LUI
    5'b11001: write_rd = rdnz; // JALR
    5'b11011: write_rd = rdnz; // JAL
    default:  write_rd = 0;
    endcase
  end

  localparam STAGE_IFETCH = 0;  // cycle 0
  localparam STAGE_EXEC   = 1;  // cycle 1
  localparam STAGE_PSTORE = 2;  // cycle 2 (on store)
  localparam STAGE_PLOAD  = 3;  // cycle 2 (on load)

  // two/three stage processor pipeline
  reg [1:0] phi;
  always @(posedge clk) begin
    if (reset) begin
      pc          <= RESET_VECTOR;      // reset vector
      X[REG_ZERO] <= 32'd0;             // zero register
      X[REG_SP]   <= STACK_POINTER;     // default stack pointer
      // perform instruction fetch
      phi         <= STAGE_IFETCH;      // instruction fetch stage
      mem_write   <= 0;                 // mem write line is low
      mem_read    <= 1;                 // mem read line is high (for fetch)
      mem_width   <= 4;                 // instruction word size
    end else begin

      // the hold feature should allow the cpu to interface with different speeds
      // and styles of memory.  we can stall the cpu until `mem_in` is valid or
      // `out_data` has been processed.
      if (!hold) begin
        case(phi)
        STAGE_IFETCH: begin // instruction fetch / decode

          // during this phase a new instruction should be placed on the bus
          // which we latch into the inst structure.

          // perform no memory action
          mem_read  <= 0;
          mem_write <= 0;
          mem_width <= access_width;
          // latch the new instruction
          inst <= mem_in;
          // go to execute phase
          phi <= STAGE_EXEC;

        end
        STAGE_EXEC: begin // execute
          // if this is a read instruction
          if (will_read) begin
            // perform a memory read
            mem_read  <= 1;
            mem_write <= 0;
            mem_width <= access_width;
            // go to post read phase
            phi <= STAGE_PLOAD;

          // if this is a store instruction
          end else if (will_store) begin
            // perform a memory store
            mem_read  <= 0;
            mem_write <= 1;
            mem_width <= access_width;
            // go to post store phase
            phi <= STAGE_PSTORE;

          // if this does not load/store
          end else begin
            // prep for instruction fetch
            mem_read  <= 1;
            mem_write <= 0;
            mem_width <= 4;
            // store result
            X[rd] <= write_rd ? res_alu : X[rd];
            // back to instruction fetch step
            pc <= next_pc;
            phi <= STAGE_IFETCH;

          end
        end
        STAGE_PSTORE: begin  // post store
          // prep for instruction fetch
          mem_read  <= 1;
          mem_write <= 0;
          mem_width <= 4;
          // back to instruction fetch step
          pc <= next_pc;
          phi <= STAGE_IFETCH;

        end
        STAGE_PLOAD: begin  // post load
          // prep for instruction fetch
          mem_read  <= 1;
          mem_write <= 0;
          mem_width <= 4;
          // store result
          X[rd] <= write_rd ? res_alu : X[rd];
          // back to instruction fetch step
          pc <= next_pc;
          phi <= STAGE_IFETCH;

        end
        endcase
      end
    end
  end
endmodule
