ARMCC = arm-linux-gnueabihf-gcc
CC = gcc
CFLAGS = -Wall -Wextra
TARGET = hello

all: $(TARGET) uuencode uudecode

$(TARGET): group_19.c
	$(ARMCC) $(CFLAGS) $? -o $@

uuencode: uuencode.c
	$(CC) $(CFLAGS) $? -o $@

uudecode: uudecode.c
	$(ARMCC) $(CFLAGS) $? -o $@


test: all
	./uuencode $(TARGET) $(TARGET)1 > $(TARGET)1.uue
#	./uudecode $(TARGET)1.uue
#	./$(TARGET)1


clean:
	rm -f $(TARGET) uuencode uudecode $(TARGET)1.uue $(TARGET)1
