/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircImplementation.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 17:36:11 by omakran           #+#    #+#             */
/*   Updated: 2024/05/29 17:23:56 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"
#include "client.hpp"

/* ------------ this file will implement the logic of ARC --------------
|                        let's parss each command                       |
 --------------------------------------------------------------------- */ 

// numeric replies: https://modern.ircdocs.horse/#rplwelcome-001
void    Server::registerNewClient(int socket) {
    Client& client = getClient(socket);

    client.setRegistered(true);
    sendMessageCommand(socket, ":irc 001 " + client.getNick() + " :Welcome " + client.getNick() + "!" + client.getUserName() + " @ " + client.getHostname());
    sendMessageCommand(socket, ":irc 002 " + client.getNick() + " :Your host is ircserv, running version 1.0");
    sendMessageCommand(socket, ":irc 003 " + client.getNick() + " :This server was created today");
    sendMessageCommand(socket, ":irc 004 " + client.getNick() + " :ircserv 1.0 o o");
    sendMessageCommand(socket, ":irc 375 " + client.getNick() + " :- ircserv Message of the day - ");
    sendMessageCommand(socket, ":irc 372 " + client.getNick() + " :- Welcome to ircserv");
    sendMessageCommand(socket, ":irc 376 " + client.getNick() + " :End of /MOTD command.");
}

void    Server::sendMessageCommand(int socket, const std::string& message){
    Client& client = getClient(socket);
    client.newMessage(message);
    std::cout << ">>>>> Sending into socket: " << socket << " : " << message << std::endl;
    getPollfd(socket).events |= POLLOUT; // set the POLLOUT event for the socket
} 

void    Server::PASS(int socket, std::string pass) {
    Client& client = getClient(socket);
    // Check if the client is already authenticated
    if (client.isAuthenticated()) {
        sendMessageCommand(socket, ":irc 462 :You may not reregister");
        return;
    } else if (pass != password) {
        sendMessageCommand(socket, ":irc 464 :Password incorrect");
        return;
    } else {
        client.setAuthenticated(true);
    }
}

