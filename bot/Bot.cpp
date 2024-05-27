/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 10:03:54 by codespace         #+#    #+#             */
/*   Updated: 2024/05/25 18:58:55 by haguezou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"
#include <sys/socket.h>
#include <cstring> 
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
  
    #define PORT 3000

Bot::Bot()
{
    // all of that should started here and get PORT as argument
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

void Bot::stratSocket() // This function that starts the socket and connect with the server
{
    int value = 1;
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    struct addrinfo *hints, *res;
    char buffer[1024];
    
    
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
    inet_pton(AF_INET, "localhost", &addr.sin_addr); // this function is used to convert the ip address from string to binary
    if(connect(socketfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Error: connect failed" << std::endl;
        exit(1);
    }
    char *msg = "JOIN #general\r\nJOIN #test\r\n";
    send(socketfd, msg, strlen(msg), 0);
    ssize_t status;
    status = send(socketfd, "/This", 5, 0); // this function is used to send the message to the server (socketfd is the socket that is connected to the server, the second argument is the message that we want to send, the third argument is the size of the message and the last argument is the flag that is used to send the message)
    std::cout << "status: " << status << std::endl;
    // if(listen(socketfd, 5) == -1)
    // {
        // std::cerr << "Error: listen failed | " << strerror(errno)  << std::endl;
        // exit(1);
    // }
    if(recv(socketfd, buffer, 5, 0) != -1)
    {
        std::cout << buffer << std::endl;
        // std::cerr << "Error: recv failed" << std::endl;   
     // this function is used to receive the message from the server (socketfd is the socket that is connected to the server, the second argument is the message that we want to receive, the third argument is the size of the message and the last argument is the flag that is used to receive the message)
    }
    // listen to incoming message from the server
    
    close(socketfd); // this function is used to close the socket (socketfd is the socket that we want to close)
}


/*

        if you
           |
          / \
         /   \
        /     \
[don't try]  [try]
      |        |
     [✖️]      / \
             /   \
            [✔️]  [✖️]
                
*/