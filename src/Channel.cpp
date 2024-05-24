#include "Channel.hpp"

Channel::Channel():name("nameChanel"){}
Channel::Channel(std::string const &chName){
    this->name = chName;
}
Channel::Channel(const Channel &src){
    *this = src;
}
Channel &Channel::operator=(const Channel &src)
{
    if(this!= &src)
       {
            this->name =src.name;
            this->key = src.key;
            this->topic = src.topic;
            this->userLimit = src.userLimit;
            this->inviteOnly = src.inviteOnly;
       } 
    
    return *this;
}
Channel::~Channel(){}
void Channel::addUser(Client& client){

    if(users.size() == userLimit){
        std::cout << "Channel is full!"<<std::endl;
        return;
    }
    if(inviteOnly){
        std::vector<std::string>::iterator it = std::find(this->inviteUser2.begin(), this->inviteUser2.end(), client);
        if(it == inviteUser2.end())
        {
            std::cout << "User " << client.getUserName()  << " is not invited to the channel " << this->name << std::endl;
            return;
        }
    }
    users.push_back(client);
    inviteUser2.erase(std::remove(inviteUser2.begin(), inviteUser2.end(), client), inviteUser2.end());

    // std::cout << "User has be add!"<<std::endl;
}
void Channel::addOperator(Client & client){
    oper.push_back(client);
}
void Channel::kickUser(const std::string &userName){
    std::vector<Client>::iterator it = users.begin();
    while(it != users.end())
    {
        if(it->getUserName() == userName)
        {
            users.erase(it);
            std::cout << "User "<<userName<<"has been kicked form channel"<< this->name <<std::endl;
            break;
        }
    
    else{
        std::cout << "User "<<userName<<"is not  in the channel"<< this->name <<std::endl;
    }
    it++;
    }
}
void Channel::setMode(std::string &mode){
    if(mode == "i")
        inviteOnly = true;
    else if(mode == "-i")
        inviteOnly = false;
    else if(mode == "k")
        key = mode;
    else if(mode == "-k")
        key = "";
    else if(mode == "t")
        topic = mode;
    else if(mode == "-t")
        topic = "";
    else if(mode == "l")
        userLimit = std::stoi(mode);
    else if(mode == "-l")
        userLimit = 0;
    else if (mode == "o")
        // take channel operator privilege
        std::cout << "Operator mode"<<std::endl;
    else if(mode == "-o")
        // remove channel operator privilege
        std::cout << "Operator mode"<<std::endl;
    else
        std::cout << "Mode not found"<<std::endl;
}
void Channel::inviteUser(std::string userName){
    inviteUser2.push_back(userName);
    std::cout << "User "<<userName<<"has been invited to the channel"<< this->name <<std::endl;
}

void Channel::setTopic(const std::string &topic){
    this->topic = topic;
}
std::string Channel::getTopic(){
    std::cout << "Topic: "<<this->topic<<std::endl;
}
void Channel::sendPublicMessage(int from_socket, const std::string &message){
    std::vector<Client>::iterator it = users.begin();
    while(it != users.end())
    {
        if(it->getFd() != from_socket)
        {
            std::cout << "Message: "<<message<<std::endl;
        }
        it++;
    }
}