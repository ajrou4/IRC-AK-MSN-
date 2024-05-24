/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 10:03:54 by codespace         #+#    #+#             */
/*   Updated: 2024/05/22 22:36:19 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include <sys/socket.h>
#include <cstring> 
#include <arpa/inet.h>
#define PORT 8080

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
    int value = 1;
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    

    
    if (socketfd == -1)
    {
         std::cerr << "Error: socket failed" << std::endl;
         exit(1);
    }
    memset(&addr, 0, sizeof(addr)); 
    if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) == -1)
    {
        std::cerr << "Error: setsockopt failed" << std::endl;
    // this function is used to reuse the address that means we can use the same address again and avoid error like address already in use
    }
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    if (bind(socketfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        std::cerr << "Error: bind failed" << std::endl;
        exit(1);
    }
    listen(socketfd, 3); // this function is used to listen to the socket (socketfd) and the second argument is the maximum number of connections that can be queued
    
    if(accept(socketfd, (struct sockaddr *)&addr, (socklen_t*)&addr) == -1)
    {
        std::cerr << "Error: accept failed" << std::endl;
        exit(1);  
    } // this function is used to accept the connection from the client (socketfd is the socket that is listening to the connection and addr is the address of the client and the last argument is the size of the address of the client
    
}