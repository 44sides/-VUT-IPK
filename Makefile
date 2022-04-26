CC = g++
CFLAGS  = -Wall -pedantic -Wextra
TARGET = hinfoscv
 
all: $(TARGET)
 
$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) $(TARGET).cpp -o $(TARGET)
