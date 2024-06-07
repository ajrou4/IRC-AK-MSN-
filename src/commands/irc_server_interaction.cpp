/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_server_interaction.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 17:03:42 by haguezou          #+#    #+#             */
/*   Updated: 2024/06/07 02:55:06 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"
#include "client.hpp"

/* ---------------------------- PING command ------------------------------
    The server sends a PING message to check if the client is responsive.
    The client must respond with a PONG message.
   -----------------------------------------------------------------------*/

void    Server::PING(int socket, std::string ping) {
    sendMessageCommand(socket, intro() + "PONG " + ping);
}
