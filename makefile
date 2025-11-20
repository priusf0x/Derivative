SOURCES = common/simple_parser.cpp tree/tree.cpp stack/stack.cpp common/tools.cpp tree/tree_dump.cpp common/my_string.cpp deritative_main.cpp

OBJ_DIR = obj
SOURCE_DIR = src

OBJECTS := $(addprefix $(OBJ_DIR)/, $(SOURCES:.cpp=.o))
SOURCES  := $(addprefix $(SOURCE_DIR)/, $(SOURCES))

HEADERS = $(wildcard $(SOURCE_DIR)/*.h)

TARGET = deritative.out

CC = g++
 

CFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O2 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer  -Wstack-usage=8192 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
# -Wlarger-than=8192
CFLAGS += -I$(SOURCE_DIR)/ -I$(SOURCE_DIR)/visuals -I$(SOURCE_DIR)/common -I$(SOURCE_DIR)/tree -I$(SOURCE_DIR)/stack -I$(SOURCE_DIR)/game -lncursesw -I$(SOURCE_DIR)/my_allocator

#START: ncurses options

# CFLAGS += -g -Wall -Wl,-O1 -pipe -O2 -flto=2 -fno-fat-lto-objects -fuse-linker-plugin -fPIC

# DEL_FILE      = rm -f
# CHK_DIR_EXISTS= test -d
# MKDIR         = mkdir -p
# COPY          = cp -f
# COPY_FILE     = cp -f
# COPY_DIR      = cp -f -R
# INSTALL_FILE  = install -m 644 -p
# INSTALL_PROGRAM = install -m 755 -p
# INSTALL_DIR   = cp -f -R
# DEL_FILE      = rm -f
# SYMLINK       = ln -f -s
# DEL_DIR       = rmdir
# MOVE          = mv -f
# TAR           = tar -cf
# COMPRESS      = gzip -9f
# LIBS_DIRS     = -I./include/
# LIBS 		  = $(LIBS_DIRS) -lncursesw
# SED           = sed
# STRIP         = strip

#END: ncurses options

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	@mkdir -p logs
	@echo "Compiling" $<
	@$(CC) $(CFLAGS) -c $< -o $@ 
	@echo "Compiled Successfully" $<

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Linked Successfully"

test:
	@./list_sester.out

build: TARGET

clean:
	@rm -rf $(OBJ_DIR)
	@rm -f $(TARGET)
	@rm -rf logs
	@echo "Cleaned Successfully"
