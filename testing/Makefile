GCC=llvm-gcc
CXX=llvm-g++
AR=ar

# Base PATH
BASE_PATH = .
SRC_PATH  = $(BASE_PATH)/src
FLIMGTK_PATH = ../../fl_imgtk/lib
XCOI_PATH = /System/Library/Frameworks/ApplicationServices.framework/Versions/A/Headers

# TARGET settings
TARGET_PKG = FLFTRenderTest
TARGET_DIR = ./bin
TARGET_OBJ = ./obj

# DEFINITIONS
DEFS  = -D_POSIX_THREADS -DSUPPORT_DRAGDROP -DMININI_ANSI
DEFS += -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_THREAD_SAFE -D_REENTRANT

# Compiler optiops 
COPTS = -O2

# CC FLAG
CFLAGS += -I$(SRC_PATH)
CFLAGS += -I$(XCOI_PATH)
CFLAGS += -I../src
CFLAGS += -I/usr/local/include/freetype2
CFLAGS += -I$(FLIMGTK_PATH)
CFLAGS += $(DEFS)
CFLAGS += $(COPTS)

# LINK FLAG
LFLAGS += -L$(FLIMGTK_PATH)
LFLAGS += -L/usr/local/lib
LFLAGS += -lfltk_png
LFLAGS += -lfltk_jpeg
LFLAGS += -lz
LFLAGS += -lfltk_images
LFLAGS += -lfltk 
LFLAGS += -lfl_imgtk
LFLAGS += -lfreetype
LFLAGS += -lpthread
LFLAGS += -framework Cocoa -framework quartz -framework Foundation -framework AppKit

# Sources
SRCST = $(wildcard $(SRC_PATH)/*.cpp)
SRCSF = $(wildcard ../src/*.cpp)

# Objects
OBJST = $(SRCST:$(SRC_PATH)/%.cpp=$(TARGET_OBJ)/%.o)
OBJSF = $(SRCSF:../src/%.cpp=$(TARGET_OBJ)/%.o)

all: prepare clean continue
continue: $(TARGET_DIR)/$(TARGET_PKG)

prepare:
	@mkdir -p $(TARGET_DIR)
	@mkdir -p $(TARGET_OBJ)

clean:
	@rm -rf $(TARGET_DIR)/$(TARGET_PKG)
	@rm -rf $(TARGET_OBJ)/*.o

$(OBJST): $(TARGET_OBJ)/%.o: $(SRC_PATH)/%.cpp
	@$(CXX) $(CFLAGS) -c $< -o $@

$(OBJSF): $(TARGET_OBJ)%.o: ../src/%.cpp
	@$(CXX) $(CFLAGS) -c $< -o $@

$(TARGET_DIR)/$(TARGET_PKG): $(OBJST) $(OBJSF)
	$(CXX) $(OBJST) $(OBJSF) $(CFLAGS) $(LFLAGS) -o $@

