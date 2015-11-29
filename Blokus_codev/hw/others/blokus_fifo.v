module blokus_fifo
(
input wire reset, clk, wr,
input wire [7:0] data_in,
output [7:0] data_out
);

wire producer_output_stream_en, producer_output_stream_rdy, consumer_input_stream_en, consumer_input_stream_rdy;
wire fifo_producer_empty, fifo_consumer_full, fifo_producer_rd, fifo_consumer_wr;
wire [7:0] data_consumer, data_producer;
wire producer_output_stream_eos;

wire rd_consumer, consumer_empty;

fifo fifo_producer(.clk(clk), .reset(reset), .rd(fifo_producer_rd), .wr(wr), .w_data(data_in), 
					.empty(fifo_producer_empty), .full(), .r_data(data_producer));
fifo fifo_consumer(.clk(clk), .reset(reset), .rd(rd_consumer), .wr(fifo_consumer_wr), .w_data(data_consumer), 
					.empty(consumer_empty), .full(fifo_consumer_full), .r_data(data_out));
Blokus Blokus_unit
(
  .reset(reset),
  .sclk(),
  .clk(clk),
  .p_producer_output_stream_en(producer_output_stream_en),
  .p_producer_output_stream_eos(producer_output_stream_eos),
  .p_producer_output_stream_data(data_producer),
  .p_producer_output_stream_rdy (producer_output_stream_rdy),
  .p_consumer_input_stream_en (consumer_input_stream_en),
  .p_consumer_input_stream_data (data_consumer),
  .p_consumer_input_stream_eos (),
  .p_consumer_input_stream_rdy (consumer_input_stream_rdy)
);

assign consumer_input_stream_en = ~fifo_consumer_full;
assign producer_output_stream_en = ~fifo_producer_empty;
assign producer_output_stream_eos = 1'b0;
assign fifo_producer_rd = producer_output_stream_rdy & (~fifo_producer_empty);
assign fifo_consumer_wr = consumer_input_stream_rdy & (~fifo_consumer_full);
assign rd_consumer = ~consumer_empty;
endmodule

