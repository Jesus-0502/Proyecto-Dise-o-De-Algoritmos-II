CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = aco_pfsp
SOURCES = main_aco.cpp ACO.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
