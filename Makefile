TARGET = $(shell basename $(CURDIR))

CC = g++
CFLAGS = -std=c++11 -pedantic-errors -Wall -Wextra -g -pthread
RCFLAGS = -std=c++11 -pedantic-errors -Wall -Wextra -DNDEBUG -O3 -pthread
LDFLAGS = -L$(LIBS_DEBUG) -Wl,-rpath=$(LIBS_DEBUG)
RLDFLAGS = -L$(LIBS_RELEASE) -Wl,-rpath=$(LIBS_RELEASE)
IFLAG = -I include -I lib/include
IFTEST = -I test/include

LIB = lib

LIBS_DEBUG = tp_shared_lib/debug
LIBS_RELEASE = tp_shared_lib/release
OBJ_DEBUG = obj/debug
OBJ_RELEASE = obj/release

.PHONY: all
all: debug release test runtest

.PHONY: debug
debug:
	mkdir -p $(LIBS_DEBUG)
	mkdir -p $(OBJ_DEBUG)
	$(CC) $(CFLAGS) $(IFLAG) -c -fpic src/$(TARGET).cpp -o $(TARGET).o
	mv $(TARGET).o $(OBJ_DEBUG)
	$(CC) $(CFLAGS) -shared -o $(LIB)$(TARGET).so $(OBJ_DEBUG)/$(TARGET).o
	mv $(LIB)$(TARGET).so $(LIBS_DEBUG)

.PHONY: release
release:
	mkdir -p $(LIBS_RELEASE)
	mkdir -p $(OBJ_RELEASE)
	$(CC) $(RCFLAGS) $(IFLAG) -c -fpic src/$(TARGET).cpp -o $(TARGET).o
	mv $(TARGET).o $(OBJ_RELEASE)
	$(CC) $(RCFLAGS) -shared -o $(LIB)$(TARGET).so $(OBJ_RELEASE)/$(TARGET).o
	mv $(LIB)$(TARGET).so $(LIBS_RELEASE)

.PHONY: test
test: debug
	$(CC) $(CFLAGS) $(LDFLAGS) $(IFLAG) $(IFTEST) test/$(TARGET)_test.cpp -o $(TARGET)_test.out -l$(TARGET)

.PHONY: runtest
runtest:
	./$(TARGET)_test.out

.PHONY: clean
clean:
	rm -fr $(OBJ_DEBUG)
	rm -fr $(OBJ_RELEASE)
	rm -fr $(LIBS_DEBUG)
	rm -fr $(TARGET)_test.out


# $^ all dependencies
# $< first dependency
# $@ the rule's target name
# % all of this type

