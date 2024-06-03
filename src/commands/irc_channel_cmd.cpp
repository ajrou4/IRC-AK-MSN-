/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_channel_cmd.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 18:40:25 by haguezou          #+#    #+#             */
/*   Updated: 2024/06/03 18:45:09 by haguezou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"
#include "client.hpp"

/* ----------------------------  LIST command ------------------------------
                    Shows channels and their topics.
   ------------------------------------------------------------------------- */
//      lists all channels or channels matching a certain pattern
void    Server::LIST(int socket, std::string) {
    Client  &client = getClient(socket);

    sendMessageCommand(socket, ":ircserver 321 " + client.getNick() + " Channel : Users Name");
    std::map<std::string, Channel*>::iterator it = channels.begin();
    for (; it != channels.end(); ++it) {
        std::stringstream ss; // create a stringstream to store the message
        Channel& channel = *it->second; // get the channel
        if (channel.getMode(Secret)) {
            continue; // skip secret channels
        }
        ss << ":ircserver 322 " << client.getNick() << " " << channel.getName() << " " << channel.getUsers().size() << " : " << channel.getTopic();
        sendMessageCommand(socket, ss.str());
    }
    sendMessageCommand(socket, ":ircserver 323 " + client.getNick() + " : End of /LIST");
}

/* ----------------------------  JOIN command --------------------------------
                    The user joins one or more channels,
                    optionally providing a key if required.
   ------------------------------------------------------------------------- */
//      joins a user to a channel.
void    Server::JOIN(int socket, std::string channelName) {
    Client& client = getClient(socket);

    std::string channel_name, channel_key;
    std::stringstream ss(channelName);
    ss >> channel_name >> std::ws; // get the channel name and get rid of the leading whitespace
    ss >> channel_key; // get the channel key
    if (channel_name.empty()){
        sendMessageCommand(socket, ":ircserver 461  JOIN :Not enough parameters");
        return;
    }
    if (channel_name[0] == '#') {
        channel_name = channel_name.substr(1); // remove the leading #
    }
    if (channel_name.size() < 1 || channel_name.size() > 20
        || channel_name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != std::string::npos
        || channel_name.find_first_of("0123456789_", 0, 1) == 0) {
            sendMessageCommand(socket, ":ircserver 403 " + channel_name + " : No such channel");
            return;
    }
    channel_name = "#" + channel_name; // add the leading #
    try {
        Channel& channel = getChannel(channel_name);
        channel_name = channel.getName();
        if (channel.getMode(Key) && (channel_key.empty() || channel_key != channel.getPassword())) {
            sendMessageCommand(socket, ":ircserver 475 " + channel_name + " : Cannot join channel (+k)");
            return;
        }
        if (channel.getMode(Limit) && channel.getUsers().size() >= channel.getMode(Limit)) {
            sendMessageCommand(socket, ":ircserver 471 " + channel_name + " : Cannot join channel (+l)");
            return;
        }
        if (channel.getMode(invit_ONLY) && !channel.hasClient(socket)) {
            sendMessageCommand(socket, ":ircserver 473 " + channel_name + " : Cannot join channel (+i)");
            return;
        }
        // if the client is not already in the channel, add them
        channel.addClient(socket);
        channel.removeInv(socket); // remove the client from the invite list
        if (channel.getUsers().size() == 1) {
            channel.addOperator(socket); // make the client an operator if they are the first in the channel
        }
        channel.broadcastMessage(":" + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname() + " JOIN " + channel_name);
        sendMessageCommand(socket, ":ircserver 332 " + client.getNick() + " " + channel_name + " : " + channel.getTopic());
        sendMessageCommand(socket, ":ircserver 353 " + client.getNick() + " = " + channel_name + " : " + client.getNick());
        sendMessageCommand(socket, ":ircserver 366 " + client.getNick() + " " + channel_name + channel.getModes());
    } catch (std::runtime_error& e) {
        createChannel(channel_name, channel_key);
        Channel& channel = getChannel(channel_name);
        channel.addClient(socket); // add the client to the channel
        channel.addOperator(socket); // make the client an operator if they are the first in the channel
        channel.broadcastMessage(":" + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname() + " JOIN " + channel_name);
        sendMessageCommand(socket, ":ircserver 331 " + client.getNick() + " " + channel_name + " :No topic is set");
        sendMessageCommand(socket, ":ircserver 353 " + client.getNick() + " = " + channel_name + " : " + client.getNick());
        sendMessageCommand(socket, ":ircserver 324 " + client.getNick() + " " + channel_name + " " + channel.getModes());
    }
}


