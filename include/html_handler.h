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

#ifndef TROKAM_HTML_HANDLER_H
#define TROKAM_HTML_HANDLER_H

/// C++
#include <string>

/***********************************************************************
 *
 *
 **********************************************************************/
namespace trokam
{
    class html_handler
    {
        public:
            static std::string extract_title(std::string local_file_name);
            static void generate_link(const std::string &url, std::string &link);
    };
}

#endif /// TROKAM_HTML_HANDLER_H
