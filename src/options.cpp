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
#include <string>
#include <iostream>

/// Boost
#include <boost/program_options.hpp>

/// Trokam
#include "db_handler.h"
#include "log.h"
#include "options.h"

/***********************************************************************
 *
 *
 **********************************************************************/
trokam::options::options(int argc, const char* argv[])
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Program Options
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help",         "Produce help message")
        ("db-host",      boost::program_options::value<std::string>(), "Host name or IP address of database server.")
        ("db-name",      boost::program_options::value<std::string>(), "Database name.")
        ("db-user",      boost::program_options::value<std::string>(), "Database user name.")
        ("seed-url",     boost::program_options::value<std::string>(), "First URL inserted into database.")
        ("page-limit",   boost::program_options::value<int>(),         "Number of pages to process")
        ("log-level",    boost::program_options::value<std::string>(), "Minimum level for logging: TRACE, DEBUG, INFO, WARNING, ERROR or FATAL")
        ("domain",       boost::program_options::value<std::string>(), "Used jointly with DOMAIN book-mode option")
        ("raw-file",     boost::program_options::value<std::string>(), "Input file with URLs mixed with other data")
        ("url-file",     boost::program_options::value<std::string>(), "File with URLs used to initialize the database")
        ("output-file",  boost::program_options::value<std::string>(), "Output file")
        ("words",        boost::program_options::value<std::string>(), "Words")
        ("query-method", boost::program_options::value<std::string>(), "Query method")
        ("socket-file",  boost::program_options::value<std::string>(), "Socket file")
        ("file-set",     boost::program_options::value<std::string>(), "File set")
        ("book-mode",    boost::program_options::value<std::string>(), "Booking mode: RANDOM, DOMAIN, SEED, LESS_POPULATED");
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Option 'help'
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(vm.count("help"))
    {
        std::cout << desc << std::endl;
        exit(0);
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Option 'db-host'
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(vm.count("db-host"))
    {
		opt_db_host= vm["db-host"].as<std::string>();
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Option 'db-name'
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(vm.count("db-name"))
    {
		opt_db_name= vm["db-name"].as<std::string>();
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Option 'db-user'
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(vm.count("db-user"))
    {
		opt_db_user= vm["db-user"].as<std::string>();
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Action for 'page-limit'
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    opt_page_limit= 1;
    if(vm.count("page-limit"))
    {
		opt_page_limit= vm["page-limit"].as<int>();
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Action for 'seed-url'
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    opt_seed_url= "";
    if(vm.count("seed-url"))
    {
		opt_seed_url= vm["seed-url"].as<std::string>();
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Action for 'domain'
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    opt_domain= "";
    if(vm.count("domain"))
    {
		opt_domain= vm["domain"].as<std::string>();
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    opt_raw_file= "";
    if(vm.count("raw-file"))
    {
		opt_raw_file= vm["raw-file"].as<std::string>();
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    opt_url_file= "";
    if(vm.count("url-file"))
    {
		opt_url_file= vm["url-file"].as<std::string>();
    }


    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    opt_output_file= "";
    if(vm.count("output-file"))
    {
		opt_output_file= vm["output-file"].as<std::string>();
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Action for 'log level'
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    opt_log_level=trokam::INFO;
    if(vm.count("log-level"))
    {
		std::string text_log_level= vm["log-level"].as<std::string>();
        if(text_log_level == "TRACE")
        {
            opt_log_level= trokam::TRACE;
        }
        else if(text_log_level == "DEBUG")
        {
            opt_log_level= trokam::DEBUG;
        }
        else if(text_log_level == "INFO")
        {
            opt_log_level= trokam::INFO;
        }
        else if(text_log_level == "WARNING")
        {
            opt_log_level= trokam::WARNING;
        }
        else if(text_log_level == "ERROR")
        {
            opt_log_level= trokam::ERROR;
        }
        else if(text_log_level == "FATAL")
        {
            opt_log_level= trokam::FATAL;
        }
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Action for 'book-mode'
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	opt_book_mode= RANDOM;
    if(vm.count("book-mode"))
    {
		std::string book_mode_option= vm["book-mode"].as<std::string>();

		if(book_mode_option == "RANDOM")
		{
			opt_book_mode= RANDOM;
		}

		if(book_mode_option == "DOMAIN")
		{
			opt_book_mode= DOMAIN;
            if(opt_domain == "")
            {
                LOG(FATAL, "With book mode RANDOM a domain must be provided using option --domain");
                exit(1);
            }
		}

		if(book_mode_option == "SEED")
		{
			opt_book_mode= SEED;
		}
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    opt_words= "";
    if(vm.count("words"))
    {
		opt_words= vm["words"].as<std::string>();
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    opt_query_method= "";
    if(vm.count("query-method"))
    {
		opt_query_method= vm["query-method"].as<std::string>();
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    opt_socket_file= "";
    if(vm.count("socket-file"))
    {
		opt_socket_file= vm["socket-file"].as<std::string>();
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    opt_file_set= "";
    if(vm.count("file-set"))
    {
		opt_file_set= vm["file-set"].as<std::string>();
    }

    if(opt_log_level <= trokam::DEBUG)
    {
        LOG(DEBUG, "db-host= "  << opt_db_host);
        LOG(DEBUG, "db-name= "  << opt_db_name);
        LOG(DEBUG, "db-user= "  << opt_db_user);
        LOG(DEBUG, "seed-url= " << opt_seed_url);
        LOG(DEBUG, "domain= "   << opt_domain);
        LOG(DEBUG, "page-limit= " << opt_page_limit);
        LOG(DEBUG, "book-mode= "  << opt_book_mode);
        LOG(DEBUG, "raw-file= "  << opt_raw_file);
        LOG(DEBUG, "output-file= "  << opt_output_file);
        LOG(DEBUG, "words= "  << opt_words);
        LOG(DEBUG, "query-method= "  << opt_query_method);
        LOG(DEBUG, "socket-file= "  << opt_socket_file);
        LOG(DEBUG, "file-set= "  << opt_file_set);
        LOG(DEBUG, "log-level= "  << opt_log_level);
    }
}

/***********************************************************************
 *
 *
 **********************************************************************/
trokam::options::options(const trokam::options &opt)
{
    opt_page_limit= opt.page_limit();
    opt_book_mode= opt.book_mode();
    opt_seed_url= opt.seed_url();
    opt_domain= opt.domain();
    opt_db_host= opt.db_host();
    opt_db_name= opt.db_name();
    opt_db_user= opt.db_user();
    opt_raw_file= opt.raw_file();
    opt_url_file= opt.url_file();
    opt_output_file= opt.output_file();
    opt_words= opt.words();
    opt_query_method= opt.query_method();
    opt_socket_file= opt.socket_file();
    opt_file_set= opt.file_set();
}

/***********************************************************************
 *
 *
 **********************************************************************/
trokam::options& trokam::options::operator= (const trokam::options &opt)
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Avoid self-assignment
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(this == &opt)
    {
        return *this;
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Copy data
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    opt_page_limit= opt.page_limit();
    opt_book_mode= opt.book_mode();
    opt_seed_url= opt.seed_url();
    opt_domain= opt.domain();
    opt_db_host= opt.db_host();
    opt_db_name= opt.db_name();
    opt_db_user= opt.db_user();
    opt_raw_file= opt.raw_file();
    opt_url_file= opt.url_file();
    opt_output_file= opt.output_file();
    opt_words= opt.words();
    opt_query_method= opt.query_method();
    opt_socket_file= opt.socket_file();
    opt_file_set= opt.file_set();

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Return this object
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    return *this;
}

/***********************************************************************
 *
 *
 **********************************************************************/
int trokam::options::page_limit() const
{
    return opt_page_limit;
}

/***********************************************************************
 *
 *
 **********************************************************************/
int trokam::options::book_mode() const
{
    return opt_book_mode;
}

/***********************************************************************
 *
 *
 **********************************************************************/
std::string trokam::options::seed_url() const
{
    return opt_seed_url;
}

/***********************************************************************
 *
 *
 **********************************************************************/
std::string trokam::options::domain() const
{
    return opt_domain;
}

/***********************************************************************
 *
 *
 **********************************************************************/
std::string trokam::options::raw_file() const
{
    return opt_raw_file;
}

/***********************************************************************
 *
 *
 **********************************************************************/
std::string trokam::options::url_file() const
{
    return opt_url_file;
}

/***********************************************************************
 *
 *
 **********************************************************************/
std::string trokam::options::output_file() const
{
    return opt_output_file;
}

/***********************************************************************
 *
 *
 **********************************************************************/
std::string trokam::options::db_host() const
{
    return opt_db_host;
}

/***********************************************************************
 *
 *
 **********************************************************************/
std::string trokam::options::db_name() const
{
    return opt_db_name;
}

/***********************************************************************
 *
 *
 **********************************************************************/
std::string trokam::options::db_user() const
{
    return opt_db_user;
}

/***********************************************************************
 *
 *
 **********************************************************************/
std::string trokam::options::words() const
{
    return opt_words;
}

/***********************************************************************
 *
 *
 **********************************************************************/
std::string trokam::options::query_method() const
{
    return opt_query_method;
}

/***********************************************************************
 *
 *
 **********************************************************************/
std::string trokam::options::socket_file() const
{
    return opt_socket_file;
}

/***********************************************************************
 *
 *
 **********************************************************************/
std::string trokam::options::file_set() const
{
    return opt_file_set;
}

/***********************************************************************
 *
 *
 **********************************************************************/
trokam::severity trokam::options::log_level() const
{
    return opt_log_level;
}
