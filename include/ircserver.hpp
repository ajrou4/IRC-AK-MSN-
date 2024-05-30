/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: majrou <majrou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 18:39:36 by omakran           #+#    #+#             */
/*   Updated: 2024/05/30 12:21:55 by majrou           ###   ########.fr       */
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
# include "Channel.hpp"

class Client;
class Channel;
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
    std::map<int, Client*>                  clients;
    //                                      map of channel names
    std::map<std::string, Channel*>          channels;
    //                                      map to store the command
    typedef void    (Server::*commandHandler)(int, std::string);
    std::map<std::string, commandHandler>   commands;

    // ------------------- member functions: -----------------------

    //              initialize the serever socket.
    void            initializeServer();
    //              handles events returned by poll().
    void            handleEvents();
    //              accept a new client connection.
    void            handleNewConnection();
    //              handle a message from client
    void            handleClientMessage(int client_fd);
    //              write message to a client
    void            WriteMsgToClient(int socket);
    //              additional helper functions as needed (oziyada mn ras lhmq hh).
    void            InithandleComands(void);
        
    //              close all opne sockets and clean up resources:
    void            cleanUp();

public:
    // constructor:
    Server(int port, const std::string& password);

    //              main loop for polling and handling events.
    void            pollLoop();

    struct pollfd&  getPollfd(int fd);
    Client&         getClient(int fd); // return the client object associated with the file descriptor.
    Client&         getClientByNick(const std::string& nick); // return the client object associated with the nickname.
    Channel&        getChannel(std::string& channel); // return the channel object associated with the channel name.
    void            sendMessageToClient(int client_fd, const std::string &message);
    void            commandsProcess(std::vector<std::string> cmds, int fd_client);
    void            sendMessageCommand(int socket, const std::string& message); // send a message to a client.
    void            sendMessageToClientChannels(int client_fd, const std::string &message);

    std::vector<Channel*>   getChannels(int client_fd);
    void                    createChannel(std::string channel_name, std::string password, std::string topic = "");

    //              register a new client.
    void            registerNewClient(int socket);
    //              CMDS
    void            CAP(int socket, std::string cap);// Client Capability Negotiation : https://tools.ietf.org/html/rfc2812#section-3.2.6 // https://ircv3.net/specs/extensions/capability-negotiation.html // https://modern.ircdocs.horse/ircv3.html#cap
    void            PASS(int socket, std::string pass);
    void            NICK(int socket, std::string nickname);
    void            USER(int socket, std::string user);
    void            LIST(int socket, std::string list);
    void            JOIN(int socket, std::string join);
    void            PART(int socket, std::string part);
    void            WHO(int socket, std::string who);
    void            WHOIS(int socket, std::string whois);
    void            PING(int socket, std::string ping);
    void            PRIVMSG(int socket, std::string privmsg);
    void            QUIT(int socket, std::string quit);
    void            KICK(int socket, std::string kick);
    void            INVITE(int socket, std::string invite);
    void            TOPIC(int socket, std::string topic);
    void            ISON(int socket, std::string ison);
    void            MODE(int socket, std::string mode);

    // destructor:
    ~Server();

};

#endif
