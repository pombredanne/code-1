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

#ifndef TROKAM_TOKEN_H
#define TROKAM_TOKEN_H

/// C++
#include <string>

#define RIGHT 0
#define TAB   "\t"

namespace trokam
{
    class token
    {
        public:
            static int split_protocol_domain_path(const std::string &url,
                                                        std::string &protocol,
                                                        std::string &domain,
                                                        std::string &path);
            static bool it_has_any_chars_of(const std::string &text,
                                            const std::string &some_chars);
            static std::string leftPadding(const std::string text,
                                           const int total_length,
                                           const std::string fill= " ");
            static std::string rightPadding(const std::string text,
                                            const int total_length,
                                            const std::string fill= " ");
    };
}

#endif /// TROKAM_TOKEN_H
