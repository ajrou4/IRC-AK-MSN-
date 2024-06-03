/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircImplementation.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 17:36:11 by omakran           #+#    #+#             */
/*   Updated: 2024/06/03 19:15:31 by haguezou         ###   ########.fr       */
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
    sendMessageCommand(socket, ":ircserver 001 " + client.getNick() + " :Welcome " + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname());
    sendMessageCommand(socket, ":ircserver 002 " + client.getNick() + " :Your host is ircserv, running version 1.0");
    sendMessageCommand(socket, ":ircserver 003 " + client.getNick() + " :This server was created " + creationTime);
    sendMessageCommand(socket, ":ircserver 004 " + client.getNick() + " : ircserver 1.0 o o");
    sendMessageCommand(socket, ":ircserver 005 " + client.getNick() + " : i,t,k,l,s tokens :are supported by this server");
    sendMessageCommand(socket, ":ircserver 375 " + client.getNick() + " :- ircserver Message of the day - ");
    sendMessageCommand(socket, ":ircserver 372 " + client.getNick() + " :- Welcome to our IRC server!");
    sendMessageCommand(socket, ":ircserver 372 " + client.getNick() + " :- Please follow the rules and have fun!");
    sendMessageCommand(socket, ":ircserver 376 " + client.getNick() + " :End of /MOTD command.");
}

void    Server::sendMessageCommand(int socket, const std::string& message){
    Client& client = getClient(socket);
    client.newMessage(message);
    std::cout << BOLDGREEN << ">>>>> Sending into socket " << socket << ": " << message << RESET << std::endl;
    getPollfd(socket).events |= POLLOUT; // set the POLLOUT event for the socket
}

// pass was here =====> src/commands/irc_pass.cpp

// nick was here =====> src/commands/irc_nick.cpp

// user was here =====> src/commands/irc_user.cpp



// ping was here =====> src/commands/irc_interaction.cpp


// list was here =====> src/commands/irc_channel_cmd.cpp

//join was here =====> src/commands/irc_channel_cmd.cpp

// privmsg was here =====> src/commands/irc_channel.cpp

// part was here =====> src/commands/irc_channel_cmd.cpp



// quit was here =====> src/commands/irc_interaction.cpp



// who was here =====> src/commands/irc_user_info.cpp   

// whois was here =====> src/commands/irc_user_info.cpp



// kick was here =====> src/commands/irc_channel_managment.cpp

// topic was here =====> src/commands/irc_channel_managment.cpp

// invite was here =====> src/commands/irc_channel_managment.cpp


// ison was here =====> src/commands/irc_miscellaneous_cmd.cpp

// mode was here =====> src/commands/irc_miscellaneous_cmd.cpp