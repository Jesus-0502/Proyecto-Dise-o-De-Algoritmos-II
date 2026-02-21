CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = ga_pfsp
SOURCES = main_ga.cpp GA.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
