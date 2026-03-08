CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = ma_pfsp
SOURCES = main_ma.cpp MA.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
