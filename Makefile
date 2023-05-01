NAME=msh
SRC= shell.c \
     free.c \
     ft_strlen.c \
     ft_strdup.c \
     ft_strjoin.c \
     ft_strncmp.c 
C_FLAGS= -Wall -Wextra -Werror -g
header= shell.h
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
