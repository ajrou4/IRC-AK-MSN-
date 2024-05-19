/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 18:39:36 by omakran           #+#    #+#             */
/*   Updated: 2024/05/19 19:32:02 by omakran          ###   ########.fr       */
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

class Server {
    //                          port number for the server
    int                         port;
    //                          connection password
    std::string                 password;
    //                          is passed to the poll function, which will monitor all the file descriptors listed in it.
    std::vector<struct pollfd>  fds;
    // std::map<int, Client> clients; // map to manage the clients

    // ------------------- member functions: ----------------------- 
    // 1- initialize the serever socket.
    void    initializeServer();
    // 2- main loop for polling and handling events.
    void    pollLoop();
    // 3- handles events returned by poll().
    void    handleEvents();
    // 4- brodcasts a message to all clients in a channel.
    void    brodcastMessage(const std::string& message, const std::string& channel);
    // additional helper functions as needed (oziyada mn ras lhmq hh).
    // close all opne sockets and clean up resources:
    void    cleanUp();

public:
    // constructor:
    Server(int port, const std::string& password);

    // starts the server:
    void    start();

    // destructor:
    ~Server();

};

#endif