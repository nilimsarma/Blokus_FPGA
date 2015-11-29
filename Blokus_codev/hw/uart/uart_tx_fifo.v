//Listing 8.4
module uart_tx_fifo
   #( // Default setting:
      // 19,200 baud, 8 data bits, 1 stop bit, 2^2 FIFO
      parameter D_BIT = 8,     // # data bits
                SB_TICK = 16, // # ticks for stop bits, 16/24/32
                              // for 1/1.5/2 stop bits
                DVSR = 27,   // baud rate divisor
                              // DVSR = 50M/(16*baud rate)
                DVSR_BIT = 5, // # bits of DVSR
                FIFO_W = 2    // # addr bits of FIFO
                              // # words in FIFO=2^FIFO_W
   )
   (
    input wire clk, reset, wr_tx,
    input wire [7:0] w_data,
    output wire tx_full, tx, tx_busy
   );

   // signal declaration

	wire s_tick, tx_rd, tx_fifo_empty, tx_fifo_not_empty, tx_fifo_rd;
	wire [7:0] tx_fifo_out;
	
   //body
   mod_m_counter baud_gen_unit(.clk(clk), .reset(reset), .q(), .max_tick(s_tick));

   fifo_uart fifo_tx_unit(.clk(clk), .reset(reset), .rd(tx_fifo_rd), .wr(wr_tx), .w_data(w_data), .empty(tx_fifo_empty),
       .full(tx_full), .r_data(tx_fifo_out));

   uart_tx uart_tx_unit(.clk(clk), .reset(reset), .tx_start(tx_fifo_not_empty),.s_tick(s_tick),
		.tx_din(tx_fifo_out), .tx_fifo_rd(tx_fifo_rd), .tx_busy(tx_busy), .tx(tx));

   assign tx_fifo_not_empty = ~tx_fifo_empty;
endmodule
