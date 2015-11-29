In this project, a minimax based artificial intelligence algorithm for Blokus Duo is implemented on an FPGA. The design adheres to the rules for ICFPT 2013 Design Competition. The design has been tested on an Altera DE2-115 board with Cyclone IV EP4CE115F29C7 FPGA device running at 50Mhz. The programming tools include ImpulseC and Verilog. Quartus II software is used to generate the necessary bitstream for the Altera FPGA.
The design includes a UART to communicate with a host system which monitors the game. The UART is designed in verilog. The AI is written in ImpulseC, which interfaces with the UART using the ImpulseC streaming interface.
The AI designed in ImpulseC involves three layers of processes. The main process communicates with the UART and feeds data to four Level-1 processes. Each of these Level-1 processes in turn feed data to five Level-2 processes. The number of processes at each level is chosen so to balance the load and ensure that all the processes are optimally utilized.
The basic AI algorithm for the implementation has been taken from the metablok project. https://code.google.com/p/metablok/wiki/MetaBlok

Folder structure of the attachment:
Blokus_codev/blokus_hw.c: ImpulseC hardware file.
Blokus_codev/blokus_sw.c: ImpulseC sw testbench.
Blokus_codev/hw/Blokus.qpf: Quartus II project file.
Blokus_codev/hw/uart: folder containing verilog files for UART.
Blokus_codev/hw/blokus_uart: top level module for Quartus II project.

http://www.eetimes.com/document.asp?doc_id=1320753