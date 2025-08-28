# Test Makefile for webserv parser
NAME_TEST = test_parser
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDE_DIR = ../include

# Source files for the parser (excluding main.cpp)
PARSER_SRCS = ../src/tokenizer.cpp \
              ../src/Exceptions.cpp \
              ../src/BaseBlock.cpp \
              ../src/CommonUtils.cpp \
              ../src/Server.cpp \
              ../src/ServerContainer.cpp \
              ../src/Location.cpp \
              ../src/parser/parse_baseblock.cpp \
              ../src/parser/parser.cpp \
              ../src/parser/parser_utils.cpp \
              ../src/parser/parse_server.cpp \
              ../src/parser/parse_location.cpp \
              ../src/parser/parse_directives.cpp

# Test source file
TEST_SRCS = test_parser.cpp

# Object files
PARSER_OBJS = $(PARSER_SRCS:.cpp=.o)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

all: $(NAME_TEST)

$(NAME_TEST): $(PARSER_OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $(PARSER_OBJS) $(TEST_OBJS) -o $(NAME_TEST)

%.o: %.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

test: $(NAME_TEST)
	@echo "Running parser tests..."
	@./$(NAME_TEST)

quick-test: $(NAME_TEST)
	@echo "Running parser tests (no debug output)..."
	@./$(NAME_TEST) 2>/dev/null || true

clean:
	rm -f $(PARSER_OBJS) $(TEST_OBJS)

fclean: clean
	rm -f $(NAME_TEST)

re: fclean all

.PHONY: all test clean fclean re
