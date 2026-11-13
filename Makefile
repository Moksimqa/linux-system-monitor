CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -I include

TARGET = monitor

SRCS = src/main.cpp src/CpuCollector.cpp src/MemCollector.cpp src/NetCollector.cpp src/ConsoleRenderer.cpp src/ProcessCollector.cpp

$(TARGET) : $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)


clean:
	rm -f $(TARGET)