void    Server::NICK(int socket, std::string nickname) {
    Client& client = getClient(socket);
    // Check if the nickname is valid
    if (nickname.size() < 1 || nickname.size() > 9 || nickname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != std::string::npos) {
        sendMessageCommand(socket, ":irc 432 " + nickname + " :Erroneous nickname");
        return;
    }
    try {
        getClientByNick(nickname);
        sendMessageCommand(socket, ":irc 433 " + nickname + " :Nickname is already in use");
    } catch (std::runtime_error& e) {
        std::stringstream broadcastMessage;
        broadcastMessage << ":" << client.getNick() << " NICK " << nickname; // broadcast the new nickname
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
        sendMessageCommand(socket, ":irc 432 " + username + " :Erroneous username");
        return;
    }
    if (!realname.empty() && (realname.size() > 50 || realname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]{}\\|^`_- ") != std::string::npos)) {
        sendMessageCommand(socket, ":irc 501 " + realname + " : Invalid Realname");
        return;
    }
    std::stringstream broadcastMessage;
    broadcastMessage << ":" << client.getNick() << " !" << username << "@" + client.getHostname() << " USER " << params; // broadcast the new username
    client.setUserName(username);
    client.setRealName(realname);
    if (client.getNick() != "" && !client.isRegistered()) // if the client is not registered, register them
        registerNewClient(socket);
    sendMessageToClientChannels(socket, broadcastMessage.str()); // send the message to all the channels the client is in
}

void    Server::JOIN(int socket, std::string channelName) {
    static_cast<void>(socket);
    static_cast<void>(channelName);
    // Client& client = getClient(socket);
    // if (!client.isAuthenticated()) {
    //     sendMessageCommand(socket, ":irc.example.com 451 * :You have not registered");
    //     return;
    // }

    // if (channels.find(channelName) == channels.end()) {
    //     channels[channelName] = Channel(channelName);
    // }

    // channels[channelName].addUser(client);
    // std::string joinMessage = ":" + client.getNick() + "!" + client.getUserName() + "@irc.example.com JOIN :" + channelName;
    // //broadcastMessage(joinMessage, channelName);
    // sendMessageCommand(socket, joinMessage);
}

void    Server::LIST(int socket, std::string list) {
    static_cast<void>(socket);
    static_cast<void>(list);
    // sendMessageCommand(socket, ":server.name 321 Channel :Users Name");
    // for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
    //     const Channel& channel = it->second;
    //     sendMessageCommand(socket, ":server.name 322 " + channel.getName() + " " + std::to_string(channel.getUsers().size()) + " :" + channel.getTopic());
    // }
    // sendMessageCommand(socket, ":server.name 323 :End of LIST");
}

void    Server::PART(int socket, std::string part) {
    static_cast<void>(socket);
    static_cast<void>(part);
    // Client& client = getClient(socket);
    // std::string channelName = part.substr(part.find(' ') + 1);
    
    // std::map<std::string, Channel>::iterator it = channels.find(channelName);
    // if (it != channels.end()) {
    //     Channel& channel = it->second;
    //     channel.removeUser(client);
    //     sendMessageCommand(socket, ":" + client.getNick() + " PART " + channelName);
    // } else {
    //     sendMessageCommand(socket, ":server.name 403 " + client.getNick() + " " + channelName + " :No such channel");
    // }
}

void    Server::WHO(int socket, std::string who) {
    static_cast<void>(socket);
    static_cast<void>(who);
    // Client& client = getClient(socket);
    // std::string channelName = who.substr(who.find(' ') + 1);
    
    // std::map<std::string, Channel>::iterator it = channels.find(channelName);
    // if (it != channels.end()) {
    //     Channel& channel = it->second;
    //     std::vector<Client> users = channel.getUsers();
    //     for (std::vector<Client>::iterator It = users.begin(); It != users.end(); ++It) {
    //         Client& user = *It;
    //         sendMessageCommand(socket, ":server.name 352 " + client.getNick() + " " + channelName + " " + user.getUserName() + " " + " server.name " + user.getNick() + " H :0 " + user.getRealName());
    //     }
    //     sendMessageCommand(socket, ":server.name 315 " + client.getNick() + " " + channelName + " :End of WHO list");
    // } else {
    //     sendMessageCommand(socket, ":server.name 403 " + client.getNick() + " " + channelName + " :No such channel");
    // }
}

void    Server::WHOIS(int socket, std::string whois) {
    static_cast<void>(socket);
    static_cast<void>(whois);
    // Client& client = getClient(socket);
    // std::string nick = whois.substr(whois.find(' ') + 1);
    
    // Client* targetClient = getClientByNick(nick);
    // if (targetClient) {
    //     sendMessageCommand(socket, ":server.name 311 " + client.getNick() + " " + targetClient->getNick() + " " + targetClient->getUserName() + " " + " * :" + targetClient->getRealName());
    //     // Add more WHOIS information as necessary
    //     sendMessageCommand(socket, ":server.name 318 " + client.getNick() + " " + targetClient->getNick() + " :End of WHOIS list");
    // } else {
    //     sendMessageCommand(socket, ":server.name 401 " + client.getNick() + " " + nick + " :No such nick");
    // }
}

void    Server::PING(int socket, std::string ping) {
    std::string server = ping.substr(ping.find(' ') + 1);
    sendMessageCommand(socket, "PONG " + server);
}

void    Server::PRIVMSG(int socket, std::string privmsg) {
    static_cast<void>(socket);
    static_cast<void>(privmsg);
    // Client& client = getClient(socket);
    // size_t pos = privmsg.find(' ');
    // std::string target = privmsg.substr(0, pos);
    // std::string message = privmsg.substr(pos + 1);
    
    // if (target[0] == '#') {
    //     // Message to a channel
    //     std::map<std::string, Channel>::iterator it = channels.find(target);
    //     if (it != channels.end()) {
    //         Channel& channel = it->second;
    //         std::vector<Client> users = channel.getUsers();
    //         for (std::vector<Client>::iterator It = users.begin(); It != users.end(); ++It) {
    //             Client &user = *It;
    //             if (user.getFd() != socket) {
    //                 sendMessageCommand(user.getFd(), ":" + client.getNick() + " PRIVMSG " + target + " :" + message);
    //             }
    //         }
    //     } else {
    //         sendMessageCommand(socket, ":server.name 403 " + client.getNick() + " " + target + " :No such channel");
    //     }
    // } else {
    //     // Message to a user
    //     Client* targetClient = getClientByNick(target);
    //     if (targetClient) {
    //         sendMessageCommand(targetClient->getFd(), ":" + client.getNick() + " PRIVMSG " + target + " :" + message);
    //     } else {
    //         sendMessageCommand(socket, ":server.name 401 " + client.getNick() + " " + target + " :No such nick");
    //     }
    // }
}

void    Server::QUIT(int socket, std::string quit) {
    static_cast<void>(socket);
    static_cast<void>(quit);
    // Client& client = getClient(socket);
    // std::string message = quit.substr(quit.find(' ') + 1);
    
    // for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
    //     it->second.removeUser(client);
    // }
    
    // sendMessageCommand(socket, ":" + client.getNick() + " QUIT :" + message);
    // close(socket);
    // clients.erase(socket);
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
    static_cast<void>(socket);
    static_cast<void>(mode);
    // Client& client = getClient(socket);
    // std::istringstream iss(mode);
    // std::string channelName, modeString;
    // iss >> channelName >> modeString;
    // std::map<std::string, Channel>::iterator it = channels.find(channelName);
    // if (it != channels.end()) {
    //     Channel& channel = it->second;
    //     if (modeString.empty()) {
    //         sendMessageCommand(socket, ":server.name 324 " + client.getNick() + " " + channelName + " +" + channel.getMode());
    //     } else {
    //         sendMessageCommand(socket, ":server.name 472 " + client.getNick() + " " + channelName + " :is unknown mode char to me");
    //     }
    // } else {
    //     sendMessageCommand(socket, ":server.name 403 " + client.getNick() + " " + channelName + " :No such channel");
    // }
}
