/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 18:39:36 by omakran           #+#    #+#             */
/*   Updated: 2024/05/21 22:39:11 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP

# include <iostream>
# include <sys/types.h>
# include <sys/socket.h> // for socket functions
# include <netinet/in.h> // for sockaddr_in
# include <unistd.h> // for close
# include <fcntl.h> // for fcntl
# include <poll.h> // for poll
# include <vector>
# include <map>
# include <netdb.h> // for getaddrinfo
# include <cstdlib> // for atoi
# include <arpa/inet.h> // for inet_ntoa
# include <string>


# include "client.hpp"

class Server {
    //                                      port number for the server
    int                                     port;
    //                                      connection password
    std::string                             password;
    //                                      server socket file descriptor
    int                                     server_fd;
    //                                      is passed to the poll function, which will monitor all the file descriptors listed in it.
    std::vector<struct pollfd>              fds;
    //                                      map to manage the clients
    std::map<int, Client>                   clients;
    //                                      map of channel names to list of client file descriptors.
    std::map<std::string, std::vector<int> > channels;

    // ------------------- member functions: ----------------------- 
    // 1-   initialize the serever socket.
    void    initializeServer();
    // 2-   handles events returned by poll().
    void    handleEvents();
    // 3-   accept a new client connection.
    void    handleNewConnection();
    // 4-   handle a message from client
    void    handleClientMessage(int client_fd);
    // 5-   broadcasts a message to all clients in a channel.
    void    broadcastMessage(const std::string& message, const std::string& channel);

    //      additional helper functions as needed (oziyada mn ras lhmq hh).

    //      close all opne sockets and clean up resources:
    void    cleanUp();

public:
    // constructor:
    Server(int port, const std::string& password);

    //      main loop for polling and handling events.
    void    pollLoop();

    // destructor:
    ~Server();

};

#endif
