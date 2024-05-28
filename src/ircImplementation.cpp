/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircImplementation.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 17:36:11 by omakran           #+#    #+#             */
/*   Updated: 2024/05/28 12:53:00 by haguezou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"
#include "client.hpp"

/* ------------ this file will implement the logic of ARC --------------
|                        let's parss each command                       |
 --------------------------------------------------------------------- */ 


void Server::sendCommand(int socket, const std::string& command){
        std::string comNewLine = command + "\r\n";
        size_t comlength= comNewLine.length();
        ssize_t byteSent= send(socket, comNewLine.c_str(), comlength, 0);
        if(byteSent == -1)
                std::cerr<< " Error sending command: "<< strerror(errno)<<std::endl;
        else if(byteSent < static_cast<ssize_t>(comlength))
                std::cerr << "Command is sent "<< comlength<<" but only sent : "<< byteSent<< " byte"<<std::endl;

}
    
void Server::PASS(int socket, const std::string& pass) {
    Client& client = getClient(socket);
    if (client.isAuthenticated()) {
        sendCommand(socket, ":irc.example.com 462 * :You may not reregister");
        return;
    }

    if (pass == this->password) {
        client.setAuthenticated(true);
        sendCommand(socket, ":irc.example.com 001 " + client.getNick() + " :Password accepted");
    } else {
        sendCommand(socket, ":irc.example.com 464 * :Password incorrect");
    }
}

void Server::NICK(int socket, const std::string& nickname) {
    Client& client = getClient(socket);
    client.handleNick(nickname);
    sendCommand(socket, ":irc.example.com 001 " + nickname + " :Nickname set to " + nickname);
}

void Server::USER(int socket, const std::string& params) {
    Client& client = getClient(socket);
    std::istringstream iss(params);
    std::string username, hostname, servername, realname;
    iss >> username >> hostname >> servername;
    realname = params.substr(params.find(':') + 1);

    client.handleUser(username);
    client.setRealName(realname);
    sendCommand(socket, ":irc.example.com 001 " + username + " :User registered");
}


void Server::JOIN(int socket, const std::string& channelName) {
    Client& client = getClient(socket);
    if (!client.isAuthenticated()) {
        sendCommand(socket, ":irc.example.com 451 * :You have not registered");
        return;
    }

    if (channels.find(channelName) == channels.end()) {
        channels[channelName] = Channel(channelName);
    }

    channels[channelName].addUser(client);
    std::string joinMessage = ":" + client.getNick() + "!" + client.getUserName() + "@irc.example.com JOIN :" + channelName;
    broadcastMessage(joinMessage, channelName);
    sendCommand(socket, joinMessage);
}
void Server::LIST(int socket, const std::string &list) {
    sendCommand(socket, ":server.name 321 Channel :Users Name");
    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        const Channel& channel = it->second;
        sendCommand(socket, ":server.name 322 " + channel.getName() + " " + std::to_string(channel.getUsers().size()) + " :" + channel.getTopic());
    }
    sendCommand(socket, ":server.name 323 :End of LIST");
}

void Server::PART(int socket, const std::string &part) {
    Client& client = getClient(socket);
    std::string channelName = part.substr(part.find(' ') + 1);
    
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it != channels.end()) {
        Channel& channel = it->second;
        channel.removeUser(client);
        sendCommand(socket, ":" + client.getNick() + " PART " + channelName);
    } else {
        sendCommand(socket, ":server.name 403 " + client.getNick() + " " + channelName + " :No such channel");
    }
}



void Server::WHO(int socket, const std::string &who) {
    Client& client = getClient(socket);
    std::string channelName = who.substr(who.find(' ') + 1);
    
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it != channels.end()) {
        Channel& channel = it->second;
        std::vector<Client> users = channel.getUsers();
        for (std::vector<Client>::iterator It = users.begin(); It != users.end(); ++It) {
            Client& user = *It;
            sendCommand(socket, ":server.name 352 " + client.getNick() + " " + channelName + " " + user.getUserName() + " " + " server.name " + user.getNick() + " H :0 " + user.getRealName());
        }
        sendCommand(socket, ":server.name 315 " + client.getNick() + " " + channelName + " :End of WHO list");
    } else {
        sendCommand(socket, ":server.name 403 " + client.getNick() + " " + channelName + " :No such channel");
    }
}


void Server::WHOIS(int socket, const std::string &whois) {
    Client& client = getClient(socket);
    std::string nick = whois.substr(whois.find(' ') + 1);
    
    Client* targetClient = getClientByNick(nick);
    if (targetClient) {
        sendCommand(socket, ":server.name 311 " + client.getNick() + " " + targetClient->getNick() + " " + targetClient->getUserName() + " " + " * :" + targetClient->getRealName());
        // Add more WHOIS information as necessary
        sendCommand(socket, ":server.name 318 " + client.getNick() + " " + targetClient->getNick() + " :End of WHOIS list");
    } else {
        sendCommand(socket, ":server.name 401 " + client.getNick() + " " + nick + " :No such nick");
    }
}



void Server::PING(int socket, const std::string &ping) {
    std::string server = ping.substr(ping.find(' ') + 1);
    sendCommand(socket, "PONG " + server);
}



