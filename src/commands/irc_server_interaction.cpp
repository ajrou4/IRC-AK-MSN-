/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_server_interaction.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 17:03:42 by haguezou          #+#    #+#             */
/*   Updated: 2024/06/06 01:26:47 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"
#include "client.hpp"

/* ---------------------------- PING command ------------------------------
    The server sends a PING message to check if the client is responsive.
    The client must respond with a PONG message.
   -----------------------------------------------------------------------*/
//      checks the connection between the client and the server.
void    Server::PING(int socket, std::string ping) {
    sendMessageCommand(socket, intro() + "PONG " + ping);
}
