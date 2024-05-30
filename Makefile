# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/18 18:37:28 by omakran           #+#    #+#              #
#    Updated: 2024/05/30 10:43:57 by haguezou         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserver

SRC = src/main.cpp src/ircserver.cpp src/client.cpp src/ircImplementation.cpp src/Channel.cpp

# SRC = src/main.cpp src/ircserver.cpp #$(wildcard src/*.cpp)

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 

#-fsanitize=address

CXX = c++

OBJ = $(SRC:.cpp=.o)

INCLUDE = -I include

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