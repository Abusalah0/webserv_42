NAME = webserv

CPP = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

SRC_DIR = src

SRC = $(SRC_DIR)/main.cpp $(SRC_DIR)/tokenizer.cpp

OBJ_DIR = obj

OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CPP) $(FLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CPP) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re