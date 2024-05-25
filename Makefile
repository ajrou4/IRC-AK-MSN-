# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: omakran <omakran@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/18 18:37:28 by omakran           #+#    #+#              #
#    Updated: 2024/05/25 19:38:23 by omakran          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRC = src/main.cpp src/ircserver.cpp src/client.cpp

# SRC = src/main.cpp src/ircserver.cpp #$(wildcard src/*.cpp)

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

CXX = c++

OBJ = $(SRC:.cpp=.o)

INCLUDE = -Iinclude

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all