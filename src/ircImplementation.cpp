/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircImplementation.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 17:36:11 by omakran           #+#    #+#             */
/*   Updated: 2024/06/02 05:05:49 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"
#include "client.hpp"
#include "Channel.hpp"

/* ------------ this file will implement the logic of ARC --------------
|                        let's parss each command                       |
 --------------------------------------------------------------------- */ 

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
    std::cout << ">>>>> Sending into socket " << socket << ": " << message << std::endl;
    getPollfd(socket).events |= POLLOUT; // set the POLLOUT event for the socket
} 

void    Server::PASS(int socket, std::string pass) {
    Client& client = getClient(socket);
    // Check if the client is already authenticated
    if (client.isAuthenticated()) {
        sendMessageCommand(socket, ":ircserver 462 :You may not reregister");
        return;
    } else if (pass != password) {
        sendMessageCommand(socket, ":ircserver 464 :Password incorrect");
        return;
    } else {
        client.setAuthenticated(true);
    }
}

void    Server::NICK(int socket, std::string nickname) {
    Client& client = getClient(socket);
    // Check if the nickname is valid
    if (nickname.size() < 1 || nickname.size() > 9 || nickname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\\[]{}|`_-^") != std::string::npos
        || nickname.find_first_of("0123456789-", 0, 1) == 0) {
            sendMessageCommand(socket, ":ircserver 432 " + nickname + " : Erroneous nickname");
            return;
    }
    try {
        getClientByNick(nickname);
        sendMessageCommand(socket, ":ircserver 433 " + nickname + " : Nickname is already in use");
    } catch (std::runtime_error& e) {
        std::stringstream broadcastMessage;
        broadcastMessage << ":" + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname() << " NICK " <<nickname; // broadcast the new nickname
        if (client.getUserName() != "" && !client.isRegistered()) // if the client is not registered, register them
            registerNewClient(socket);
        client.setNick(nickname);
        sendMessageToClientChannels(socket, broadcastMessage.str()); // send the message to all the channels the client is in
    }
}

void    Server::USER(int socket, std::string params) {
    Client& client = getClient(socket);
    std::string username, realname, skipChar;
    std::istringstream iss(params);
    iss >> username >> std::ws; // get the username and get rid of the leading whitespace
    std::getline(iss, skipChar, ':'); // get rid of the colon
    iss >> std::ws; // get rid of the leading whitespace
    std::getline(iss, realname, '\0');
    //                                          https://modern.ircdocs.horse/#userlen-parameter
    if (username.size() < 1 || username.size() > 12
        || username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != std::string::npos
        || username.find_first_of("0123456789", 0, 1) == 0) {
            sendMessageCommand(socket, ":ircserver 432 " + username + " : Erroneous username");
            return;
    }
    if (!realname.empty() && (realname.size() > 50 || realname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]{}\\|^`_- ") != std::string::npos)) {
        sendMessageCommand(socket, ":ircserver 501 " + realname + " : Invalid Realname");
        return;
    }
    std::stringstream broadcastMessage;
    broadcastMessage << ":" << client.getNick() << "!" << username << "@" + client.getHostname() << " USER " << params; // broadcast the new username
    client.setUserName(username);
    client.setRealName(realname);
    if (client.getNick() != "" && !client.isRegistered()) // if the client is not registered, register them
        registerNewClient(socket);
    sendMessageToClientChannels(socket, broadcastMessage.str()); // send the message to all the channels the client is in
}

void    Server::JOIN(int socket, std::string channelName) {
    Client& client = getClient(socket);

    std::string channel_name, channel_key;
    std::istringstream iss(channelName);
    iss >> channel_name >> std::ws; // get the channel name and get rid of the leading whitespace
    iss >> channel_key; // get the channel key
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

void    Server::WHO(int socket, std::string who) {
    Client& client = getClient(socket);
    std::stringstream ss(who);
    std::string mask;
    ss >> mask;
    if (mask.empty()) {
        sendMessageCommand(socket, ":ircserver 431 " + client.getNick() + " :No nickname given");
        return;
    }
    try {
        Channel& channel = getChannel(mask);
        const std::vector<int>& users = channel.getUsers();
        for (size_t i = 0; i < users.size(); i++) {
            Client& user = getClient(users[i]);
            std::string mode = "H"; // set the mode to H for all users
            if (channel.isOperator(user.getFd()))
                mode += "@"; // add @ for operators
            std::stringstream msg; // create a stringstream to store the message
            msg << ":ircserver 352 " << client.getNick() << " " << channel.getName() << " " << user.getUserName() << " " << user.getHostname() << " " << "*" << " " << user.getNick() << " " << mode << " " << ":0 " << user.getRealName();
            sendMessageCommand(socket, msg.str());   
        }
        sendMessageCommand(socket, ":ircserver 315 " + client.getNick() + " " + mask + " :End of /WHO list");
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, ":ircserver 403 " + client.getNick() + " " + mask + " :No such channel"); // send an error message if the channel does not exist
    }
}

