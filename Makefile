UNAME := $(shell uname)

#INCLUDE_DIRS=$(BOOST_INCLUDE)
#INCLUDE_STR=$(foreach d, $(INCLUDE_DIRS), -I$d)

# LIB_DIRS=$(BOOST_LIB_DIR)
# LIB_STR=$(foreach d, $(LIB_DIRS), -L$d)

# For Mac OS X 10.10 x86_64 Yosemite
ifeq ($(UNAME), Darwin)
  CFLAGS = -Wall -std=c++11 -fsanitize-undefined-trap-on-error -fsanitize=integer-divide-by-zero
  DEBUG_FLAGS = -Ofast -ferror-limit=10
  #LDFLAGS = -lboost_program_options-mt -lboost_serialization -lpthread
# For Ubuntu 12.04 x86_64
else ifeq ($(UNAME), Linux)
  CFLAGS = -Wall -Wl,--no-as-needed -std=c++11 
  DEBUG_FLAGS = -gdwarf-3 -O0 -DDEBUG # -gdwarf-3 necessary for debugging with gdb v7.4
endif

#DIR_PARAMS=$(INCLUDE_STR) $(LIB_STR)

CC=clang++ 
TARGET = yggdrasil
SRC = main.cpp
OBJ_FILES = $(patsubst %.cpp,%.o,$(SRC))

.PHONY: all clean

all: CFLAGS += $(DEBUG_FLAGS) 
all: LINK_FLAGS += $(DEBUG_FLAGS) 
#$(DIR_PARAMS) $(LDFLAGS)  
all: $(OBJ_FILES)
	$(CC) -o $(TARGET) $(OBJ_FILES)

#$(CC) $(CFLAGS) $(INCLUDE_STR) -c $< -o $@
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET)
	rm -f $(OBJ_FILES)
