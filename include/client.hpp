/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 00:46:31 by omakran           #+#    #+#             */
/*   Updated: 2024/05/29 19:04:05 by haguezou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <netinet/in.h> // for sockaddr_in
# include <unistd.h> // for close
# include <cstring> // for strerror
# include <cerrno> // for errno
# include <iostream>
# include <sstream>
# include <vector>
#include "Channel.hpp"

class Channel;
// class Server;
class Client {
    //                          client socket file descriptor
    int                         fd;
    std::string                 ip;
    std::string                 hostname;
    //                          buffer to store the incoming data.
    std::stringstream           inboundBuffer;
    //                          buffer to store the outgoing data.
    std::stringstream           outboundBuffer;
    //                          client's informations
    std::string                 nickname;
    std::string                 username;
    std::string                 realname;
    std::vector<std::string>    channels;
    //                          flags to check if the client is authenticated and registered.
    bool                        authenticated; 
    bool                        registered;
    std::vector<std::string>    messageQueue;

    // utility function to split the message into command and parameters.
    // std::vector<std::string>    splitMessage(const std::string& message);

public:
    Client(int fd, std::string ip, std::string hostname);
    ~Client();
    /* 
       ########################################################
       ---------------- member functions: ---------------------
       ######################################################## 
    */
    void                        handleNick(const std::string& nick);
    void                        handleUser(const std::string& user);
    void                        handleJoin(const std::string& channel);
    void                        handlePrivmsg(const std::string& target, const std::string& message);

    std::string                 getNick() const;
    std::string                 getUserName() const; // return the client's username.
    int                         getFd() const; // return the client's file descriptor.
    std::string                 getRealName() const;
    std::string                 getHostname() const;
    void                        setRealName(std::string realName);
    void                        setUserName(std::string userName);
    void                        setNick(const std::string& nick);
    void                        setRegistered(bool registered = true);

    // utility function to split the message into command and parameters.
    std::vector<std::string>    splitMessage(const std::string& message);

    //                          internal message processing
    void                        handleMessage(const std::string& message);

    bool                        isAuthenticated(void) const;
    bool                        outBoundReady(void) const;
    bool                        inboundReady(void) const;
    bool                        isRegistered(void) const;

    void                        setAuthenticated(bool authenticated = true);
    void                        newMessage(const std::string &message);
    void                        appendToInboundBuffer(std::string data);

    std::vector<std::string>    splitCommands(void);
};

#endif
