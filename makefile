CXX      = g++
CXXFLAGS = -std=c++17 -Wall -I.
LIBS     = -lcurl -lncurses

TARGET = espn

$(TARGET): main.cpp espn-api.h ui.h
	$(CXX) $(CXXFLAGS) main.cpp $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)