void    Server::WHOIS(int socket, std::string whois) {
    Client& client = getClient(socket);
    std::stringstream ss(whois);
    std::string mask;
    ss >> mask;
    if (mask.empty()) {
        sendMessageCommand(socket, ":ircserver 431 " + client.getNick() + " :No nickname given");
        return;
    }
    try {
        Client &target = getClientByNick(mask);
        std::stringstream msg;
        msg << ":ircserver 311 " << client.getNick() << " " << target.getNick() << " " << target.getUserName() << " " << target.getHostname() << " * :" << target.getRealName();
        sendMessageCommand(socket, msg.str());
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, ":ircserver 401 " + client.getNick() + " " + mask + " :No such nick/channel"); // send an error message if the channel does not exist
    }
}

void    Server::PING(int socket, std::string ping) {
    std::string server = ping.substr(ping.find(' ') + 1);
    sendMessageCommand(socket, ":ircserver PONG " + server);
}

void    Server::PRIVMSG(int socket, std::string privmsg) {
    Client& client = getClient(socket);
    std::string target, message;
    std::istringstream iss(privmsg);
    iss >> target >> std::ws; // get the target and get rid of the leading whitespace
    std::getline(iss, message, '\0'); // get the message
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
            channel.brodcastMessage(":" + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname() + " PRIVMSG " + target + " :" + message, socket);
        } else {
            Client& targetClient = getClientByNick(target);
            sendMessageCommand(targetClient.getFd(), ":" + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname() + " PRIVMSG " + target + " :" + message);
        }
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, ":ircserver 401 " + client.getNick() + " " + target + " :No such nick/channel");
    }
}

void    Server::QUIT(int socket, std::string quit) {
    Client& client = getClient(socket);
    std::stringstream ss;
    ss << ":" + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname() << " QUIT :" << quit;
    sendMessageToClientChannels(socket, ss.str());
    removeClient(socket);
}

void    Server::KICK(int socket, std::string kick) {
    static_cast<void>(socket);
    static_cast<void>(kick);
    // Client& client = getClient(socket);
    // std::istringstream iss(kick);
    // std::string channelName, nick, comment;
    // iss >> channelName >> nick >> comment;
    
    // std::map<std::string, Channel>::iterator it = channels.find(channelName);
    // if (it != channels.end()) {
    //     Channel& channel = it->second;
    //     Client* targetClient = getClientByNick(nick);
    //     if (targetClient && channel.hasUser(*targetClient)) {
    //         channel.removeUser(*targetClient);
    //         sendMessageCommand(socket, ":" + client.getNick() + " KICK " + channelName + " " + nick + " :" + comment);
    //     } else {
    //         sendMessageCommand(socket, ":server.name 441 " + client.getNick() + " " + nick + " " + channelName + " :They aren't on that channel");
    //     }
    // } else {
    //     sendMessageCommand(socket, ":server.name 403 " + client.getNick() + " " + channelName + " :No such channel");
    // }
}



void    Server::INVITE(int socket, std::string invite) {
    static_cast<void>(socket);
    static_cast<void>(invite);
    // Client& client = getClient(socket);
    // std::istringstream iss(invite);
    // std::string nick, channelName;
    // iss >> nick >> channelName;
    
    // Client* targetClient = getClientByNick(nick);
    // if (targetClient) {
    //     sendMessageCommand(targetClient->getFd(), ":" + client.getNick() + " INVITE " + nick + " :" + channelName);
    //     sendMessageCommand(socket, ":server.name 341 " + client.getNick() + " " + nick + " " + channelName);
    // } else {
    //     sendMessageCommand(socket, ":server.name 401 " + client.getNick() + " " + nick + " :No such nick");
    // }
}



void    Server::TOPIC(int socket, std::string topic) {
    static_cast<void>(socket);
    static_cast<void>(topic);
    // Client& client = getClient(socket);
    // std::istringstream iss(topic);
    // std::string channelName, newTopic;
    // iss >> channelName >> newTopic;
    
    // std::map<std::string, Channel>::iterator it = channels.find(channelName);
    // if (it != channels.end()) {
    //     Channel& channel = it->second;
    //     channel.setTopic(newTopic);
    //     sendMessageCommand(socket, ":" + client.getNick() + " TOPIC " + channelName + " :" + newTopic);
    // } else {
    //     sendMessageCommand(socket, ":server.name 403 " + client.getNick() + " " + channelName + " :No such channel");
    // }
}


