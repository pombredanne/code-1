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

#ifndef TROKAM_OPTIONS_H
#define TROKAM_OPTIONS_H

/***********************************************************************
 *
 *
 **********************************************************************/
namespace trokam
{
    enum severity
    {
        TRACE,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    class options
    {
        public:
            options(int argc, const char* argv[]);
            options(const options &opt);
            options& operator= (const options &opt);
            int          page_limit() const;
            int          book_mode() const;
            std::string  seed_url() const;
            std::string  domain() const;
            std::string  raw_file() const;
            std::string  url_file() const;
            std::string  output_file() const;
            severity     log_level() const;
            std::string  db_host() const;
            std::string  db_name() const;
            std::string  db_user() const;
            std::string  words() const;
            std::string  query_method() const;
            std::string  socket_file() const;
            std::string  file_set() const;

        private:
            int          opt_page_limit;
            int          opt_book_mode;
            std::string  opt_seed_url;
            std::string  opt_domain;
            std::string  opt_db_host;
            std::string  opt_db_name;
            std::string  opt_db_user;
            std::string  opt_raw_file;
            std::string  opt_url_file;
            std::string  opt_output_file;
            std::string  opt_words;
            std::string  opt_query_method;
            std::string  opt_socket_file;
            std::string  opt_file_set;
            severity  opt_log_level;
    };
}

#endif /// TROKAM_OPTIONS_H
