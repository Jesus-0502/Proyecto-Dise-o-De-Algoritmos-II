CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = pr_rg_pfsp
SOURCES = main_pr_rg.cpp pr_rg.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
