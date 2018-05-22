CC = g++
BIN = solarSystem
OBJS = $(BUILD)/$(BIN).o $(BUILD)/openGLUtils.o $(BUILD)/geometry.o
CFLAGS = -std=c++11
INC = -I/opt/local/include -I./include/
LIB = -L/opt/local/lib
LIBS = -lglfw -lGLEW -framework OpenGL

BUILD = ./src/obj
SRC = ./src

$(BIN): $(OBJS)
	$(CC) -o $(BIN) $(OBJS) $(INC) $(LIB) $(LIBS)

$(BUILD)/%.o: $(SRC)/%.cpp 
	$(CC) -c -o $@ $< $(INC) $(CFLAGS)

clean: 
	rm $(BUILD)/*.o
	rm $(BIN)