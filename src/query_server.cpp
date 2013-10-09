/***********************************************************************
 *                            T R O K A M
 *                 Free and Open Source Search Engine
 *
 * Documentation  http://www.trokam.com/
 * Development    http://www.github.com/trokam
 *
 * Copyright (C) 2013, Nicolas Slusarenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 **********************************************************************/

/// Boost
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/signal.hpp>

/// Trokam
#include "converter.h"
#include "query_server.h"
#include "log.h"
#include "token.h"

/***********************************************************************
 *
 **********************************************************************/
trokam::query_server::query_server(boost::asio::io_service &ios, trokam::options &options): ioService(ios),
                                                                                            opt(options),
                                                                                            db(options)
{}

/***********************************************************************
 *
 **********************************************************************/
trokam::query_server::~query_server()
{}

/***********************************************************************
 *
 **********************************************************************/
void trokam::query_server::listen(const std::string& file)
{
    boost::asio::local::stream_protocol::acceptor pending(ioService, boost::asio::local::stream_protocol::endpoint(file));
    while(run)
    {
        socket_ptr sock(new boost::asio::local::stream_protocol::socket(ioService));
        pending.accept(*sock);
        boost::thread t(boost::bind(&query_server::session, this, sock));
    }
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::query_server::session(socket_ptr sock)
{
    try
    {
        std::stringstream incoming;
        while(true)
        {
            char data[MAX_LENGTH];

            boost::system::error_code error;
            size_t length = sock->read_some(boost::asio::buffer(data), error);
            if (error == boost::asio::error::eof)
            {
                LOG(TRACE, "connection closed cleanly by peer");
                break;
            }
            else if (error)
            {
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// Some other error.
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                throw boost::system::system_error(error);
            }

            incoming.write(data, length);
            if(incoming.str().length() >= 200)
            {
                LOG(DEBUG, "sending answer");
                std::string result;

                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// Allowing only one thread query database at the
                /// same time because libpqxx is not thread safe.
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                only_one.lock();
                db.query_word_set(incoming.str(), result);
                only_one.unlock();

                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                ///
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                unsigned int data_blocks= ceil((float(result.length())+10.0)/200.0);
                unsigned int data_len= data_blocks*200;

                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// Writes the length of the answer to the requester.
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                result= trokam::token::rightPadding(result, data_len-10);
                std::string answer_len= converter::to_string(result.length()+10);
                answer_len= trokam::token::leftPadding(answer_len, 10, "0");
                boost::asio::write(*sock, boost::asio::buffer(answer_len.c_str(), answer_len.length()));

                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// Writes the answer back to the requester.
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                boost::asio::write(*sock, boost::asio::buffer(result.c_str(), result.length()));

                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// Cleaning up buffers.
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                incoming.str("");
                strcpy(data,"\0");
            }
        }
    }
    catch(std::exception& e)
    {}
}
