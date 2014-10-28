SOURCES=./src/main.cpp ./src/sudoku_solver.cpp ./src/exact_cover.cpp ./src/constraint_propagation.cpp
Target=SudokuSolver

OBJS=$(SOURCES:.cpp=.o)

CPPFLAGS = -I. 
CXXFLAGS = -std=c++11 -O3 -Wall -ffast-math

all: all_linux


%.o: %.cpp
	@echo "Compiling" $@
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

all_linux: $(OBJS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(OBJS) -o $(Target)

clean: 
	@$(RM) -rf $(OBJS)
	@$(RM) $(Target)

.PHONY: all_linux clean
