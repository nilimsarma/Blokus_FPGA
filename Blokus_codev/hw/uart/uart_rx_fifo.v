`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    19:53:45 08/24/2013 
// Design Name: 
// Module Name:    uart_rx_fifo 
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
module uart_rx_fifo
#(
//115200 baud, 8 data bits, 1 stop bit, 2^2 FIFO
parameter D_BIT = 8, //#data bits
			SB_TICK = 16,	//#ticks for stop bits
								//16/24/32 for 1/1.5/2 bits
			DVSR = 27,  //baud rate divisor
			DVSR_BIT = 5, //bits of DVSR
			FIFO_W = 2	//#address bits of FIFO
							//#words in FIFO = 2^FIFO_W
)
(
input clk, reset, rx, rd_rx,
output wire [7:0] r_data,
output rx_empty, rx_full, rx_busy
 );

wire s_tick, rx_fifo_wr;
wire [7:0] rx_dout;

mod_m_counter s_tick_clk(.clk(clk), .reset(reset), .max_tick(s_tick), .q());

uart_rx uart_rx_unit(.clk(clk), .reset(reset), .rx(rx), .s_tick(s_tick), 
.rx_fifo_wr(rx_fifo_wr), .rx_busy(rx_busy), .rx_dout(rx_dout));

fifo_uart fifo_unit (.clk(clk), .reset(reset), .rd(rd_rx), .wr(rx_fifo_wr), .w_data(rx_dout), 
	.empty(rx_empty), .full(rx_full), .r_data(r_data));

endmodule

