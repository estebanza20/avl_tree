CXX = g++
CPPFLAGS := -I./ -I./include/ -std=c++14 -Wall

APP = main
APP_SRC = src/$(APP).cpp

TEST = unittest
TEST_SRC = test/$(TEST).cpp

SRC := $(wildcard src/data_structures/*.cpp)
OBJ := $(SRC:.cpp=.o)

RM = rm -f

.PHONY: all run run_unittest clean

all: $(APP)

$(APP): $(OBJ)
	$(CXX) $(CPPFLAGS) $(APP_SRC) $^ -o $@

$(TEST): $(OBJ)
	$(CXX) $(CPPFLAGS) $(TEST_SRC) $^ -o $@

run: $(APP)
	./$(APP)

run_unittest: $(TEST)
	./$(TEST)

clean:
		$(RM) $(APP) $(TEST) $(OBJ)
