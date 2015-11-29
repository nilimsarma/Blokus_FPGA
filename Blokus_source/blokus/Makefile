all:	blokus-host blokus-httpd

#CFLAGS	= -Wall -g
CC = gcc

blokus-host: blokus-host.c blokus-minimax.c
	$(CC) $(CFLAGS) blokus-host.c blokus-minimax.c -o blokus-host -lpthread

blokus-httpd: blokus-httpd.c
	$(CC) $(CFLAGS) blokus-httpd.c -o blokus-httpd

ann_test: ann_data_test.c
	$(CC) $(CFLAGS) -o ann_data_test ann_data_test.c

neuralnet: blokus-neuralnet.c
	$(CC) $(CFLAGS) -o neuralnet blokus-neuralnet.c -lm

piece_reorder: piece_reorder.c
	$(CC) $(FLAGS) -o piece_reorder piece_reorder.c
clean:
	-rm -rf *.o *~ .*~ core blokus-host blokus-httpd ann_data_test neuralnet piece_reorder *zip

archive:
	-zip blokus-host-`date "+%g%m%d"`.zip Makefile setup_com12.bat *.h *.c blokus.html
