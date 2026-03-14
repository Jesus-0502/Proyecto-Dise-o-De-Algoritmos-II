CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = ma2_pfsp
SOURCES = main_ma.cpp MA2.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
