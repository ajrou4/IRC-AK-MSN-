/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_channel_managment.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 18:49:45 by haguezou          #+#    #+#             */
/*   Updated: 2024/06/06 23:59:03 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "ircserver.hpp"

/* ----------------------------  KICK command ------------------------------
                A channel operator can kick out a user, 
                optionally providing a reason.
   ------------------------------------------------------------------------- */
//      removes a user from a channel.
void    Server::KICK(int socket, std::string kick) {
    Client& client = getClient(socket);
    std::string channelName, target, message;
    std::stringstream ss(kick);
    std::stringstream broadcast;
    
    ss >> channelName >> target >> std::ws; // get the channel name and target and get rid of the leading whitespace
    std::getline(ss, message, '\0'); // get the message
    if (channelName.empty() || target.empty()) {
        sendMessageCommand(socket, intro() + "461 KICK : Not enough parameters");
        return;
    }
    try {
        Channel& channel = getChannel(channelName);
        if (!channel.hasClient(socket)) { // if the client is not in the channel
            sendMessageCommand(socket, intro() + "442 " + channelName + " : You're not on that channel");
            return;
        }
        if (!channel.isOperator(socket)) { // if the client is not an operator
            sendMessageCommand(socket, intro() + "482 " + channelName + " : You're not a channel operator");
            return;
        }
        Client& targetClient = getClientByNick(target);
        if (!channel.hasClient(targetClient.getFd())) { // if the target is not in the channel
            sendMessageCommand(socket, intro() + "441 " + target + " " + channelName + " : They aren't on that channel");
            return;
        }

        broadcast << client.intro() << "KICK " << channelName << " " << target << " : " << message;
        channel.broadcastMessage(broadcast.str());
        channel.removeClient(targetClient.getFd());
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, intro() + "403 " + channelName + " : No such channel");
        return;
    }
}

/* ----------------------------  TOPIC command ------------------------------
                Without a topic, it shows the current topic.
                With a topic, it sets the new topic..
   -------------------------------------------------------------------------- */
//      Sets or gets the topic of a channel.
void    Server::TOPIC(int socket, std::string topic) {
    Client& client = getClient(socket);
    std::string channelName, newTopic;
    std::stringstream ss(topic);

    ss >> channelName >> std::ws; // get the channel name and get rid of the leading whitespace
    std::getline(ss, newTopic, '\0'); // get the new topic
    if (channelName.empty()) {
        sendMessageCommand(socket, intro() + "461 TOPIC : Not enough parameters");
        return;
    }
    try {
        Channel& channel = getChannel(channelName);
        if (!channel.hasClient(socket)) { // if the client is not in the channel
            sendMessageCommand(socket, intro() + "442 " + channelName + " : You're not on that channel");
            return;
        }
        if (newTopic.empty()) {
            sendMessageCommand(socket, intro() + "331 " + client.getNick() + " " + channelName + " : " + channel.getTopic());
            return;
        }
        if (channel.getMode(ToPic) && !channel.isOperator(socket)) { // if the client is not an operator
            sendMessageCommand(socket, intro() + "482 " + channelName + " : You're not a channel operator");
            return;
        }
        channel.setTopic(newTopic);
        channel.broadcastMessage(client.intro() + "TOPIC " + channelName + " : " + newTopic);
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, intro() + "403 " + channelName + " : No such channel");
    }
}

/* ----------------------------  INVITE command ------------------------------
                Used to invite a user to a channel
                they are not currently a member of.
   -------------------------------------------------------------------------- */
//      Invites a user to a channel.
void    Server::INVITE(int socket, std::string invite) {
    Client& client = getClient(socket);
    std::string target, channelName;
    std::stringstream ss(invite);
    ss >> target >> channelName; // get the target and channel name
    if (target.empty() || channelName.empty()) {
        sendMessageCommand(socket, intro() + "461 INVITE : Not enough parameters");
        return;
    }
    try {
        Client& targetClient = getClientByNick(target);
        Channel& channel = getChannel(channelName);
        if (!channel.hasClient(socket)) { // if the client is not in the channel
            sendMessageCommand(socket, intro() + "442 " + channelName + " : You're not on that channel");
            return;
        }
        if (channel.hasClient(targetClient.getFd())) { // if the target is already in the channel
            sendMessageCommand(socket, intro() + "443 " + target + " " + channelName + " : is already on channel");
            return;
        }
        sendMessageCommand(targetClient.getFd(), client.intro() + "INVITE " + target + " " + channelName);
        channel.addInv(targetClient.getFd());
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, intro() + "403 " + channelName + " : No such channel");
    }
}