NAME        := computor
LIB         := libcomputor.a
UNIT_BIN    := unit_tests

CXX         := c++
CXXFLAGS    := -Wall -Wextra -Werror -std=c++11 -Iinclude
DEPFLAGS    := -MMD -MP

OBJ_DIR     := obj

LIB_SRCS    := src/MathUtils.cpp \
               src/Rational.cpp \
               src/Lexer.cpp \
               src/Parser.cpp \
               src/Term.cpp \
               src/Polynomial.cpp \
               src/Solver.cpp \
               src/Token.cpp \
               src/Printer.cpp

MAIN_SRC    := main.cpp

TEST_SRCS   := tests/unit/test_main.cpp \
               tests/unit/test_mathutils.cpp \
               tests/unit/test_rational.cpp \
               tests/unit/test_lexer.cpp \
               tests/unit/test_parser.cpp \
               tests/unit/test_polynomial.cpp \
               tests/unit/test_solver.cpp \
               tests/unit/test_printer.cpp

LIB_OBJS    := $(LIB_SRCS:%.cpp=$(OBJ_DIR)/%.o)
MAIN_OBJ    := $(MAIN_SRC:%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJS   := $(TEST_SRCS:%.cpp=$(OBJ_DIR)/%.o)
DEPS        := $(LIB_OBJS:.o=.d) $(MAIN_OBJ:.o=.d) $(TEST_OBJS:.o=.d)

all: $(NAME)

$(NAME): $(MAIN_OBJ) $(LIB)
	$(CXX) $(CXXFLAGS) $(MAIN_OBJ) $(LIB) -o $@

$(LIB): $(LIB_OBJS)
	ar rcs $@ $^

$(UNIT_BIN): $(TEST_OBJS) $(LIB)
	$(CXX) $(CXXFLAGS) $(TEST_OBJS) $(LIB) -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

unit: $(UNIT_BIN)
	./$(UNIT_BIN)

e2e: $(NAME)
	@sh tests/e2e/run.sh ./$(NAME)

test: unit e2e

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(LIB) $(UNIT_BIN)

re: fclean
	$(MAKE) all

-include $(DEPS)

.PHONY: all clean fclean re unit e2e test
