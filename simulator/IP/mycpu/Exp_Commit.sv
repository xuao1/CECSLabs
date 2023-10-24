`include "./include/config.sv"

// Lab4 TODO: implement the Exp_Commit module for ecall instruction
// you need to generate exception code for `syscall from machine mode`
module Exp_Commit(
    // input  logic [ 0:0] clk,
    input  logic [ 0:0] ecall_signal,
    output logic [ 0:0] interrupt_en,
    output logic [ 0:0] exception_en,
    output logic [ 3:0] exception_num
);
    // Lab4 TODO: implement the Exp_Commit module for ecall instruction
    // you need to generate exception code for `syscall from machine mode`
    assign interrupt_en = 1'b0;
    assign exception_en = ecall_signal;
    assign exception_num = 4'hb;
endmodule