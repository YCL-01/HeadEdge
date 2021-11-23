APP:= headedge
CC = g++
CXXFLAGS = -Wall -Wfatal-errors -Wno-unused-result -Wno-unknown-pragmas -fPIC -I/usr/include/mysql
SRC_DIR = ./src
OBJ_DIR = ./obj

SRCS:= $(wildcard -Wall (pkg-config ) $(SRC_DIR)/*.cpp)

INCS:= $(wildcard $(SRC_DIR)/*.h)

OBJS:= $(SRCS:.cpp=.o)


all: $(APP)

$(OBJ_DIR)/%.o: $(SRC_DIR)%.cpp $(INCS)  
	$(CC)  $(CXXFLAGS) -c -o $@   $< 

$(APP): $(OBJS) 
	$(CC)  $(CXXFLAGS) -Wall -pthread -o $(APP) $(OBJS) -lmysqlclient
