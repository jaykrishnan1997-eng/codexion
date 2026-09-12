# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jkrishna <jkrishna@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/09/11 12:22:45 by jkrishna          #+#    #+#              #
#    Updated: 2026/09/12 11:47:45 by jkrishna         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME 		= codexion

CFLAGS 		= -Wall -Wextra -Werror
CC 			= cc
THREAD 		= -pthread

INCLUDES 	= -Iinclude

SRC 		= src/codexion.c \
			  src/init.c \
			  src/threads.c \
			  src/routine.c \
			  src/actions.c \
			  src/monitor.c \
			  src/mutex.c \
			  src/heap.c \
			  src/utils.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(THREAD) $(OBJ) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(THREAD) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re