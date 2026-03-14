CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = ss_pfsp
SOURCES = main_ss.cpp SS.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
