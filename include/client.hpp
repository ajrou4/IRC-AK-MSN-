/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: majrou <majrou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 00:46:31 by omakran           #+#    #+#             */
/*   Updated: 2024/05/27 23:32:13 by majrou           ###   ########.fr       */
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

class Client {
    //                          client socket file descriptor
    int                         fd;
    //                          client's informations
    std::string                 nickname;
    std::string                 username;
    std::string                 realname;
    std::vector<std::string>    channels;
    bool                        authenticated;
    std::vector<std::string>    messageQueue;

    // utility function to split the message into command and parameters.
    std::vector<std::string>    splitMessage(const std::string& message);

public:
    void                        handleNick(const std::string& nick);
    void                        handleUser(const std::string& user);
    void                        handleJoin(const std::string& channel);
    void                        handlePrivmsg(const std::string& target, const std::string& message);

    Client(int fd);
    ~Client();
    std::string                 getNick() const;
    std::string                 getUserName() const; // return the client's username.
    int                         getFd() const; // return the client's file descriptor.
    std::string                 getRealName() const;
    void                        setRealName(const std::string& realName);
    /* ########################################################
       ---------------- member functions: ---------------------
       ######################################################## */

    //                          internal message processing
    void                        handleMessage(const std::string& message);

    bool                        isAuthenticated(void) const;

    void                        setAuthenticated(bool authenticated = true);
    void                        newMessage(const std::string &message);
};

#endif
