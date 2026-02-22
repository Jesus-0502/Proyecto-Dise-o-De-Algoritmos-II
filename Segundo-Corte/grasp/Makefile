# CXX = g++
# CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

# TARGET = grasp_pfsp
# SOURCES = main.cpp grasp.cpp LS.cpp NEH.cpp
# SOURCES2 = main2.cpp grasp.cpp LS.cpp NEH.cpp
# SOURCES3 = main3.cpp grasp.cpp LS.cpp NEH.cpp
# SOURCES4 = main4.cpp grasp.cpp LS.cpp NEH.cpp
# SOURCES5 = main5.cpp grasp.cpp LS.cpp NEH.cpp

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

TARGET = grasp_pfsp
SOURCES = main_grasp.cpp grasp.cpp LS.cpp NEH.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)
