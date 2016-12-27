CC=gcc
#CC=powerpc-linux-gnu-gcc


all:
	$(CC) -o aclk_dpi_decap.o -c aclk_dpi_decap.c
	$(CC) -o psend.o -c psend.c
	$(CC) -o psend psend.o aclk_dpi_decap.o

clean:
	rm psend.o aclk_dpi_decap.o psend
