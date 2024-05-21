/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 18:39:05 by omakran           #+#    #+#             */
/*   Updated: 2024/05/20 00:22:09 by omakran          ###   ########.fr       */
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
    pollLoop();
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

// main loop of the server:
void    Server::pollLoop() {
    // to ensures the server is always ready to handle new connections and messages.
    while (true) {
        //                      fds.size(): this gives the number of file descriptors being monitored.
        int ret = poll(&fds[0], fds.size(), -1); // -1:  timeout means it will wait indefinitely for an event.
        if (ret == -1) {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        // to process the events detected.
        handleEvents();
    }
}

void    Server::handleEvents() {
    // iterate over all monitored file descriptors.
    for (size_t i = 0; i < fds.size(); i++) {
        // check if the current file descriptor has a POLLIN even (data to read).
        // this is a bitwise & operation, if the POLLIN bit is set in revents, it will be non-zero (true).
        if (fds[i].revents & POLLIN) {
            // check if the event is on the server socket (new client connection).
            if (fds[i].fd == fds[0].fd) {
                // accept the new client connection.
                int client_fd = accept(fds[i].fd, NULL, NULL);
                if (client_fd == -1) {
                    std::cerr << "Accept error: " << strerror(errno) << std::endl;
                    continue;
                }
                // set the new client socket to non-blocking mode.
                fcntl(client_fd, F_SETFL, O_NONBLOCK);

                // create a new pollfd structer for the client.
                struct pollfd   client_pollfd;
                client_pollfd.fd = client_fd;
                client_pollfd.events = POLLIN;

                // add the new client to the list of monitored file descriptors.
                fds.push_back(client_pollfd);
                // dont't forget to add client.
            } else { // client socket.
            }
        }
    }
}

void    Server::cleanUp() {
    for (size_t i = 0; i < fds.size(); i++)
    {
        close(fds[i].fd);
    }
    fds.clear();
    // don't forget to cleare client also.
}

