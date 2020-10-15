#arm-linux-gnueabihf-gcc -std=99
CC=arm-linux-gnueabihf-gcc
LINK=arm-linux-gnueabihf-gcc

 
OBJ-dec = decode_test.o \
	  ipu/lcd.o \
	  ipu/ipu_test.o
	  
OBJ-enc = encode_test.o \
	  ipu/lcd.o \
	  ipu/ipu_test.o
      
TARGET-dec = decode_test.out
TARGET-enc = encode_test.out

all: $(TARGET-dec) \
	$(TARGET-enc)



$(TARGET-dec): $(OBJ-dec)
	$(CC) -o $(TARGET-dec) $(OBJ-dec) ${LIBS} -lrt -lpthread -L ./lib -lvpu -lipu -ltqipu -ltqvpu	
	
$(TARGET-enc): $(OBJ-enc)
	$(CC) -o $(TARGET-enc) $(OBJ-enc) ${LIBS} -lrt -lpthread -L ./lib -lvpu -lipu -ltqipu -ltqvpu	


%.o: %.c
	$(CC) $(INC) -Wall -O2 -c $^ -o $@

.PHONY: clean
clean:
	rm -f $(OBJDIR)/$(TARGET-dec) $(OBJ-dec) \
	$(TARGET-enc) $(OBJ-enc)\


	
	


