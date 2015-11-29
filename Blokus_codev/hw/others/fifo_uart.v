// Listing 4.20
module fifo
   #(
    parameter B=8, // number of bits in a word
              W=2  // number of address bits
   )
   (
    input wire clk, reset,
    input wire rd, wr,
    input wire [B-1:0] w_data,
    output wire empty, full,
    output wire [B-1:0] r_data
   );

   //signal declaration
   reg [B-1:0] array_reg [2**W-1:0];  // register array
   reg [W-1:0] w_ptr_reg, w_ptr_succ, r_ptr_reg, r_ptr_succ;
   reg full_reg, empty_reg;
	wire wr_en;

   assign r_data = array_reg[r_ptr_reg];	
   assign wr_en = wr & ~full_reg;

   // register for read and write pointers
   always @(posedge clk, posedge reset)
      if (reset)
         begin
            w_ptr_reg <= 0;
            r_ptr_reg <= 0;
				w_ptr_succ <= 0;
				r_ptr_succ <= 0;
            full_reg <= 1'b0;
            empty_reg <= 1'b1;
         end
      else
         begin
				// register file write operation
				if (wr_en)  array_reg[w_ptr_reg] <= w_data;
	
				w_ptr_succ <= w_ptr_reg + 1;
				r_ptr_succ <= r_ptr_reg + 1;
				
				case ({wr, rd})
				
					// 2'b00:  no op
					2'b01: // read
						if (~empty_reg) // not empty
							begin
								r_ptr_reg <= r_ptr_succ;
								full_reg <= 1'b0;
								if (r_ptr_succ==w_ptr_reg)
									empty_reg <= 1'b1;
							end
					2'b10: // write
						if (~full_reg) // not full
							begin
								w_ptr_reg <= w_ptr_succ;
								empty_reg <= 1'b0;
								if (w_ptr_succ==r_ptr_reg)
									full_reg <= 1'b1;
							end
					2'b11: // write and read
						begin
							w_ptr_reg <= w_ptr_succ;
							r_ptr_reg <= r_ptr_succ;
						end
				endcase
				/*
				if(rd & (~empty_reg))
					begin
						r_ptr_reg <= r_ptr_succ;
						full_reg <= 1'b0;
						if (r_ptr_succ==w_ptr_reg)
							empty_reg <= 1'b1;
						end
				else if(wr & (~full_reg))
					begin
						w_ptr_reg <= w_ptr_succ;
						empty_reg <= 1'b0;
						if (w_ptr_succ==r_ptr_reg)
							full_reg <= 1'b1;
					end
				*/
         end
	assign full = full_reg;
   assign empty = empty_reg;
endmodule
