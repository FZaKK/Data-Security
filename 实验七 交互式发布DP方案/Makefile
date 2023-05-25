CC = gcc
TARGET = test
OBJS = csvpackage.o laplace.o test.o
INCLUDE = -I./include
all:test testhist
$(TARGET):csvpackage.o laplace.o test.o
	$(CC) csvpackage.o laplace.o test.o -o $(TARGET)  -lm
testhist:csvpackage.o laplace.o testhist.o 
	$(CC) csvpackage.o laplace.o testhist.o -o testhist -lm
csvpackage.o:csvpackage.c
	$(CC) $(INCLUDE) -c csvpackage.c 
laplace.o:laplace.c
	$(CC) $(INCLUDE) -c laplace.c -lm
test.o:test.c
	$(CC) $(INCLUDE) -c test.c 
testhist.o:testhist.c
	$(CC) $(INCLUDE) -c testhist.c


 .PHONY:clean
 clean:
	 rm *.o test testhist

