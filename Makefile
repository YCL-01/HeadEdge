CC = gcc
CXXFLAGS = -Wall -Wfatal-errors -Wno-unused-result -Wno-unknown-pragmas -fPIC
LDFLAGS = -pthread

SRC_DIR = ./src
OBJ_DIR = ./obj
TARGET = headedge

SRCS = $(notdir $(wildcard $(SRC_DIR)/*.c))
OBJS = $(SRCS:.c=.o)
DEPS = $(SRCS:.o=.d)

OBJECTS = $(patsubst %.o,$(OBJ_DIR)/%.o,$(OBJS))
DEPS = $(OBJECTS:.o=.d)

all: headedge

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CXXFLAGS) -c $< -o $@ -MD $(LDFLAGS)

$(TARGET) : $(OBJECTS)
	$(CC) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

.PHONY: clean all
clean:
	rm -f $(OBJECTS) $(DEPS) $(TARGET)

-include $(DEPS)

 
