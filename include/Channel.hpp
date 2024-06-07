/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/26 10:50:47 by majrou            #+#    #+#             */
/*   Updated: 2024/06/06 23:50:32 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define  CHANNEL_HPP

# include <iostream>
# include <vector> 
# include <algorithm>
# include <sys/socket.h> 
# include <sys/types.h>
# include <netinet/in.h>
# include <fcntl.h> 
# include <unistd.h> 
# include <arpa/inet.h>
# include <poll.h>
# include <csignal>
# include <string>
# include <netdb.h>
# include <cstring>
# include "client.hpp"
# include "ircserver.hpp"

class Client; // we use forward declaration to avoid circular dependency (thats means we include client.hpp in Channel.cpp because we use Client class in Channel.cpp and Client is not yet defined in Channel.hpp)
class Server;

enum ChannelMode
{
    invit_ONLY = 1,
    ToPic = 2,
    Key = 3, // means password
    Limit = 4, // means user limit
    Secret = 5, // means hidden
    Moderated = 6, // means only operator can send message
 };

class Channel
{
    private:
        std::string                 name;
        std::string                 key;
        std::string                 password;
        std::string                 topic;
        int                         userLimit;
        bool                        inviteOnly;
        std::vector<std::string >   inviteUser2;
        std::vector<Client*>         oper;
        int                         mode;
        std::vector<int>            clients;
        std::vector<int>            invites;
        std::vector<int>            operators;
        Server                      *server; // for server
        std::vector<int>            plusVoices;

    public:
            Channel();
            Channel(std::string name, std::string password, Server *server);
            Channel(std::string const &chName);
            Channel(const Channel &src);
            Channel& operator=(const Channel &src);
            ~Channel();

            void                    addOperator(int socket);
            
            std::string const       &getName()const;
            const std::string       getPassword() const;
            void                    setPassword(const std::string password);
            void                    setMode(ChannelMode KEY, bool value);
            bool                    getMode(ChannelMode KEY)const;
            
            //                      void kickUser(const std::string &userName);
            void                    inviteUser(std::string userName);
            void                    setTopic(std::string topic);
            //                      void joinChannel(Client& client);

            const std::vector<int>  &getUsers()const;
            const std::string       &getTopic() const;

            bool                    hasClient(int fd) const;
            void                    addClient(int fd);
            void                    removeInv(int fd);
            bool                    isOperator(int fd) const;
            std::string             getModes() const; // return the channel modes as a string.
            void                    broadcastMessage(std::string message); // send a message to all clients in the channel.
            void                    brodcastMessage(std::string message, int fd); // send a message to all clients in the channel except the sender.
            void                    removeClient(int fd); // remove a client from the channel.
            void                    removeOperator(int fd); // remove a client from the operator list.
            bool                    hasPlusV(int fd);
            void                    addPlusV(int fd);
            void                    removePlusV(int fd);
            void                    setUserLimit(int limit);
            void                    addInv(int fd);
            bool                    hasInvet(int fd) const;
            int                     getCountOperator() const;
            int                     getLimit() const;
            int                     getCountClient() const;
            std::string             getClientsNicks() const;
};

void                                helperOperator(Channel &channel, Client &client, Server &server); // helper function to add operator to the channel.

#endif