/* ----------------------------  PRIVMSG command ------------------------------
                    Used for private communication with users 
                    or to send messages to channels.
   ---------------------------------------------------------------------------- */
//      Sends a private message to a user or channel.
void    Server::PRIVMSG(int socket, std::string privmsg) {
    Client& client = getClient(socket);
    std::string target, message;
    std::stringstream ss(privmsg);
    ss >> target >> std::ws; // get the target and get rid of the leading whitespace
    std::getline(ss, message, '\0'); // get the message
    if (target.empty()) {
        sendMessageCommand(socket, ":ircserver 411 " + client.getNick() + " :No recipient given");
        return;
    }
    if (message.empty()) {
        sendMessageCommand(socket, ":ircserver 412 " + client.getNick() + " :No text to send");
        return;
    }
    try {
        if (target[0] == '#') { // if the target is a channel
            Channel& channel = getChannel(target);
            if (!channel.hasClient(socket)) { // if the client is not in the channel
                sendMessageCommand(socket, ":ircserver 442 " + client.getNick() + " " + target + " :You're not on that channel");
                return;
            }
            if (channel.getMode(Moderated) && !channel.isOperator(socket) && !channel.hasPlusV(socket)) { // if the channel is moderated and the client is not an operator or voiced
                sendMessageCommand(socket, ":ircserver 404 " + client.getNick() + " " + target + " :Cannot send to channel");
                return;
            }
            channel.brodcastMessage(":" + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname() + " PRIVMSG " + "#" + target + " :" + message, socket);
        } else {
            Client& targetClient = getClientByNick(target);
            sendMessageCommand(targetClient.getFd(), ":" + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname() + " PRIVMSG " + target + " :" + message);
        }
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, ":ircserver 401 " + client.getNick() + " " + target + " :No such nick/channel");
    }
}

/* ----------------------------  PART command ------------------------------
                        The user leaves one or more channels.
   ----------------------------------------------------------------------- */
//       leaves a channel.
void    Server::PART(int socket, std::string part) {
    Client& client = getClient(socket);
    std::stringstream ss(part); // create a stringstream from the message
    std::string channelName;
    ss >> channelName; // get the channel name
    if (channelName.empty()) {
        sendMessageCommand(socket, ":ircserver 461 PART : Not enough parameters");
        return;
    }
    try {
        Channel& channel = getChannel(channelName);
        if (!channel.hasClient(socket)) {
            sendMessageCommand(socket, ":ircserver 442 " + channelName + " : You're not on that channel");
            return;
        }
        helperOperator(channel, client, *this); // check if the client is the only operator in the channel
        channel.broadcastMessage(":" + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname() + " PART " + part);
        channel.removeClient(socket);
    } catch (std::runtime_error& e) {
        sendMessageCommand(socket, ":ircserver 403 " + client.getNick() + " " + channelName + " :No such channel");
    } 
}

/* ----------------------------  QUIT command ------------------------------
                Ends the session with an optional message
            that is sent to all users in the channels the user was part of.
   ------------------------------------------------------------------------- */
//      disconnects from the server
void    Server::QUIT(int socket, std::string quit) {
    Client& client = getClient(socket);
    std::stringstream ss;
    ss << ":" + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname() << " QUIT :" << quit;
    sendMessageToClientChannels(socket, ss.str());
    removeClient(socket);
}
