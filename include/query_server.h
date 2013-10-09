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

#ifndef TROKAM_QUERY_SERVER_H
#define TROKAM_QUERY_SERVER_H

/// C
#include <cstring>

/// C++
#include <iostream>
#include <string>
#include <sstream>

/// Boost
#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>

/// Trokam
#include "options.h"
#include "postgresql.h"
#include "db_handler.h"

const int MAX_LENGTH= 65536;
typedef boost::shared_ptr<boost::asio::local::stream_protocol::socket> socket_ptr;

/***********************************************************************
 *
 *
 **********************************************************************/
namespace trokam
{
    class query_server
    {
        public:
            query_server(boost::asio::io_service &ios, trokam::options &options);
            ~query_server();
            void listen(const std::string& file);

        private:
            bool run;
            boost::asio::io_service &ioService;
            trokam::options opt;
            trokam::db_handler db;

            boost::mutex only_one;
            std::string outgoing;
            std::stringstream incoming;
            void session(socket_ptr sock);
    };
}

#endif  /// TROKAM_QUERY_SERVER_H
