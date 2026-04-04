CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = abc_pfsp
SOURCES = main_abc.cpp ABC.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
