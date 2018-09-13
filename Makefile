CXX = g++
CPPFLAGS := -I./ -I./include/ -std=c++14 -Wall -g

APP = main
APP_SRC = src/$(APP).cpp

TEST = unittest
TEST_SRC = test/$(TEST).cpp

SRC := $(wildcard src/data_structures/*.cpp)
OBJ := $(SRC:.cpp=.o)

RM = rm -f

.PHONY: all run run_unittest clean

all: $(APP)

$(APP): $(OBJ) $(APP_SRC) $(SRC)
	$(CXX) $(CPPFLAGS) $(APP_SRC) $(OBJ) -o $@

$(TEST): $(OBJ) $(TEST_SRC) $(SRC)
	$(CXX) $(CPPFLAGS) $(TEST_SRC) $(OBJ) -o $@

run: $(APP)
	./$(APP)

run_unittest: $(TEST)
	./$(TEST)

clean:
		$(RM) $(APP) $(TEST) $(OBJ)
