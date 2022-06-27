#pragma once
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "message_connection.h"
using boost::asio::ip::tcp;


class tcp_server
{

    tcp::acceptor acceptor_;
    boost::asio::io_service& io_service;
    std::function<void(std::shared_ptr<message_connection>)> connectionAccepted;
    

public:
    tcp_server(boost::asio::io_service& io_service, int port, 
        std::function<void(std::shared_ptr<message_connection>)> connectionAccepted)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
        io_service(io_service), connectionAccepted(connectionAccepted)
    {

    }
    void start_accept()
    {
        auto new_connection = std::make_shared<message_connection>(io_service);

        acceptor_.async_accept(new_connection->socket(),
            boost::bind(&tcp_server::handle_accept, this, new_connection,
                boost::asio::placeholders::error));
    }
private:
    void handle_accept(std::shared_ptr<message_connection> new_connection,
        const boost::system::error_code& error)
    {
        if (!error)
        {
            std::cout << "accepted" << std::endl;
            connectionAccepted(new_connection);
            //new_connection->writeMesssage("Send All", 1);
            start_accept();
        }
        else {
            std::cout << "error accpeting " << error << std::endl;
        }
    }

};