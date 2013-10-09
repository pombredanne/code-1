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

/// C++
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <iostream>

/// Boost
#include <boost/asio.hpp>

/// Trokam
#include "converter.h"
#include "db_handler.h"
#include "log.h"
#include "query_client.h"
#include "token.h"

using boost::asio::local::stream_protocol;

enum { max_length = 200 };

/***********************************************************************
 *
 *
 **********************************************************************/
trokam::query_client::query_client(trokam::options &options): opt(options)
{}

/***********************************************************************
 *
 *
 **********************************************************************/
void trokam::query_client::execute()
{
    std::string words= opt.words();

    if(words != "")
    {
        if(opt.query_method() == "direct")
        {
            std::string answer;
            trokam::db_handler *db= new trokam::db_handler(opt);
            words= trokam::token::leftPadding(words, 200);
            db->query_word_set(words, answer);
            std::cout << answer << std::endl;
            delete db;
        }

        if(opt.query_method() == "server")
        {
            boost::asio::io_service io_service;
            stream_protocol::socket s(io_service);
            s.connect(stream_protocol::endpoint(opt.socket_file().c_str()));

            std::string words= trokam::token::leftPadding(opt.words(), 200);
            boost::asio::write(s, boost::asio::buffer(words.c_str(), words.length()));

            char reply[max_length];
            boost::asio::read(s, boost::asio::buffer(reply, max_length));

            std::string answer;
            answer.append(reply);
            unsigned int expected_len= converter::string_to_uint(answer.substr(0,10));
            LOG(DEBUG, "expected length of answer= " << expected_len);

            while(answer.length() < expected_len)
            {
                boost::asio::read(s, boost::asio::buffer(reply, max_length));
                answer.append(reply);
                LOG(DEBUG, "current length of answer= " << answer.length());
            }

            std::cout << "\n";
            std::cout << answer.substr(10, answer.length()-10) << "\n" << std::endl;
        }

    }
    else
    {
        LOG(FATAL, "Provide some words with option --words");
        exit(1);
    }
}

