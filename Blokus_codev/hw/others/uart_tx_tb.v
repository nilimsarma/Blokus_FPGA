`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   05:59:58 08/25/2013
// Design Name:   uart_tx
// Module Name:   D:/VHDL_Workspace/VHDL_Xilinx/uart/uart_tx_tb.v
// Project Name:  uart
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: uart_tx
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module uart_tx_tb;

	// Inputs
	reg clk;
	reg reset;
	reg tx_start;
	reg [7:0] tx_din;

	// Outputs
	wire tx_fifo_rd;
	wire tx_busy;
	wire tx;

	wire s_tick;
	// Instantiate the Unit Under Test (UUT)
	uart_tx uart_tx_uut (
		.clk(clk), 
		.reset(reset), 
		.tx_start(tx_start), 
		.s_tick(s_tick), 
		.tx_din(tx_din), 
		.tx_fifo_rd(tx_fifo_rd),
		.tx_busy(tx_busy),
		.tx(tx)
	);
	
	mod_m_counter counter_uut (
	.clk(clk),
	.reset(reset),
	.max_tick(s_tick),
	.q()
	);
	initial
	$monitor ($time, " tx = %b", tx);
	initial begin
		// Initialize Inputs
		clk = 0;
		reset = 0;
		tx_start = 0;
		tx_din = 0;
		#3 reset = 1;
		#2 reset = 0;
		#10 tx_din = 8'b01010101;
		tx_start = 1;
		#1500 reset = 1;
		#2 reset = 0;
	end
   
	always
		#1 clk = ~clk;
		
	initial
	# 50000 $finish;

endmodule

