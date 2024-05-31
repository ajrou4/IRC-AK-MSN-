/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 18:39:05 by omakran           #+#    #+#             */
/*   Updated: 2024/05/31 18:21:03 by haguezou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"
#include "Channel.hpp"

Server::Server(int port, const std::string& password) : port(port), password(password) {
    initializeServer();
    InithandleComands();
}

Server::~Server() {
    cleanUp();
}

// initiale the command
void    Server::InithandleComands(void) {
    commands["PASS"] = &Server::PASS;
    commands["NICK"] = &Server::NICK;
    commands["USER"] = &Server::USER;
    commands["LIST"] = &Server::LIST;
    commands["JOIN"] = &Server::JOIN;
    commands["PART"] = &Server::PART;
    commands["WHO"] = &Server::WHO;
    commands["WHOIS"] = &Server::WHOIS;
    commands["PING"] = &Server::PING;
    commands["PRIVMSG"] = &Server::PRIVMSG;
    commands["QUIT"] = &Server::QUIT;
    commands["KICK"] = &Server::KICK;
    commands["INVITE"] = &Server::INVITE;
    commands["TOPIC "] = &Server::TOPIC;
    commands["ISON"] = &Server::ISON;
    commands["MODE"] = &Server::MODE;
}

void    Server::initializeServer() {
    struct sockaddr_in  server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation error: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // set socket options
    int opt = 1; // this value will be used as the option value for the function
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) { // SO_REUS: allow the socket to bind a port that is in a TIME_WAIT,
        std::cerr << "Setsockopt error: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // set the socket to non-blocking mode
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Fcntl error: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // bind the socket to the address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind error: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections
    if (listen(server_fd, SOMAXCONN) < 0) {
        std::cerr << "Listen error: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // add the server socket to the polling list
    struct pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN | POLLERR | POLLHUP; // events to monitor: POLLIN: there's data to read, POLLERR: there's an error, POLLHUP: the client disconnected
    fds.push_back(server_pollfd);

    std::cout << "Server started on 0.0.0.0 : " << port << std::endl;
}

// main loop of the server:
void    Server::pollLoop() {
    while (true) {
        int poll_count = poll(fds.data(), fds.size(), -1); // wait indefinitely for events
        if (poll_count < 0) {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        handleEvents();
    }
}

void    Server::handleEvents() {
    // check if the event is ready for reading.
    if (fds[0].revents & POLLIN)
        handleNewConnection();
    // iterate over all monitored file descriptors.
    for (size_t i = 1; i < fds.size(); i++)
    {
        if (fds[i].revents & (POLLERR | POLLHUP)) // if there's an error or the client disconnected
            QUIT(fds[i].fd, "Client disconnected");
        else if (fds[i].revents | POLLIN) // if there's data to read
            handleClientMessage(fds[i].fd);
        else if (fds[i].revents | POLLOUT)  // if there's data to write
            WriteMsgToClient(fds[i].fd);
    }
}

// write message to a client:
void    Server::WriteMsgToClient(int socket)  {
    Client& client = getClient(socket);
    // if thre's no data to send, return
    if (!client.outBoundReady())
        return;
    std::string message = client.getOutboundBuffer(); // get the message to send
    ssize_t bytes_sent; // number of bytes sent
    if ((bytes_sent = send(socket, message.c_str(), message.size(), 0)) <= 0) {
        std::cerr << "Send error: " << strerror(errno) << std::endl;
        return;
    }
    client.advOutboundBuffer(bytes_sent);
    if (!client.outBoundReady()) {
        struct pollfd &client_pollfd = getPollfd(socket);
        client_pollfd.events &= ~POLLOUT; // mark the client as not ready for writing
    }
}

// accept a new client connection:
void    Server::handleNewConnection() {
    struct sockaddr_in  client_addr; // client address structure.
    socklen_t           client_len = sizeof(client_addr); // lenght of the client address structure.
    //                  accept the new connection.
    int                 client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

    if (client_fd < 0) {
        std::cerr << "Accept error: " << strerror(errno) << std::endl;
        return;
    }
    // set the new client socket to non-blocking mode.
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Fcntl error: " << strerror(errno) << std::endl;
        close (client_fd);
        return;
    }

    // add the new client to the list of file descriptors to poll
    struct pollfd       client_pollfd;
    client_pollfd.fd = client_fd; // client file descriptor.
    client_pollfd.events = POLLIN | POLLERR | POLLHUP; // events to monitor.
    fds.push_back(client_pollfd); // add to the list of poll file descriptors.
    std::string clientIp = inet_ntoa(client_addr.sin_addr); // client IP address.
    clients[client_fd] = new Client(client_fd, clientIp, clientIp); // create a new client object.

    std::cout << "New connection from " << clientIp << std::endl;
}

// handle a message from a client:
void    Server::handleClientMessage(int client_fd) {
    char    buffer[4096]; // buffer to reading data.
    int     bytes_read;
    Client& client = getClient(client_fd);
    if ((bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) <= 0) {
        return;
    }
    else
    {
        buffer[bytes_read] = '\0'; // null terminate the buffer.
    }
    std::string data = std::string(buffer, bytes_read); // convert the buffer to a string.
    client.appendToInboundBuffer(data); // append the data to the client's inbound buffer.
    if (client.inboundReady()) 
    {
        std::vector<std::string> commands = client.splitCommands();
        commandsProcess(commands, client_fd); // process the commands.
    }
    else{
        std::cout << ">>>>> inboundReady is faild : no command detected !" << std::endl;
        exit(EXIT_FAILURE);
    }
}

Client& Server::getClient(int fd) {
    std::map<int, Client*>::iterator it = clients.find(fd); // find the client object associated with the file descriptor.
    if (it == clients.end()) {
        throw std::runtime_error("Client not found for fd: " + std::to_string(fd));
    }
    return *it->second; // return the client object.
}

struct pollfd& Server::getPollfd(int fd) {
    // find the pollfd object associated with the file descriptor.
    for (size_t i = 0; i < fds.size(); ++i) {
        if (fds[i].fd == fd) {
            return fds[i];
        }
    }
    throw std::runtime_error("Pollfd not found for fd: " + std::to_string(fd));
}

void    Server::commandsProcess(std::vector<std::string> cmds, int fd_client) {
    std::cout << ">>>>> command Proccess => Recieved from socket: " << fd_client << ": " << cmds.size() << " commands" << std::endl;
    Client& client = getClient(fd_client);
    std::vector<std::string>::iterator it = cmds.begin();
    while (it != cmds.end()) {
        std::cout << "<<<<< Recieved from socket: " << fd_client << ": " << *it << std::endl;
        std::string command_name;
        std::string command_params;
        std::stringstream ss(*it);
        
        ss >> command_name >> std::ws;  // extract the command name
        std::transform(command_name.begin(), command_name.end(), command_name.begin(), ::toupper); // convert the command name to uppercase
        std::getline(ss, command_params, '\0'); // extract the command parameters
        if (command_name != "PASS" && !client.isAuthenticated()) { // if the client is not authenticated
            sendMessageCommand(fd_client, ":irc 451 :You have not registered");
        } else if (command_name != "PASS" && command_name != "NICK" && command_name != "USER" && !client.isRegistered()) {
            sendMessageCommand(fd_client, ":irc 451 :You have not registered");
        } else if (commands.find(command_name) == commands.end()) {
            sendMessageCommand(fd_client, ":irc 421 " + command_name + " : Unknown command");
        } else if (command_name == "QUIT") {
            QUIT(fd_client, command_params);
        } else {
            (this->*commands[command_name])(fd_client, command_params); // call the command handler
        }
        it++;
    }
}

void    Server::sendMessageToClientChannels(int client_fd, const std::string &message) {
    //Client& client = getClient(client_fd); // retrieve the client object associated with client_fd
    std::vector<Channel*> channels = getChannels(client_fd);
    for (size_t i = 0; i < channels.size(); i++) {
        channels[i]->brodcastMessage(message, client_fd); // broadcast the message to the channel
    }
}

std::vector<Channel*>   Server::getChannels(int client_fd) {
   // Client& client = getClient(client_fd); // retrieve the client object associated with client_fd
    std::vector<Channel*> result;
    std::map<std::string, Channel*>::iterator it = channels.begin();
    for (; it != channels.end(); it++) {
        if (it->second->hasClient(client_fd)) {
            result.push_back(it->second);
        }
    }
    return result;
}

Client  &Server::getClientByNick(const std::string& nick) {
    // find the client object associated with the nickname.
    std::map<int, Client*>::iterator it = clients.begin();
    for (; it != clients.end() && it->second->getNick() != nick ; it++)
        ; // do nothing
    if (it == clients.end()) {
        throw std::runtime_error("Client not found for nick: " + nick);
    }
    return *it->second; // return the client object.
}

Channel &Server::getChannel(std::string& channel) {
    if (channel[0] == '#')
        channel = channel.substr(1); // remove the '#' character
    for (size_t i = 0; i < channel.size(); i++) {
        channel[i] = std::tolower(channel[i]);
        }
        std::map<std::string, Channel*>::iterator it = channels.find(channel); // find the channel object associated with the channel name.
        if (it == channels.end()) {
            throw std::runtime_error("Channel not found: " + channel);
            }
        return *it->second; // return the channel object.
}

void    Server::createChannel(std::string channel_name, std::string password, std::string topic) {
    std::string val = channel_name;
    if (channel_name[0] == '#')
        val = val.substr(1); // remove the '#' character
    for (size_t i = 0; i < val.size(); i++) {
        val[i] = std::tolower(val[i]);
    }
    if (channels.find(val) != channels.end()) {
        throw std::runtime_error("Channel already exists: " + channel_name);
    }
    Channel* channel = new Channel(channel_name, password, this); // create a new channel object to store the channel information.
    channel->setTopic(topic); // set the channel topic
    channels.insert(std::make_pair(val, channel)); // add the channel to the map of channels.
}

void    Server::removeClient(int fd) {
    std::map<int, Client*>::iterator it = clients.find(fd); // find the client object associated with the file descriptor.
    if (it != clients.end()) {
        std::vector<Channel*> channels = getChannels(fd);
        for (size_t i = 0; i < channels.size(); i++) {
            helperOperator(*channels[i], getClient(fd), *this);
            channels[i]->removeClient(fd); // remove the client from the channel.
        }
        delete it->second; // delete the client object.
        clients.erase(it); // remove the client from the map of clients.
    }
    std::vector<struct pollfd>::iterator it2 = fds.begin(); // find the client in the list of file descriptors to poll.
    for (; it2 != fds.end() && it2->fd != fd; it2++)
        ; // do nothing
    if (it2 != fds.end()) {
        fds.erase(it2); // remove the client from the list of file descriptors to poll.
    }
    std::cout << "Client desconnected from the Socket: " << fd << std::endl;
}

void    Server::cleanUp() {
    close (server_fd);
    for (size_t i = 0; i < fds.size(); i++)
    {
        close(fds[i].fd);
    }
    fds.clear();
    clients.clear();
}