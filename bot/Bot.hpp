/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 09:48:58 by codespace         #+#    #+#             */
/*   Updated: 2024/06/01 18:57:41 by haguezou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
# define BOT_HPP

#include <iostream>
#include "../include/Channel.hpp"
#include "../include/client.hpp"

class Channel;
class Bot
{
    private:
        std::string _name;
        int         _port;
        int         _sockfd;
        struct sockaddr_in _servaddr;
        struct hostent *_server;
        std::vector<Channel*> _channels;
        std::vector<Client*> _clients;
    public:
        Bot();
        ~Bot();
        Bot(const Bot &copy);
        Bot &operator=(const Bot &copy);
        // accessors
        std::string getName() const;
        // memeber functions
        void stratSocket();
        void connectToServer();
        void getChannels(Channel &channel);
        void run();
};

#endif