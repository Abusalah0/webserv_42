NAME = webserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g -MMD

SRC_DIR = src
PARSER_DIR = parser
HTTPBUFFERS_DIR = HTTPBuffers

SRC = \
	main.cpp \
	tokenizer.cpp \
	Exceptions.cpp \
	BaseBlock.cpp \
	CommonUtils.cpp \
	Server.cpp \
	ServerContainer.cpp \
	Location.cpp \
	Client.cpp \
	HTTPBuffer.cpp \
	HTTPHeader.cpp \
	normalize_path.cpp \
	CGIHandler.cpp \
	parser/parse_baseblock.cpp \
	parser/parser.cpp \
	parser/parser_utils.cpp \
	parser/parse_server.cpp \
	parser/parse_location.cpp \
	parser/parse_directives.cpp \
	parser/parser_print.cpp \

OBJ_DIR = obj
SRCS = $(addprefix $(SRC_DIR)/, $(SRC)) 
OBJ = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.d)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/$(PARSER_DIR)/%.o: $(SRC_DIR)/$(PARSER_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)/$(PARSER_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/$(HTTPBUFFERS_DIR)/%.o: $(SRC_DIR)/$(HTTPBUFFERS_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)/$(HTTPBUFFERS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

-include $(DEPS)