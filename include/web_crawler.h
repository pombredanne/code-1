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

#ifndef TROKAM_WEB_CRAWLER_H
#define TROKAM_WEB_CRAWLER_H

/// C
#include <cstdlib>
#include <signal.h>

/// C++
#include <string>

/// Trokam
#include "db_handler.h"
#include "file_converter.h"
#include "options.h"
#include "url_processor.h"
#include "web_handler.h"

/***********************************************************************
 *
 *
 **********************************************************************/
namespace trokam
{
    class web_crawler
    {
        public:
            web_crawler(trokam::options &options);
            void run();

        private:
            std::string base_directory;
            std::string directory_files;
            std::string file_content_raw;
            std::string file_content_text;

            trokam::options opt;
            trokam::web_handler web;
            trokam::db_handler db;
            trokam::url_processor up;
            trokam::file_converter fc;

            static bool stop;
            static void ctrl_c_signal(int signal);
            // void crtl_c_handler(int signal);
    };
}
#endif  /// TROKAM_WEB_CRAWLER_H
