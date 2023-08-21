# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hlaadiou <hlaadiou@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/21 02:50:41 by hlaadiou          #+#    #+#              #
#    Updated: 2023/08/21 03:09:01 by hlaadiou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME =		minishell
SRCS = 		sources/builtins/cd_builtin.c \
			sources/builtins/echo_builtin.c \
			sources/builtins/env_builtin.c \
			sources/builtins/exit_builtin.c \
			sources/builtins/export_builtin.c \
			sources/builtins/export_builtin_utils.c \
			sources/builtins/pwd_builtin.c \
			sources/builtins/unset_builtin.c \
			sources/environment/environment.c \
			sources/environment/environment_utils.c \
			sources/executor/executor.c \
			sources/executor/executor_utils1.c \
			sources/executor/executor_utils2.c \
			sources/executor/executor_utils3.c \
			sources/executor/executor_utils4.c \
			sources/executor/executor_utils5.c \
			sources/executor/executor_utils6.c \
			sources/executor/executor_utils7.c \
			sources/executor/executor_utils8.c \
			sources/executor/executor_utils9.c \
			sources/executor/heredoc.c \
			sources/executor/open_heredocs.c \
			sources/garbage/free.c \
			sources/garbage/garbage.c \
			sources/libft/ft_atoi.c \
			sources/libft/ft_atoull.c \
			sources/libft/ft_isalnum.c \
			sources/libft/ft_isalpha.c \
			sources/libft/ft_isdigit.c \
			sources/libft/ft_itoa.c \
			sources/libft/ft_putstr_fd.c \
			sources/libft/ft_split.c \
			sources/libft/ft_strchr.c \
			sources/libft/ft_strcmp.c \
			sources/libft/ft_strdup.c \
			sources/libft/ft_strjoin.c \
			sources/libft/ft_strlcpy.c \
			sources/libft/ft_strlen.c \
			sources/libft/ft_strncmp.c \
			sources/libft/ft_strrchr.c \
			sources/libft/ft_strtrim.c \
			sources/libft/ft_substr.c \
			sources/macro_processor/dir.c \
			sources/macro_processor/env_insert_sort.c \
			sources/macro_processor/list_expander.c \
			sources/macro_processor/list_expander_utils1.c \
			sources/macro_processor/list_expander_utils2.c \
			sources/macro_processor/list_expander_utils3.c \
			sources/macro_processor/pattern_insert_sort.c \
			sources/macro_processor/redir_expander.c \
			sources/macro_processor/redir_expander_utils1.c \
			sources/macro_processor/redir_expander_utils2.c \
			sources/macro_processor/redir_expander_utils3.c \
			sources/macro_processor/redir_expander_utils4.c \
			sources/macro_processor/split_redir.c \
			sources/macro_processor/split_token.c \
			sources/macro_processor/wildcard.c \
			sources/main/minishell.c \
			sources/parser/rdp.c \
			sources/parser/rdp_utils1.c \
			sources/parser/rdp_utils2.c \
			sources/parser/rdp_utils3.c \
			sources/prompt/prompt.c \
			sources/signals/hdoc_signals.c \
			sources/signals/signals.c \
			sources/tokenizer/lexer.c \
			sources/tokenizer/lexer_utils1.c \
			sources/tokenizer/lexer_utils2.c \
			sources/tokenizer/lexer_utils3.c \
			sources/tokenizer/syntax.c \
			sources/tokenizer/syntax_utils1.c \
			sources/tokenizer/syntax_utils2.c
C_FLAGS =	-Wall -Wextra -Werror -g #-fsanitize=address
CC		=	cc
HEADER  = 	include/minishell.h
LIB     = 	-L $(HOME)/.brew/Cellar/readline/8.2.1/lib
INC     = 	-I $(HOME)/.brew/Cellar/readline/8.2.1/include
OBJ     = 	$(SRCS:.c=.o)

all: 		$(NAME)

$(NAME): 	$(OBJ)
			$(CC) $(C_FLAGS) $^ -o $@ -lreadline $(LIB) $(INC)

%.o: %.c 	$(HEADER)
			$(CC) $(C_FLAGS) $(INC) -c $< -o $@

clean:
			rm -f $(OBJ)

fclean: 	clean
			rm -f $(NAME)

re: 		fclean all

.PHONY: 	all clean fclean re
