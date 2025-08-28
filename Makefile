NAME = webserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iinclude

SRC_DIR = src

SRC = main.cpp \
		tokenizer.cpp \
		Exceptions.cpp \
		BaseBlock.cpp \
		CommonUtils.cpp \
		Server.cpp \
		ServerContainer.cpp \
		Location.cpp \
		parser/parse_baseblock.cpp \
		parser/parser.cpp \
		parser/parser_utils.cpp \
		parser/parse_server.cpp \
		parser/parse_location.cpp \
		parser/parse_directives.cpp \


OBJ_DIR = obj
SRCS = $(addprefix $(SRC_DIR)/, $(SRC)) 
OBJ = $(SRCS:%.cpp=%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) 
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

# Test targets
test: $(NAME)
	@echo "Building and running parser tests..."
	@cd tests && make test

test-build:
	@echo "Building test suite..."
	@cd tests && make

test-clean:
	@echo "Cleaning test build files..."
	@cd tests && make clean

test-summary:
	@./tests/test_summary.sh

.PHONY: all clean fclean re test test-build test-clean test-summary