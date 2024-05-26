/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: majrou <majrou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 18:39:36 by omakran           #+#    #+#             */
/*   Updated: 2024/05/27 00:02:43 by majrou           ###   ########.fr       */
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
    //                                      map to store the command
    typedef void    (Server::*commandHandler)(int, std::string);
    std::map<std::string, commandHandler>   commands;

    // ------------------- member functions: ----------------------- 
    // 1-           initialize the serever socket.
    void            initializeServer();
    // 2-           handles events returned by poll().
    void            handleEvents();
    // 3-           accept a new client connection.
    void            handleNewConnection();
    // 4-           handle a message from client
    void            handleClientMessage(int client_fd);
    // 5-           broadcasts a message to all clients in a channel.
    void            broadcastMessage(const std::string& message, const std::string& channel);
        
    //              additional helper functions as needed (oziyada mn ras lhmq hh).
    void            InithandleComands(void);
        
    //              close all opne sockets and clean up resources:
    void            cleanUp();

public:
    // constructor:
    Server(int port, const std::string& password);

    //              main loop for polling and handling events.
    void            pollLoop();

    Client&         getClient(int fd);
    void            sendMessageToClient(int client_fd, const std::string &message);
    struct pollfd&  getPollfd(int fd);

    // here a function to register the new client.
    // CMD
    void            PASS(int socket, const std::string &pass);
    void            NICK(int socket, const std::string &nickname);
    void            USER(int socket, const std::string &user);
    void            LIST(int socket, const std::string &list);
    void            JOIN(int socket, const std::string &join);
    void            PART(int socket, const std::string &part);
    void            WHO(int socket, const std::string &who);
    void            WHOIS(int socket, const std::string &whois);
    void            PING(int socket, const std::string &ping);
    void            PRIVMSG(int socket, const std::string &privmsg);
    void            QUIT(int socket, const std::string &quit);
    void            KICK(int socket, const std::string &kick);
    void            INVITE(int socket, const std::string &invite);
    void            TOPIC(int socket, const std::string &topic);
    void            ISON(int socket, const std::string &ison);
    void            MODE(int socket, const std::string &mode);

    // destructor:
    ~Server();
    void sendCommand(int socket, const std::string& command);

};

#endif
