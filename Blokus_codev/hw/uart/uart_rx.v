//Listing 8.1
module uart_rx
   #(
     parameter D_BIT = 8,     // # data bits
               SB_TICK = 16  // # ticks for stop bits
   )
   (
    input wire clk, reset, rx, s_tick,
    output wire rx_fifo_wr, rx_busy,
    output wire [7:0] rx_dout
   );

   // symbolic state declaration
   localparam [1:0]
      idle  = 2'b00,
      start = 2'b01,
      data  = 2'b10,
      stop  = 2'b11;

	reg [1:0] state_reg, state_next;
	reg [3:0] s_reg, s_next;
	reg [2:0] n_reg, n_next;
	reg [7:0] b_reg, b_next, b_reg_done;
	reg rx_busy_reg, rx_busy_next, rx_fifo_wr_reg;
	
	initial
		begin
			state_reg = idle;	state_next = idle;
			s_reg = 0;	s_next = 0;
			n_reg = 0;	n_next = 0;
			b_reg = 0;	b_next = 0;	b_reg_done = 0;
			rx_busy_reg = 0; rx_busy_next = 0; rx_fifo_wr_reg = 0;
	end
	
   // body
   // FSMD state & data registers
   always @(posedge clk, posedge reset)
      if (reset)
         begin
				state_reg <= idle;	state_next <= idle;
				s_reg <= 0;	s_next <= 0;
				n_reg <= 0;	n_next <= 0;
				b_reg <= 0;	b_next <= 0;	b_reg_done <= 0;
				rx_busy_reg <= 0; rx_busy_next <= 0; rx_fifo_wr_reg <= 0;
         end
		else if (s_tick)
		  begin
			case (state_reg)
				start:
					if(rx)
						begin
							state_next <= idle;
							s_next <= 0;
							$display ("error1");
						end
					else
						begin
							if (s_reg==7)
								begin
									state_next <= data;
									s_next <= 0;
									rx_busy_next <= 1'b1;
								end
							else
								s_next <= s_reg + 1;
						end
					
				data:
					if (s_reg==15)
						begin
							s_next <= 0;
							b_next <= {rx, b_reg[7:1]};
							if (n_reg==(D_BIT-1))
								state_next <= stop ;
							 else
								n_next <= n_reg + 1;
						 end
					else
						s_next <= s_reg + 1;
				stop:
					if (s_reg==(SB_TICK-1))
						begin
							if(rx)
								begin
									b_reg_done <= b_reg;
									rx_fifo_wr_reg <= 1'b1;
								end
							else
								$display ("error2");
							state_next <= idle;
							s_next <= 0;
							n_next <= 0;
							b_next <= 0;
							rx_busy_next <= 0;
						end
					else
						s_next <= s_reg + 1;
				endcase
			end
      else
         begin
				if(state_reg == idle)
					begin
						if (~rx) 
							state_reg <= start;
							state_next <= start;
						end
				else
					state_reg <= state_next;
						
			s_reg <= s_next;
			n_reg <= n_next;
			b_reg <= b_next;
			rx_busy_reg <= rx_busy_next;
			rx_fifo_wr_reg <= 0;
				
       end
   // output
   assign rx_dout = b_reg_done;
	assign rx_busy = rx_busy_reg;
	assign rx_fifo_wr = rx_fifo_wr_reg;
endmodule
