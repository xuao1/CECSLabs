module SegReg_LS_WB#(
    parameter PC_RESET_VAL = 32'h0
)(
    input  logic [ 0:0] clk,
    input  logic [ 0:0] rstn,
    input  logic [ 0:0] stall,
    input  logic [ 0:0] flush,

    input  logic [31:0] pc_ls,
    input  logic [31:0] inst_ls,
    input  logic [31:0] alu_result_ls,
    input  logic [31:0] mem_rdata_ls,
    input  logic [ 0:0] wb_rf_sel_ls,
    input  logic [ 0:0] rf_we_ls,
    input  logic [ 0:0] commit_ls,
    input  logic [ 0:0] read_ls,

    input  logic [31:0] csr_wdata_ls,
    output logic [31:0] csr_wdata_wb,

    input  logic [ 0:0] ecall_signal_ls,
    output logic [ 0:0] ecall_signal_wb,

    input  logic [ 0:0] mret_signal_ls,
    output logic [ 0:0] mret_signal_wb,

    output logic [31:0] pc_wb,
    output logic [31:0] inst_wb,
    output logic [31:0] alu_result_wb,
    output logic [31:0] mem_rdata_wb,
    output logic [ 0:0] wb_rf_sel_wb,
    output logic [ 0:0] rf_we_wb,
    output logic [ 0:0] commit_wb,
    output logic [ 0:0] uncache_read_wb
);
    always_ff @(posedge clk) begin
        if(!rstn || flush) begin
            pc_wb           <= PC_RESET_VAL;
            inst_wb         <= 32'h13;
            alu_result_wb   <= 32'h0;
            mem_rdata_wb    <= 32'h0;
            wb_rf_sel_wb    <= 1'h0;
            rf_we_wb        <= 1'h0;
            commit_wb       <= 1'h0;
            uncache_read_wb <= 1'h0;
            csr_wdata_wb    <= 32'h0;
            ecall_signal_wb <= 1'h0;
            mret_signal_wb  <= 1'h0;
        end 
        else if(!stall) begin
            pc_wb           <= pc_ls;
            inst_wb         <= inst_ls;
            alu_result_wb <= alu_result_ls;
            mem_rdata_wb    <= mem_rdata_ls;
            wb_rf_sel_wb    <= wb_rf_sel_ls;
            rf_we_wb        <= rf_we_ls;
            commit_wb       <= commit_ls;
            uncache_read_wb <= read_ls && alu_result_ls[31:28] == 4'ha;
            csr_wdata_wb    <= csr_wdata_ls;
            ecall_signal_wb <= ecall_signal_ls;
            mret_signal_wb  <= mret_signal_ls;
        end
    end

endmodule