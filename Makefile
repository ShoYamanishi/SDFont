.DEFAULT_GOAL := all


INSTALL_DIR_LIB = /usr/local/lib
INSTALL_DIR_BIN = /usr/local/bin

.PHONY: all
.PHONY: install
.PHONY: clean

HAS_LIBPNG=YES

ifeq ($(HAS_LIBPNG), YES)
USE_LIBPNG=-DUSE_LIBPNG
LINK_LIBPNG=-lpng
endif

CC           = c++
LD           = c++
RM           = rm -f
RMR          = rm -fr
CP           = cp

PWD          =  $(shell pwd)
SRC_DIR_LIB  = ./src_lib
SRC_DIR_BIN  = ./src_bin
BIN_DIR      = ./bin
LIB_DIR      = ./libs

OBJ_DIR_REL  = ./objs_release
OBJ_DIR_DBG  = ./objs_debug


CPPFLAGS     = -Wall -std=c++1y
CPPFLAGS_REL = -fPIC -O3 $(USE_LIBPNG)
CPPFLAGS_DBG = -g $(USE_LIBPNG)
CPPFLAGS_BIN = $(USE_LIBPNG) -L./libs -lfreetype -framework CoCoa -framework OpenGL -framework IOKit -framework CoreFoundation -framework CoreVideo -lglfw3 -lGLEW $(LINK_LIBPNG) -lstdc++

CFLAGS_DBG   = -I. -I./include -I/usr/X11/include -I/usr/local/include/freetype2
CFLAGS_REL   = -I. -I./include -I/usr/X11/include -I/usr/local/include/freetype2

LDFLAGS      = -shared -lpng -lfreetype -lGLEW -framework OpenGL
TARGET_LIB   = $(LIB_DIR)/libsdfont.so

DIR_GUARD    = @mkdir -p $(@D)

OBJS_REL     = $(patsubst %,$(OBJ_DIR_REL)/%, \
                         $(subst .cpp,.o, \
                           $(notdir \
                             $(wildcard \
                               $(SRC_DIR_LIB)/*.cpp ))))

OBJS_DBG     = $(patsubst %,$(OBJ_DIR_DBG)/%, \
                         $(subst .cpp,.o, \
                           $(notdir \
                             $(wildcard \
                               $(SRC_DIR_LIB)/*.cpp ))))


BINS         = $(patsubst %,$(BIN_DIR)/%, \
                         $(basename \
                           $(notdir \
                             $(wildcard \
                               $(SRC_DIR_BIN)/*.cpp ))))


$(OBJ_DIR_REL)/%.o: $(SRC_DIR_LIB)/%.cpp
	$(DIR_GUARD)
	$(CC) $(CFLAGS_REL) $(CPPFLAGS) $(CPPFLAGS_REL) -c $< -o $@

$(OBJ_DIR_DBG)/%.o: $(SRC_DIR_LIB)/%.cpp
	$(DIR_GUARD)
	$(CC) $(CFLAGS_DBG) $(CPPFLAGS) $(CPPFLAGS_DBG) -c $< -o $@

$(TARGET_LIB):	$(OBJS_REL)
	$(DIR_GUARD)
	$(LD) $(LDFLAGS) -o $@ $^

$(BIN_DIR)/%:	$(SRC_DIR_BIN)/%.cpp $(TARGET_LIB)
	$(DIR_GUARD)
	$(CC) $(CFLAGS_REL) $(CPPFLAGS) $(CPPFLAGS_BIN) $^ $(TARGET_LIB) -o $@

all:	$(TARGET_LIB) $(BINS)


clean:
	-$(RM) $(TARGET_LIB) 
	-$(RMR) $(OBJ_DIR_REL) $(OBJ_DIR_DBG) $(LIB_DIR) $(BIN_DIR)

install:
	sudo $(CP) $(TARGET_LIB) $(INSTALL_DIR_LIB)
