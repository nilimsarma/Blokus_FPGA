module blokus_uart
(
input wire reset_pin, clk, rx,
output tx, rx_busy, tx_busy
);

wire reset;
wire producer_output_stream_en, producer_output_stream_rdy, consumer_input_stream_en, consumer_input_stream_rdy;
wire fifo_producer_empty, fifo_consumer_full, fifo_producer_rd, fifo_consumer_wr;
wire [7:0] data_consumer, data_producer;
wire producer_output_stream_eos;

uart_tx_fifo uart_tx_fifo_unit   (.clk(clk), .reset(reset), .wr_tx(fifo_consumer_wr), 
									.w_data(data_consumer), .tx_full(fifo_consumer_full), .tx(tx), .tx_busy(tx_busy));   

uart_rx_fifo uart_rx_fifo_unit	(.clk(clk), .reset(reset), .rx(rx), .rd_rx(fifo_producer_rd), 
								.r_data(data_producer), .rx_empty(fifo_producer_empty), .rx_full(), .rx_busy(rx_busy));
													
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
assign reset = ~reset_pin;

endmodule
