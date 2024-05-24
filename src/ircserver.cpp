/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 18:39:05 by omakran           #+#    #+#             */
/*   Updated: 2024/05/23 13:46:25 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ircserver.hpp"

Server::Server(int port, const std::string& password) : port(port), password(password) {
    initializeServer();
}

Server::~Server() {
    cleanUp();
}

void    Server::initializeServer() {
    struct sockaddr_in  server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation error: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Setsockopt error: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // set the socket to non-blocking mode
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Fcntl error: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // bind the socket to the address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind error: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections
    if (listen(server_fd, 10) == -1) {
        std::cerr << "Listen error: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // add the server socket to the polling list
    struct pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN;
    fds.push_back(server_pollfd);
}

// main loop of the server:
void    Server::pollLoop() {
    while (true) {
        int poll_count = poll(fds.data(), fds.size(), -1);
        if (poll_count < 0) {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        handleEvents();
    }
}

void    Server::handleEvents() {
    // iterate over all monitored file descriptors.
    for (size_t i = 0; i < fds.size(); i++) {
        // check if the event is ready for reading.
        if (fds[i].revents & POLLIN) {
            // check if the event is on the server socket (new client connection).
            if (fds[i].fd == server_fd) {
                // if it's the server socket, handle new connection.
                handleNewConnection();
            } else { // client socket.
                // otherise, handle client message.
                handleClientMessage(fds[i].fd);
            }
        }
    }
}

// accept a new client connection:
void    Server::handleNewConnection() {
    struct sockaddr_in  client_addr; // client address structure.
    socklen_t           client_len = sizeof(client_addr); // lenght of the client address structure.
    //                  accept the new connection.
    int                 client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        std::cerr << "Accept error: " << strerror(errno) << std::endl;
        return;
    }

    // set the new client socket to non-blocking mode.
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Fcntl error: " << strerror(errno) << std::endl;
        close (client_fd);
        return;
    }

    // add the new client to the list of file descriptors to poll
    struct pollfd       client_pollfd;
    client_pollfd.fd = client_fd; // client file descriptor.
    client_pollfd.events = POLLIN; // interested in reading events
    fds.push_back(client_pollfd); // add to the list of poll file descriptors.

    std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << ": " << ntohs(client_addr.sin_port) << std::endl;

    // add the new client to the client map.
    clients.insert(std::make_pair(client_fd, Client(client_fd)));
}

// handle a message from a client:
void    Server::handleClientMessage(int client_fd) {
    char    buffer[1024]; // buffer to reading data.
    // read data from the client.
    int     bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_read == -1) {
        std::cerr << "Recv error: " << strerror(errno) << std::endl;
        return;
    } else if (bytes_read == 0) {// connection closed by client.
        close(client_fd);
    
        // remove the client from polling list
        for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); ++it) {
            if (it->fd == client_fd) {
                fds.erase(it);
                break;
            }
        }
        clients.erase(client_fd); // remove the client from the client map
    } else {
        buffer[bytes_read] = '\0';

        std::cout << "Received message from client " << client_fd << ": " << buffer << std::endl;

        // pass the message to the client's handleMessage function.
        std::map<int, Client>::iterator it = clients.find(client_fd);
        if (it != clients.end()) {
            it->second.handleMessage(std::string(buffer));
        }
    }
}

// boardcast a message to all clients in a channel
void    Server::broadcastMessage(const std::string& message, const std::string& channel) {
    if (channels.find(channel) != channels.end()) {
        for (size_t i = 0; i < channels[channel].size(); i++)
        {
            int client_fd = channels[channel][i];
            send(client_fd, message.c_str(), message.size(), 0);
        }
    } else {
        std::cerr << "Channel " << channel << " does not exist." << std::endl;
    }
}

void    Server::cleanUp() {
    close (server_fd);
    for (size_t i = 0; i < fds.size(); i++)
    {
        close(fds[i].fd);
    }
    fds.clear();
    clients.clear();
}
