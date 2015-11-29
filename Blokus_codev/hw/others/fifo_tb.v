`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   03:48:28 08/25/2013
// Design Name:   fifo
// Module Name:   D:/VHDL_Workspace/VHDL_Xilinx/uart/fifo_tb.v
// Project Name:  uart
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: fifo
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module fifo_tb;

	// Inputs
	reg clk;
	reg reset;
	reg rd;
	reg wr;
	reg [7:0] w_data;

	// Outputs
	wire empty;
	wire full;
	wire [7:0] r_data;

	// Instantiate the Unit Under Test (UUT)
	fifo uut (
		.clk(clk), 
		.reset(reset), 
		.rd(rd), 
		.wr(wr), 
		.w_data(w_data), 
		.empty(empty), 
		.full(full), 
		.r_data(r_data)
	);
	
	initial
	$monitor($time, " array_reg = %h, full = %b, empty = %b, rd = %b, wr = %b, r_data = %h",
				uut.array_reg, uut.full, uut.empty, uut.rd, uut.wr, uut.r_data);
	initial begin
		// Initialize Inputs
		clk = 0;
		reset = 0;
		rd = 0;
		wr = 0;
		w_data = 0;
		#3 reset = 1;
		#2 reset = 0;
	end
   
	always
	 #1 clk = ~clk;
	
	initial 
	begin
		#11 w_data = 8'h0A;
		wr = 1;
		#2 wr = 0;
		
		#10 w_data = 8'h0B;
		wr = 1;
		#2 wr = 0;
		
		#10 w_data = 8'h0C;
		wr = 1;
		#2 wr = 0;
		
		#10 w_data = 8'h0D;
		wr = 1;
		#2 wr = 0;
		
		#10 rd = 1;
		#2 rd = 0;
		
		#10 rd = 1;
		#2 rd = 0;
		
		#10 w_data = 8'h0E;
		wr = 1;
		#2 wr = 0;
		
		#10 w_data = 8'h0F;
		wr = 1;
		#2 wr = 0;
		
		#10 rd = 1;
		#2 rd = 0;

		#10 rd = 1;
		#2 rd = 0;
		
		#10 rd = 1;
		#2 rd = 0;
		
		#10 rd = 1;
		#2 rd = 0;
		
	end
	initial
	#200 $finish;
	
endmodule

