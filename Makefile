NAME      := game-jam
CXX       := c++
CXXSTD    := -std=c++17
WARNINGS  := -Wall -Wextra -Werror
OPT       := -O2 -g

# -isystem, not -I: raylib's own header warnings must not fail YOUR build.
RAYLIB_PREFIX := $(shell brew --prefix raylib)
RAYLIB_INC    := -isystem $(RAYLIB_PREFIX)/include
RAYLIB_LIBS   := -L$(RAYLIB_PREFIX)/lib -lraylib \
                 -framework Cocoa -framework IOKit \
                 -framework CoreVideo -framework OpenGL

SRC_DIR   := src
OBJ_DIR   := build
SRCS      := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS      := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CXXFLAGS  := $(CXXSTD) $(WARNINGS) $(OPT) -I$(SRC_DIR) $(RAYLIB_INC)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) $(RAYLIB_LIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ---- tests: core/ only. No raylib, no main.cpp, no window. ----
CORE_SRCS := $(shell find $(SRC_DIR)/core -name '*.cpp')
TEST_SRCS := $(shell find tests -name '*.cpp' ! -name 'catch_amalgamated.cpp')

test: $(OBJ_DIR)/catch_amalgamated.o
	$(CXX) $(CXXSTD) $(WARNINGS) -O0 -g -I$(SRC_DIR) -isystem tests \
	    $(CORE_SRCS) $(TEST_SRCS) $< -o run_tests
	./run_tests

# Vendored third-party: compiled WITHOUT -Werror on purpose.
$(OBJ_DIR)/catch_amalgamated.o: tests/catch_amalgamated.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXSTD) -O0 -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) run_tests

re: fclean all

.PHONY: all clean fclean re test