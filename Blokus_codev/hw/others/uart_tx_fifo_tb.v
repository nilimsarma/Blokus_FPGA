`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    20:23:59 08/22/2013 
// Design Name: 
// Module Name:    uart_tb 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module uart_tx_fifo_tb;

reg clk, reset_pin, wr_tx_pin;
reg[7:0] w_data;
wire tx_full, tx, tx_busy;

uart_tx_fifo uart_tx_fifo_uut
(.clk(clk), .reset_pin(reset_pin), .wr_tx_pin(wr_tx_pin), .w_data(w_data), .tx_full(tx_full), 
.tx(tx), .tx_busy(tx_busy));

initial
$monitor($time, " tx = %b", tx);

initial
begin
	clk = 1'b0;
	wr_tx_pin = 1'b1;
	reset_pin = 1'b1;
	//w_data = 8'h41; //'A'	8'b01000001
	w_data = 8'b00001000;
	#3 reset_pin = 1'b0;
	#2 reset_pin = 1'b1;
	#2 wr_tx_pin = 1'b0;
	#2 wr_tx_pin = 1'b1;
	#10000;
	w_data = 8'b00101000;
	#2 wr_tx_pin = 1'b0;
	#2 wr_tx_pin = 1'b1;
	
end

always
	#1 clk = ~clk;
	
initial
	#50000 $finish;
	
endmodule
