NAME=msh
SRC= environment.c \
	 ft_strlen.c \
	 ft_strdup.c \
	 ft_strcmp.c \
	 ft_strncmp.c \
	 ft_strjoin.c \
	 minishell.c \
	 prompt.c \

C_FLAGS= -Wall -Wextra -Werror -g
header= minishell.h
obj= $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(obj)
	gcc $(C_FLAGS) $^ -lreadline -o $@

%.o: %.c $(header)
	gcc $(C_FLAGS) -c $< -o $@

clean:
	rm -f $(obj)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
