SOURCE	:= Main.c Parser.c Registers.c Core.c IFe.c ID.c EX.c MEM.c WB.c
CC	:= gcc
TARGET	:= RVSim

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) -std=c99 -g  -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)
