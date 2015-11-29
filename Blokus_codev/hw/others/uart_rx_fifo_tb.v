`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   20:25:25 08/24/2013
// Design Name:   uart_rx_fifo
// Module Name:   D:/VHDL_Workspace/VHDL_Xilinx/uart/uart.v
// Project Name:  uart
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: uart_rx_fifo
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module uart_rx_fifo_tb;

// Inputs
reg clk;
reg reset_pin;
reg rx;
reg s_tick;
reg rd_rx_pin;

// Outputs
wire [7:0] r_data;
wire rx_empty;
wire rx_full;
wire rx_busy;

// Instantiate the Unit Under Test (UUT)
uart_rx_fifo uut (
	.clk(clk), 
	.reset_pin(reset_pin), 
	.rx(rx),  
	.rd_rx_pin(rd_rx_pin), 
	.r_data(r_data), 
	.rx_empty(rx_empty), 
	.rx_full(rx_full),
	.rx_busy(rx_busy)
);

initial
$monitor ($time, " memory = %h rx_full = %b rd_rx_pin = %b r_data = %h empty = %b", 
uut.fifo_unit.array_reg, uut.rx_full, uut.rd_rx_pin, uut.r_data, uut.rx_empty);

initial begin
	// Initialize Inputs
	clk = 0;
	reset_pin = 1;
	rx = 1'b1;
	rd_rx_pin = 1;
	#3 reset_pin = 1'b0;
	#2 reset_pin = 1'b1;
end

always
#1 clk = ~clk;

initial
begin
#100 rx = 1'b0;
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

#900 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b1;
#864 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b1;

#900 rx = 1'b0;
#864 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b1;
#864 rx = 1'b1;
#864 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b1;

#1000 rd_rx_pin = 0;
#2 rd_rx_pin = 1;

#900 rx = 1'b0;
#864 rx = 1'b0;
#864 rx = 1'b1;
#864 rx = 1'b1;
#864 rx = 1'b1;
#864 rx = 1'b0;
#864 rx = 1'b0;
#864 rx = 1'b0;
#864 rx = 1'b0;
#864 rx = 1'b1;

#900 rd_rx_pin = 0;
#2 rd_rx_pin = 1;
#2 rd_rx_pin = 0;
#2 rd_rx_pin = 1;
end

initial
#60000 $finish	;
	
endmodule
