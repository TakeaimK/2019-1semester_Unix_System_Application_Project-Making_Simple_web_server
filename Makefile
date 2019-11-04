CC = gcc
BIN = simple_webserver
TARGET =main
RM = rm -rf
LDFLAGS = -lpthread

SRCS = $(TARGET).c 
OBJS = $(TARGET).o 

$(TARGET) : $(OBJS)
	$(CC) -o $(BIN) $(OBJS) $(LDFLAGS)
					                   
clean:
	$(RM) $(OBJS) $(BIN) core
						       
run:
	@./$(TARGET)
