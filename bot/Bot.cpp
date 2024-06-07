/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 10:03:54 by codespace         #+#    #+#             */
/*   Updated: 2024/06/07 19:21:45 by haguezou         ###   ########.fr       */
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
    
}

void Bot::connectToServer() // This function that connects to the server
{
    // connect to the server
    int sockfd;
    
    struct sockaddr_in servaddr;
    struct hostent *server;
    
    
    

    // set the server address
    bzero((char *) &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // loopback address 127.0.0.1
    
    // get the server address
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }
    std::cout << "Socket created and connected to localhost" << std::endl;
    
    std::string message = "Hello from the client";
    int message_len = message.length();
    ssize_t n;

    std::cout << sockfd << std::endl;
    std::cout << message_len << std::endl;
    std::cout << n << std::endl;
    
    n = send(sockfd, message.c_str(), message_len, 0);
    if(n < 0)
    {
        std::cerr << "Error sending message" << std::endl;
        exit(1);
    }
    std::cout << "Message sent" << std::endl;
}

void Bot::getChannels(Channel &channel) // This function that gets the channels
{
    _channels.push_back(&channel);
}

void Bot::run() // This function that runs the bot
{
    // run the bot
    // create a socket
    // connect to the server
    // get the channels
    // get the clients
    // get the messages
    // send the messages
    // close the socket
    // close the bot
    Bot::connectToServer();
}

void Bot::lunchBot(int fd_client, std::vector<std::string> command) // This function that lunches the bot
{

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