void    Server::ISON(int socket, std::string ison) {
    static_cast<void>(socket);
    static_cast<void>(ison);
    // Client& client = getClient(socket);
    // std::istringstream iss(ison);
    // std::string nicks;
    // std::getline(iss, nicks);
    
    // std::istringstream nickStream(nicks);
    // std::string nick;
    // std::string onlineNicks;
    // while (nickStream >> nick) {
    //     if (getClientByNick(nick)) {
    //         if (!onlineNicks.empty()) {
    //             onlineNicks += " ";
    //         }
    //         onlineNicks += nick;
    //     }
    // }
    // sendMessageCommand(socket, ":server.name 303 " + client.getNick() + " :" + onlineNicks);
}

void    Server::MODE(int socket, std::string mode){
    Client& client = getClient(socket);
    Channel *channel;

    std::string target, modeStr, modeParams;
    std::istringstream iss(mode); // create a stringstream from the message
    iss >> target >> modeStr >> modeParams; // get the target, mode, and mode parameters
    if (target.empty()) {
        sendMessageCommand(socket, ":ircserver 461 " + client.getNick() + " MODE :Not enough parameters");
        return;
    }
    try {
        channel = &getChannel(target); // try to get the channel
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, ":ircserver 403 " + client.getNick() + " " + target + " :No such channel"); // send an error message if the channel does not exist
        return;
    }
    if (mode.empty()) {
        sendMessageCommand(socket, ":ircserver 324 " + client.getNick() + " " + target + " " + channel->getModes());
        return;
    }
    if (!channel->isOperator(socket) || !channel->hasClient(socket)) { // if the client is not an operator or not in the channel
        sendMessageCommand(socket, ":ircserver 482 " + client.getNick() + " :You're not a channel operator");
        return;
    }
    bool addMode = true;
    if (modeStr[0] == '+') {
        addMode = true; // set addMode to true if the mode is a plus
        modeStr = modeStr.substr(1); // remove the plus
    }
    if (modeStr.length() != 1) { // if the mode is not a single character
        sendMessageCommand(socket, ":ircserver 472 " + client.getNick() + " " + modeStr + " :is unknown mode char to me");
        return;
    }
    switch (modeStr[0]) {
        case 'o': // means give/take channel operator privileges
            if (modeParams.empty()) {
                sendMessageCommand(socket, ":ircserver 461 " + client.getNick() + " MODE :Not enough parameters");
                return;
            }
            try {
                Client& targetClient = getClientByNick(modeParams);
                if (addMode) { // if the mode is a plus
                    channel->addOperator(targetClient.getFd()); // add the client as an operator
                } else {
                    channel->removeOperator(targetClient.getFd()); // remove the client as an operator
                }
            }
            catch (std::runtime_error& e) {
                sendMessageCommand(socket, ":ircserver 401 " + client.getNick() + " " + modeParams + " :No such nick/channel");
            }
            break;
        case 'v': // means give/take the voice privilege
            if (modeParams.empty()) {
                sendMessageCommand(socket, ":ircserver 461 " + client.getNick() + " MODE :Not enough parameters");
                return;
            }
            try {
                Client& targetClient = getClientByNick(modeParams);
                if (addMode) { // if the mode is a plus
                    channel->addPlusV(targetClient.getFd()); // add the client as a voiced user
                } else {
                    channel->removePlusV(targetClient.getFd()); // remove the client as a voiced user
                }
            }
            catch (std::runtime_error& e) {
                sendMessageCommand(socket, ":ircserver 401 " + client.getNick() + " " + modeParams + " :No such nick/channel");
            }
            break;
        case 'k': // means set the channel key
            if (modeParams.empty()) {
                sendMessageCommand(socket, ":ircserver 461 " + client.getNick() + " MODE :Not enough parameters");
                return;
            }
            channel->setMode(Key, addMode);
            if (addMode) {
                channel->setPassword(modeParams);
            } else {
                channel->setPassword("");
            }
            break;
        case 'l': // means set the user limit
            if (modeParams.empty() && addMode) {
                sendMessageCommand(socket, ":ircserver 461 " + client.getNick() + " MODE :Not enough parameters");
                return;
            }
            if (addMode) {
                channel->setUserLimit(std::stoi(modeParams)); // stoi converts the string to an integer
            } else {
                channel->setUserLimit(0); // set the user limit to 0
            }
            break;
        case 'i': // means set the invite-only mode
            channel->setMode(invit_ONLY, addMode);
            break;
        case 'm': // means set the moderated mode
            channel->setMode(Moderated, addMode);
            break;
        case 's': // means set the secret mode
            channel->setMode(Secret, addMode);
            break;
        case 't': // means set the topic protection mode
            channel->setMode(ToPic, addMode);
            break;
        default:
            sendMessageCommand(socket, ":ircserver 472 " + client.getNick() + " " + modeStr + " :is unknown mode char to me");
            break;
    }
    channel->broadcastMessage(":" + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname() + " MODE " + channel->getName() + " "  );
}
