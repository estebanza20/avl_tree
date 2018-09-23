CXX = g++
INC = -I./ -I./include/

CPPFLAGS += -isystem $(GTEST_DIR)/include $(INC)
CXXFLAGS += -g -std=c++14 -Wall -Wextra -pthread

SRCDIR := src
TESTDIR := test
BUILDDIR := build

GTEST_DIR = $(TESTDIR)/googletest
GTEST_BUILD = $(BUILDDIR)/$(GTEST_DIR)

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)


# Main application
MAIN = main
MAINSRC = $(TESTDIR)/$(MAIN).cpp

# Unit test
UNIT = unittest
UNITSRC = $(TESTDIR)/$(UNIT).cpp

SRC := $(wildcard $(SRCDIR)/**/*.cpp)
OBJ = $(addprefix $(BUILDDIR)/,$(SRC:.cpp=.o))

RM = rm -rf

.PHONY: all run run_unittest clean

all: $(MAIN) $(UNIT)

$(MAIN): $(BUILDDIR)/$(MAIN)
$(UNIT): $(BUILDDIR)/$(UNIT)

run: $(MAIN)
	./$(BUILDDIR)/$(MAIN)

run_unittest: $(UNIT)
	./$(BUILDDIR)/$(UNIT)

$(BUILDDIR)/$(MAIN): $(MAINSRC) $(OBJ)
	mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $^ -o $@

$(BUILDDIR)/$(UNIT): $(UNITSRC) $(OBJ) $(GTEST_BUILD)/gtest_main.a
	mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -lpthread $^ -o $@

$(BUILDDIR)/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $^ -o $@

$(GTEST_BUILD)/gtest-all.o : $(GTEST_SRCS_)
	mkdir -p $(GTEST_BUILD)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc -o $@

$(GTEST_BUILD)/gtest_main.o : $(GTEST_SRCS_)
	mkdir -p $(GTEST_BUILD)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc -o $@

$(GTEST_BUILD)/gtest_main.a : $(GTEST_BUILD)/gtest-all.o $(GTEST_BUILD)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

clean:
		$(RM) $(BUILDDIR)