void Server::PRIVMSG(int socket, const std::string &privmsg) {
    Client& client = getClient(socket);
    size_t pos = privmsg.find(' ');
    std::string target = privmsg.substr(0, pos);
    std::string message = privmsg.substr(pos + 1);
    
    if (target[0] == '#') {
        // Message to a channel
        std::map<std::string, Channel>::iterator it = channels.find(target);
        if (it != channels.end()) {
            Channel& channel = it->second;
            std::vector<Client> users = channel.getUsers();
            for (std::vector<Client>::iterator It = users.begin(); It != users.end(); ++It) {
                Client user = *It;
                if (user.getFd() != socket) {
                    sendCommand(user.getFd(), ":" + client.getNick() + " PRIVMSG " + target + " :" + message);
                }
            }
        } else {
            sendCommand(socket, ":server.name 403 " + client.getNick() + " " + target + " :No such channel");
        }
    } else {
        // Message to a user
        Client* targetClient = getClientByNick(target);
        if (targetClient) {
            sendCommand(targetClient->getFd(), ":" + client.getNick() + " PRIVMSG " + target + " :" + message);
        } else {
            sendCommand(socket, ":server.name 401 " + client.getNick() + " " + target + " :No such nick");
        }
    }
}



void Server::MODE(int socket, const std::string &mode) {
    Client& client = getClient(socket);
    std::istringstream iss(mode);
    std::string channelName, modeString;
    iss >> channelName >> modeString;
    
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it != channels.end()) {
        Channel& channel = it->second;
        if (modeString.empty()) {
            sendCommand(socket, ":server.name 324 " + client.getNick() + " " + channelName + " +" + channel.getMode());
        } else {
            sendCommand(socket, ":server.name 472 " + client.getNick() + " " + channelName + " :is unknown mode char to me");
        }
    } else {
        sendCommand(socket, ":server.name 403 " + client.getNick() + " " + channelName + " :No such channel");
    }
}


void Server::QUIT(int socket, const std::string &quit) {
    Client& client = getClient(socket);
    std::string message = quit.substr(quit.find(' ') + 1);
    
    for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
        it->second.removeUser(client);
    }
    
    sendCommand(socket, ":" + client.getNick() + " QUIT :" + message);
    close(socket);
    clients.erase(socket);
}


void Server::KICK(int socket, const std::string &kick) {
    Client& client = getClient(socket);
    std::istringstream iss(kick);
    std::string channelName, nick, comment;
    iss >> channelName >> nick >> comment;
    
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it != channels.end()) {
        Channel& channel = it->second;
        Client* targetClient = getClientByNick(nick);
        if (targetClient && channel.hasUser(*targetClient)) {
            channel.removeUser(*targetClient);
            sendCommand(socket, ":" + client.getNick() + " KICK " + channelName + " " + nick + " :" + comment);
        } else {
            sendCommand(socket, ":server.name 441 " + client.getNick() + " " + nick + " " + channelName + " :They aren't on that channel");
        }
    } else {
        sendCommand(socket, ":server.name 403 " + client.getNick() + " " + channelName + " :No such channel");
    }
}



void Server::INVITE(int socket, const std::string &invite) {
    Client& client = getClient(socket);
    std::istringstream iss(invite);
    std::string nick, channelName;
    iss >> nick >> channelName;
    
    Client* targetClient = getClientByNick(nick);
    if (targetClient) {
        sendCommand(targetClient->getFd(), ":" + client.getNick() + " INVITE " + nick + " :" + channelName);
        sendCommand(socket, ":server.name 341 " + client.getNick() + " " + nick + " " + channelName);
    } else {
        sendCommand(socket, ":server.name 401 " + client.getNick() + " " + nick + " :No such nick");
    }
}



void Server::TOPIC(int socket, const std::string &topic) {
    Client& client = getClient(socket);
    std::istringstream iss(topic);
    std::string channelName, newTopic;
    iss >> channelName >> newTopic;
    
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it != channels.end()) {
        Channel& channel = it->second;
        channel.setTopic(newTopic);
        sendCommand(socket, ":" + client.getNick() + " TOPIC " + channelName + " :" + newTopic);
    } else {
        sendCommand(socket, ":server.name 403 " + client.getNick() + " " + channelName + " :No such channel");
    }
}


void Server::ISON(int socket, const std::string &ison) {
    Client& client = getClient(socket);
    std::istringstream iss(ison);
    std::string nicks;
    std::getline(iss, nicks);
    
    std::istringstream nickStream(nicks);
    std::string nick;
    std::string onlineNicks;
    while (nickStream >> nick) {
        if (getClientByNick(nick)) {
            if (!onlineNicks.empty()) {
                onlineNicks += " ";
            }
            onlineNicks += nick;
        }
    }
    sendCommand(socket, ":server.name 303 " + client.getNick() + " :" + onlineNicks);
}
void  Server::MODE(int socket, const std::string &mode){
    Client& client = getClient(socket);
    std::istringstream iss(mode);
    std::string channelName, modeString;
    iss >> channelName >> modeString;
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it != channels.end()) {
        Channel& channel = it->second;
        if (modeString.empty()) {
            sendCommand(socket, ":server.name 324 " + client.getNick() + " " + channelName + " +" + channel.getMode());
        } else {
            sendCommand(socket, ":server.name 472 " + client.getNick() + " " + channelName + " :is unknown mode char to me");
        }
    } else {
        sendCommand(socket, ":server.name 403 " + client.getNick() + " " + channelName + " :No such channel");
    }
}
