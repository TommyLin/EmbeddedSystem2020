ARMCC = arm-linux-gnueabihf-gcc
CC = gcc
CFLAGS = -Wall -Wextra
TARGET = hello

all: $(TARGET) uuencode.x86 uudecode

$(TARGET): group_19.c
	$(ARMCC) $(CFLAGS) $? -o $@

uuencode.x86: uuencode.c
	$(CC) $(CFLAGS) $? -o $@

uudecode: uudecode.c
	$(ARMCC) $(CFLAGS) $? -o $@


test: all
	./uuencode.x86 $(TARGET) $(TARGET)1 > $(TARGET)1.uue
#	./uudecode $(TARGET)1.uue
#	./$(TARGET)1


clean:
	rm -f $(TARGET) uuencode.x86 uudecode $(TARGET)1.uue $(TARGET)1
