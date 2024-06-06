/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircImplementation.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 17:36:11 by omakran           #+#    #+#             */
/*   Updated: 2024/06/06 01:21:46 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"
#include "client.hpp"
#include "Channel.hpp"
#include "Colors.hpp"

// numeric replies: https://modern.ircdocs.horse/#rplwelcome-001
void    Server::registerNewClient(int socket) {
    Client& client = getClient(socket);

    client.setRegistered(true);
    sendMessageCommand(socket, intro() + "001 " + client.getNick() + " :Welcome " + client.getIdentifierNetwork());
    sendMessageCommand(socket, intro() + "002 " + client.getNick() + " :Your host is ircserv, running version 1.0");
    sendMessageCommand(socket, intro() + "003 " + client.getNick() + " :This server was created " + creationTime);
    sendMessageCommand(socket, intro() + "004 " + client.getNick() + " : ircserver 1.0 o o");
    sendMessageCommand(socket, intro() + "005 " + client.getNick() + " : i,t,k,l,s tokens :are supported by this server");
    sendMessageCommand(socket, intro() + "375 " + client.getNick() + " :- ircserver Message of the day - ");
    sendMessageCommand(socket, intro() + "372 " + client.getNick() + " :- Welcome to our IRC server!");
    sendMessageCommand(socket, intro() + "372 " + client.getNick() + " :- Please follow the rules and have fun!");
    sendMessageCommand(socket, intro() + "376 " + client.getNick() + " :End of /MOTD command.");
}

void    Server::sendMessageCommand(int socket, const std::string& message){
    Client& client = getClient(socket);
    client.newMessage(message);
    std::cout << BOLDGREEN << ">>>>> Sending into socket " << RESET << message << std::endl;
    getPollfd(socket).events |= POLLOUT; // set the POLLOUT event for the socket
}
