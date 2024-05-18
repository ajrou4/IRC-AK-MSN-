/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 18:39:05 by omakran           #+#    #+#             */
/*   Updated: 2024/05/19 00:41:23 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"

Server::Server(int port, const std::string& password)
    : port(port), password(password) {}

Server::~Server() {
    cleanUp();
}

// ============ Start ============
void    Server::start() {
    initializeServer();
}

void    Server::initializeServer() {
    // when a server needs to bind to a particular IP address and port, it needs to provide this information to the OS in a structured way.
    struct sockaddr_in  server_addr;
    server_addr.sin_family = AF_INET; //  tells the OS what type of addresses it should expect: IPv4.
    server_addr.sin_port = htons(port); // converts the port number from host's byte order to the network byte order.
    server_addr.sin_addr.s_addr = INADDR_ANY; // a special constant that allows the server to accept connections on any of the host’s IP addresses.
    
    // creating the socket:
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); // tells the OS to create a TCP/IP socket, 0 is default protocol.
    if (server_fd == -1) {
        std::cerr << "Socket error: " << strerror(errno) << std::endl; // errno to indicate the error. strerror(errno) returns a string that describes this error.
        exit (EXIT_FAILURE);
    }

    // binding the socket:
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind error: " << strerror(errno) << std::endl;
        exit (EXIT_FAILURE);
    }

    // listening for connections:
    //                    SOMAXCONN: the max number of connection the system suppports. 
    if (listen(server_fd, SOMAXCONN) == -1) {
        std::cerr << "Listen error: " << strerror(errno) << std::endl;
        close(server_fd);
        exit (EXIT_FAILURE);
    }

    // setting the socket to Non-Blocking Mode:
    fcntl(server_fd, F_SETFL, O_NONBLOCK); // to handle multiple clients simultaneously without getting stuck waiting for one operation to complete.
    
    // adding the server socket to the polling list:
    struct pollfd server_pollfd;
    server_pollfd.fd = server_fd; //  tells poll that we are interested in monitoring this particular socket.
    server_pollfd.events = POLLIN; // this indicates that we are interested in knowing when there is data to be read on server_fd, (incoming data).
    fds.push_back(server_pollfd); // adds server_pollfd to a vector.
}

void    Server::cleanUp() {
    for (size_t i = 0; i < fds.size(); i++)
    {
        close(fds[i].fd);
    }
    fds.clear();
    // don't forget to cleare client also.
}

