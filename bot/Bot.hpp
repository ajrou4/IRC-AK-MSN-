/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 09:48:58 by codespace         #+#    #+#             */
/*   Updated: 2024/05/21 09:51:07 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_HPP
# define BOT_HPP

#include <iostream>
class Bot
{
    private:
        std::string _name;
    public:
        Bot();
        ~Bot();
        Bot(const Bot &copy);
        Bot &operator=(const Bot &copy);
        // accessors
        std::string getName() const;
};

#endif