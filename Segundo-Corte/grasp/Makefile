CXX = g++
CXXFLAGS = -O3

TARGET = grasp_pfsp
SOURCES = main.cpp grasp.cpp LS.cpp NEH.cpp
SOURCES2 = main2.cpp grasp.cpp LS.cpp NEH.cpp
SOURCES3 = main3.cpp grasp.cpp LS.cpp NEH.cpp
SOURCES4 = main4.cpp grasp.cpp LS.cpp NEH.cpp
SOURCES5 = main5.cpp grasp.cpp LS.cpp NEH.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

$(TARGET)2: $(SOURCES2)
	$(CXX) $(CXXFLAGS) -o $(TARGET)2 $(SOURCES2)

$(TARGET)3: $(SOURCES3)
	$(CXX) $(CXXFLAGS) -o $(TARGET)3 $(SOURCES3)

$(TARGET)4: $(SOURCES4)
	$(CXX) $(CXXFLAGS) -o $(TARGET)4 $(SOURCES4)

$(TARGET)5: $(SOURCES5)
	$(CXX) $(CXXFLAGS) -o $(TARGET)5 $(SOURCES5)

clean:
	rm -f $(TARGET)*