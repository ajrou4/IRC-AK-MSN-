/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 10:03:54 by codespace         #+#    #+#             */
/*   Updated: 2024/05/21 21:41:29 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include <sys/socket.h>

Bot::Bot()
{
}

Bot::~Bot()
{
}

Bot::Bot(const Bot &copy)
{
    *this = copy;
}

Bot &Bot::operator=(const Bot &copy)
{
    if (this != &copy)
    {
        this->_name = copy._name;
    }
    return *this;
}

void Bot::stratSocket() // This function that starts the socket
{
    socket(AF_INET, SOCK_STREAM, 0); // This is a socket function that creates a socket (AF_INET - IPv4, SOCK_STREAM - TCP, 0 - IP protocol
    
    // std::cout << "Bot::stratSocket()" << std::endl;
}