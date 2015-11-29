module uart
(
input wire reset_pin, clk, rx,
output tx, rx_busy, tx_busy
);

wire [7:0] data;
wire rx_empty;

uart_tx_fifo uart_tx_fifo_unit
(.clk(clk), .reset_pin(reset_pin), .wr_tx_pin(rx_empty), .w_data(data),
.tx_full(), .tx(tx), .tx_busy(tx_busy));

uart_rx_fifo uart_rx_fifo_unit
(.clk(clk), .reset_pin(reset_pin), .rx(rx), .rd_rx_pin(rx_empty), 
.r_data(data) ,.rx_empty(rx_empty) ,.rx_full(), .rx_busy(rx_busy));

endmodule
