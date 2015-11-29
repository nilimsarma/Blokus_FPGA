`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   19:01:56 08/24/2013
// Design Name:   uart_rx
// Module Name:   D:/VHDL_Workspace/VHDL_Xilinx/Impulse/uart/uart_rx_tb.v
// Project Name:  uart
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: uart_rx
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module uart_rx_tb;

// Inputs
reg clk;
reg reset;
reg rx;

// Outputs
wire rx_fifo_wr;
wire rx_busy;
wire [7:0] rx_dout;

wire s_tick;
// Instantiate the Unit Under Test (UUT)
uart_rx uut (
	.clk(clk), 
	.reset(reset), 
	.rx(rx), 
	.s_tick(s_tick), 
	.rx_fifo_wr(rx_fifo_wr), 
	.rx_busy(rx_busy), 
	.rx_dout(rx_dout)
);

mod_m_counter counter_uut (
	.clk(clk),
	.reset(reset),
	.max_tick(s_tick),
	.q()
	);
	
initial
$monitor($time, " b_reg_done = %b", uut.b_reg_done);

initial begin
	// Initialize Inputs
	clk = 0;
	reset = 0;
	rx = 1'b1;
	#3 reset = 1'b1;
	#2 reset = 1'b0;
end

always
#1 clk = ~clk;

initial
begin
#100 rx= 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b1;
#864 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b1;

#900 rx = 1'b0;
#864 rx = 1'b0;
#864 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b1;
#864 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b1;
end

initial
#50000 $finish	;

endmodule
