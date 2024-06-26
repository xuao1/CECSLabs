`include "./include/config.sv"
module CSR(
    input  logic [ 0:0] clk,
    input  logic [ 0:0] rstn,
    input  logic [11:0] raddr,
    input  logic [11:0] waddr,
    input  logic [ 0:0] we,
    input  logic [31:0] wdata,
    output logic [31:0] rdata,

    input  logic [ 0:0] exception_en,
    input  logic [ 0:0] interrupt_en,
    input  logic [ 3:0] exception_num,

    output logic [31:0] mtvec_global,

    input  logic [31:0] pc_wb,
    output logic [31:0] mepc_global,

    input  logic [ 0:0] mret_signal_wb 
    // Lab4 TODO: you need to add some input or output pors to implement CSRs' special functions
);
    import "DPI-C" function void set_csr_ptr(input logic [31:0] m1 [], input logic [31:0] m2 [], input logic [31:0] m3 [], input logic [31:0] m4 []);

    reg [31:0] mstatus;
    always_ff @(posedge clk) begin
        if(!rstn) begin
            mstatus <= 32'h0;
        end
        // Lab4 TODO: implement mstatus
        else if(exception_en) begin
            mstatus <= {mstatus[31:12], mstatus[8:0], 3'b110};
        end
        else if(mret_signal_wb) begin
            mstatus <= {mstatus[31:12], 3'b001, mstatus[11:3]};
        end
        else if(we && (waddr == `CSR_MSTATUS)) begin
            // mstatus <= wdata;
            // 第30-22位必须是零
            mstatus <= {wdata[31:31], 9'b0, wdata[21:0]};
        end
    end

    reg [31:0] mtvec;
    always_ff @(posedge clk) begin
        if(!rstn) begin
            mtvec <= 32'h0;
        end
        // Lab4 TODO: implement mtvec
        else if(we && (waddr == `CSR_MTVEC)) begin
            // mtvec <= wdata;
            // 最后两位必须是零
            mtvec <= {wdata[31:2], 2'b0};
        end
    end

    reg [31:0] mcause;
    always_ff @(posedge clk) begin
        if(!rstn) begin
            mcause <= 32'h0;
        end
        // Lab4 TODO: implement mcause
        else if(exception_en) begin
            mcause <= {interrupt_en, 27'b0, exception_num};
        end
        else if(we && (waddr == `CSR_MCAUSE)) begin
           // mcause <= wdata;
           // 30-4位必须是零
            mcause <= {wdata[31:31], 27'b0, wdata[3:0]};
        end
    end

    reg [31:0] mepc;
    always_ff @(posedge clk) begin
        if(!rstn) begin
            mepc <= 32'h0;
        end
        // Lab4 TODO: implement mepc
        else if(exception_en) begin
            mepc <= pc_wb;
        end
        else if(we && (waddr == `CSR_MEPC)) begin
            mepc <= wdata;
        end
    end

    // read
    always_comb begin
        case(raddr)
            `CSR_MSTATUS: rdata = mstatus;
            `CSR_MTVEC  : rdata = mtvec;
            `CSR_MCAUSE : rdata = mcause;
            `CSR_MEPC   : rdata = mepc;
            default     : rdata = 32'h0;
        endcase
    end
    initial begin
        set_csr_ptr(mstatus, mtvec, mepc, mcause);
    end

    assign mtvec_global = mtvec;
    assign mepc_global  = mepc;
endmodule