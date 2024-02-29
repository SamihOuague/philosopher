# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: souaguen <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/31 01:35:37 by souaguen          #+#    #+#              #
#    Updated: 2024/02/29 01:36:42 by souaguen         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC=cc
CFLAGS=-Wall -Werror -Wextra -pthread # -fsanitize=thread #-Weverything
SRC=main.c \
    philo.c \
    monitor.c \
    lst_utils.c \
    philo_utils.c \
    time_utils.c \
    utils.c 
OBJ=$(SRC:.c=.o)
NAME=philo

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) -lpthread

all: $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
