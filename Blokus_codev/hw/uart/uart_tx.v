//Listing 8.3
module uart_tx
   #(
     parameter D_BIT = 8,     // # data bits
               SB_TICK = 16  // # ticks for stop bits
   )
   (
    input wire clk, reset, tx_start, s_tick,
    input wire [7:0] tx_din,
    output wire tx_fifo_rd, tx_busy, tx
   );

   // symbolic state declaration
   localparam [1:0]
      idle  = 2'b00,
      start = 2'b01,
      data  = 2'b10,
      stop  = 2'b11;

   // signal declaration
	reg [1:0] state_reg, state_next;
	reg [3:0] s_reg, s_next;
	reg [2:0] n_reg, n_next;
	reg [7:0] b_reg, b_next;
	reg tx_reg, tx_next, tx_fifo_rd_reg, tx_busy_reg, tx_busy_next;
	
	initial
	begin
		state_reg = idle; state_next = idle;
		s_reg = 0; s_next = 0;
		n_reg = 0; n_next = 0;
		b_reg = 0; b_next = 0;
		tx_reg = 1'b1; tx_next = 1'b1;
		tx_fifo_rd_reg = 0;
		tx_busy_reg = 0; tx_busy_next = 0;
	end
	
   // body
   // FSMD state & data registers
   always @(posedge clk, posedge reset)
      if (reset)
         begin
				state_reg <= idle; state_next <= idle;
				s_reg <= 0; s_next <= 0;
				n_reg <= 0; n_next <= 0;
				b_reg <= 0; b_next <= 0;
				tx_reg <= 1'b1; tx_next <= 1'b1;
				tx_fifo_rd_reg <= 0;
				tx_busy_reg <= 0; tx_busy_next <= 0;
         end
      else if(s_tick)
			begin
				case (state_reg)
				idle:
					begin
						tx_next <= 1'b1;
						if (tx_start)
							begin
								state_next <= start;
								tx_fifo_rd_reg <= 1'b1;
								b_next <= tx_din;
							end
					end
				start:
					begin
						tx_next <= 1'b0;
						tx_busy_next <= 1'b1;
							if (s_reg==15)
								begin
									state_next <= data;
									s_next <= 0;
								end
							else
								s_next <= s_reg + 1;
					end
				data:
					begin
						tx_next <= b_reg[0];
							if (s_reg==15)
								begin
									s_next <= 0;
									b_next <= b_reg >> 1;
									if (n_reg==(D_BIT-1))
										state_next <= stop ;
									else
										n_next <= n_reg + 1;
								end
							else
								s_next <= s_reg + 1;
					end
				stop:
					begin
						tx_next <= 1'b1;
							if (s_reg==(SB_TICK-1))
								begin
									state_next <= idle;
									s_next <= 0;
									n_next <= 0;
									b_next <= 0;
									tx_busy_next <= 0;
								end
							else
								s_next <= s_reg + 1;
					end
				endcase
			end
		else
         begin
            state_reg <= state_next;
            s_reg <= s_next;
            n_reg <= n_next;
            b_reg <= b_next;
            tx_reg <= tx_next;
				tx_busy_reg <= tx_busy_next;
				tx_fifo_rd_reg <= 0;
         end
	assign tx = tx_reg;
	assign tx_fifo_rd = tx_fifo_rd_reg;
	assign tx_busy = tx_busy_reg;
endmodule
