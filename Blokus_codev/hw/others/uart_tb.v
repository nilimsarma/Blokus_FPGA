`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   01:50:50 08/27/2013
// Design Name:   uart
// Module Name:   D:/VHDL_Workspace/VHDL_Xilinx/uart/uart_tb.v
// Project Name:  uart
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: uart
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module uart_tb;

	// Inputs
	reg reset_pin;
	reg clk;
	reg rx;

	// Outputs
	wire tx;
	wire rx_busy;
	wire tx_busy;

	// Instantiate the Unit Under Test (UUT)
	uart uut (
		.reset_pin(reset_pin), 
		.clk(clk), 
		.rx(rx), 
		.tx(tx), 
		.rx_busy(rx_busy), 
		.tx_busy(tx_busy)
	);
	
	initial
	$monitor ($time, " tx = %b", tx);
	initial begin
		// Initialize Inputs
		reset_pin = 1;
		clk = 0;
		rx = 0;
		#30 reset_pin = 0;
		#20 reset_pin = 1;
		
		#1000 rx = 1'b0;
		#8640 rx = 1'b1;
		#8640 rx = 1'b1;
		#8640 rx = 1'b0;
		#8640 rx = 1'b1;
		#8640 rx = 1'b0;
		#8640 rx = 1'b1;
		#8640 rx = 1'b0;
		#8640 rx = 1'b1;
		#8640 rx = 1'b1;

		#9000 rx = 1'b0;
		#8640 rx = 1'b0;
		#8640 rx = 1'b0;
		#8640 rx = 1'b1;
		#8640 rx = 1'b1;
		#8640 rx = 1'b0;
		#8640 rx = 1'b1;
		#8640 rx = 1'b0;
		#8640 rx = 1'b1;
		#8640 rx = 1'b1;
	end
   
	always
		#10 clk = ~clk;
	//initial
		//#600000 $finish;
endmodule

