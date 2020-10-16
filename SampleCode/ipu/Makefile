
CC=arm-linux-gnueabihf-gcc
LINK=arm-linux-gnueabihf-gcc



OBJ-camera = v4l2_camera.o lcd.o


TARGET-camera = v4l2_camera


all: $(TARGET-camera)

$(TARGET-camera): $(OBJ-camera)
ifneq ($(wildcard lib/libtqipu.so),)
	$(CC) -o $(TARGET-camera) $(OBJ-camera) ${LIBS} -lrt -lpthread -L ./lib -ltqipu
else
	$(CC) -fPIC -shared -o lib/libtqipu.so lib/tq_ipu.c
	$(CC) -o $(TARGET-camera) $(OBJ-camera) ${LIBS} -lrt -lpthread -L ./lib -ltqipu
endif

%.o: %.c
	$(CC) $(INC) -Wall -O2 -c $^ -o $@

.PHONY: clean
clean:
	rm -f $(TARGET-camera) $(OBJ-camera)
