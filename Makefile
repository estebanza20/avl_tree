CXX = g++
INC = -I./ -I./include/

CPPFLAGS := $(INC) -std=c++14 -Wall -g

SRCDIR := src
TESTDIR := test
BUILDDIR := build

APP = main
APPSRC = $(TESTDIR)/$(APP).cpp

UNIT = unittest
UNITSRC = $(TESTDIR)/$(UNIT).cpp

SRC := $(wildcard $(SRCDIR)/**/*.cpp)
OBJ = $(addprefix $(BUILDDIR)/,$(SRC:.cpp=.o))

RM = rm -rf

.PHONY: all run run_unittest clean

all: $(APP)

$(APP): $(APPSRC) $(SRC) $(OBJ)
	mkdir -p $(BUILDDIR)
	$(CXX) $(CPPFLAGS) $(APPSRC) $(OBJ) -o $(BUILDDIR)/$@

$(UNIT): $(UNITSRC) $(SRC) $(OBJ)
	mkdir -p $(BUILDDIR)
	$(CXX) $(CPPFLAGS) $(UNITSRC) $(OBJ) -o $(BUILDDIR)/$@

$(BUILDDIR)/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) -c $^ -o $@

run: $(APP)
	./$(BUILDDIR)/$(APP)

run_unittest: $(UNIT)
	./$(BUILDDIR)/$(UNIT)

clean:
		$(RM) $(BUILDDIR)
