.PHONY : clean

CXX= clang++
CPPFLAGS= -fPIC -g -DDEBUG -Wall
LDFLAGS= -shared

SOURCES = $(shell echo *.cpp)
HEADERS = $(shell echo *.h)
OBJECTS=$(SOURCES:.cpp=.o)

TARGET= libh264dec.so

all: $(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)
