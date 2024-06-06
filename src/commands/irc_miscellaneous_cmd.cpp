/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_miscellaneous_cmd.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 18:53:09 by haguezou          #+#    #+#             */
/*   Updated: 2024/06/06 04:25:36 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "ircserver.hpp"

/* ----------------------------  ISON command --------------------------------
                Returns a list of online users matching the given nicknames.
   --------------------------------------------------------------------------- */
//      checks if a user is online.
void    Server::ISON(int socket, std::string ison) {
    Client& client = getClient(socket);
    std::string nickname;
    std::vector<std::string> nicknames;
    std::stringstream   message;
    std::stringstream ss(ison);

    ss >> nickname;
    if (nickname.empty()) {
        sendMessageCommand(socket, intro() + "461 ISON : Not enough parameters");
        return;
    }
    message << intro() << "303 " << client.getNick() << " :";
    while (!ss.eof()) { // while the stringstream is not at the end
        try {
            Client& target = getClientByNick(nickname);
            message << " " << target.getNick();
        }
        catch (std::runtime_error& e) {
            message << " " << nickname;
        }
        ss >> nickname;
    }
    sendMessageCommand(socket, message.str());
}

/* ----------------------------  MODE command --------------------------------
            Can be used to set or check user modes (like operator status) 
            or channel modes (like moderation).
   --------------------------------------------------------------------------- */
//      sets or checks modes for a user or channel.
void    Server::MODE(int socket, std::string mode) {
    Client& client = getClient(socket);
    std::string target, modeStr, modeParams;
    std::stringstream ss(mode); // create a stringstream from the message
    ss >> target >> modeStr >> modeParams; // get the target, mode, and mode parameters
    if (target.empty()) {
        sendMessageCommand(socket, intro() + "461 MODE : Not enough parameters");
        return;
    }
    Channel *channel;
    try {
        channel = &getChannel(target); // try to get the channel
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, intro() + "403 MODE : No such channel"); // send an error message if the channel does not exist
        return;
    }
    if (mode.empty()) {
        sendMessageCommand(socket, intro() + "324 " + client.getNick() + " " + channel->getName() + " " + channel->getModes());
        return;
    }
    if (!channel->isOperator(socket) || !channel->hasClient(socket)) { // if the client is not an operator or not in the channel
        sendMessageCommand(socket, intro() + "482 MODE : You're not a channel operator");
        return;
    }
    bool addMode = true;
    if (modeStr[0] == '+' || modeStr[0] == '-') {
        addMode = modeStr[0] == '+'; // if the mode is a plus
        modeStr = modeStr.substr(1); // remove the plus
    }
    if (modeStr.length() != 1) { // if the mode is not a single character
        sendMessageCommand(socket, intro() + "472 MODE : is unknown mode char to me");
        return;
    }
    switch (modeStr[0]) {
        case 'i': // means set the invite-only mode
            channel->setMode(invit_ONLY, addMode);
            break;
        case 'm': // means set the moderated mode
            channel->setMode(Moderated, addMode);
            break;
        case 't': // means set the topic protection mode
            channel->setMode(ToPic, addMode);
            break;
        case 's': // means set the secret mode
            channel->setMode(Secret, addMode);
            break;
        case 'k': // means set the channel key
            if (modeParams.empty() && addMode) {
                sendMessageCommand(socket, intro() + "461 MODE : Not enough parameters");
                return;
            }
            channel->setMode(Key, addMode);
            if (addMode)
                channel->setPassword(modeParams);
            break;
        case 'v': // means give/take the voice privilege
            if (modeParams.empty()) {
                sendMessageCommand(socket, intro() + "461 MODE : Not enough parameters");
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
                sendMessageCommand(socket, intro() + "401 MODE : No such target");
            }
            break;
        case 'o': // means give/take channel operator privileges
            if (modeParams.empty()) {
                sendMessageCommand(socket, intro() + "461 MODE : Not enough parameters");
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
                sendMessageCommand(socket, intro() + "401 MODE : No such target");
            }
            break;
        case 'l': // means set the user limit
            if (modeParams.empty() && addMode) {
                sendMessageCommand(socket, intro() + "461 MODE : Not enough parameters");
                return;
            }
            if (modeParams.find_first_not_of("0123456789") != std::string::npos) { // if the mode parameters are not numbers
                sendMessageCommand(socket, intro() + "472 MODE : is unknown mode char to me");
                return;
            }
            if (addMode) {
                channel->setUserLimit(std::stoi(modeParams)); // stoi converts the string to an integer
            } else {
                channel->setUserLimit(0); // set the user limit to 0
            }
            break;
        default:
            sendMessageCommand(socket, intro() + "472 MODE : Unknown mode");
            break;
    }
    channel->broadcastMessage(client.intro() + " MODE " + channel->getName() + " "  + (addMode ? "+" : "-") + modeStr + " " + (modeStr == "k" ? "********" : modeParams));
}
