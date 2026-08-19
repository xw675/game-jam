NAME      := game-jam
CXX       := c++
CXXSTD    := -std=c++17
WARNINGS  := -Wall -Wextra -Werror
OPT       := -O2 -g

SRC_DIR   := src
OBJ_DIR   := build

UNAME_S := $(shell uname -s)
EXE     :=

ifeq ($(UNAME_S),Darwin)
    RAYLIB_PREFIX := $(shell brew --prefix raylib)
    RAYLIB_INC    := -isystem $(RAYLIB_PREFIX)/include
    RAYLIB_LIBS   := -L$(RAYLIB_PREFIX)/lib -lraylib \
                     -framework Cocoa -framework IOKit \
                     -framework CoreVideo -framework OpenGL \
                     -framework CoreAudio -framework AudioToolbox
endif

ifeq ($(UNAME_S),Linux)
    RAYLIB_INC  := $(shell pkg-config --cflags raylib 2>/dev/null)
    RAYLIB_LIBS := $(shell pkg-config --libs raylib 2>/dev/null)
    ifeq ($(strip $(RAYLIB_LIBS)),)
        RAYLIB_LIBS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
endif

ifneq (,$(findstring MINGW,$(UNAME_S)))
    EXE         := .exe
    RAYLIB_INC  :=
    RAYLIB_LIBS := -lraylib -lopengl32 -lgdi32 -lwinmm -static-libgcc -static-libstdc++
endif

ifndef RAYLIB_LIBS
    $(error Unsupported platform "$(UNAME_S)" - add a block for it above)
endif


SRCS := $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/*/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CXXFLAGS := $(CXXSTD) $(WARNINGS) $(OPT) -I$(SRC_DIR) $(RAYLIB_INC)

all: $(NAME)$(EXE)

$(NAME)$(EXE): $(OBJS)
	$(CXX) $(OBJS) $(RAYLIB_LIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ---- tests: core/ only. No raylib, no main.cpp, no window. ----
# Note there is no $(RAYLIB_*) anywhere below. That is the point: the test
# binary builds identically on all three platforms.
CORE_SRCS := $(wildcard $(SRC_DIR)/core/*.cpp)
TEST_SRCS := $(filter-out tests/catch_amalgamated.cpp,$(wildcard tests/*.cpp))

test: $(OBJ_DIR)/catch_amalgamated.o
	$(CXX) $(CXXSTD) $(WARNINGS) -O0 -g -I$(SRC_DIR) -isystem tests \
	    $(CORE_SRCS) $(TEST_SRCS) $< -o run_tests$(EXE)
	./run_tests$(EXE)

# Vendored third-party: compiled WITHOUT -Werror on purpose.
$(OBJ_DIR)/catch_amalgamated.o: tests/catch_amalgamated.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXSTD) -O0 -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)$(EXE) run_tests$(EXE)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re test