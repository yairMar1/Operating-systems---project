CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

SRCS = main.cpp MSTServer.cpp Graph.cpp StrategyFactory.cpp GraphVisualizer.cpp ThreadPoll.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = graph_program

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

# To run the program with a specific number of threads, use the following command:
# make run NUM_THREADS=4
run: $(EXEC)
	@echo "Running with $(NUM_THREADS) threads"
	./$(EXEC) $(NUM_THREADS)