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

/// C
#include <stdio.h>

/// C++
#include <iostream>

/// Trokam
#include "token.h"

/***********************************************************************
 *
 *
 **********************************************************************/
int trokam::token::split_protocol_domain_path(const std::string &url,
                                                    std::string &protocol,
                                                    std::string &domain,
                                                    std::string &path)
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Get the protocol part. For instance 'http://'
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::size_t two_slash= url.find("//");
    if (two_slash == std::string::npos)
    {
        /// Here I should raise an exception
        return 1;
    }
    else
    {
        protocol= url.substr(0, two_slash+2);
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Get the domain part. For instance 'www.dmoz.org'
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::size_t first_slash= url.find('/', two_slash+3);
    if (first_slash != std::string::npos)
    {
        domain= url.substr(7, first_slash-7);
    }
    else
    {
        first_slash= url.length()-1;
        domain= url.substr(7, first_slash-7);
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Get the path part. For instance '/some/interesting/page.html'
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    path= url.substr(first_slash, url.length()-first_slash-1);

    return RIGHT;
}

/***********************************************************************
 *
 *
 **********************************************************************/
bool trokam::token::it_has_any_chars_of(const std::string &text,
                                        const std::string &some_chars)
{
    std::size_t pos;

    pos= text.find_first_of(some_chars);
    if (pos != std::string::npos)
    {
        return true;
    }
    return false;
}

std::string trokam::token::leftPadding(const std::string text,
                                       const int total_length,
                                       const std::string fill)
{
    std::string piece;
    int diff= total_length-text.length();

    for (int i=0; i<diff; i++)
    {
        piece+= fill;
    }
    return (piece + text);
}

std::string trokam::token::rightPadding(const std::string text,
                                        const int total_length,
                                        const std::string fill)
{
    std::string piece;
    int diff= total_length-text.length();

    for (int i=0; i<diff; i++)
    {
        piece+= fill;
    }
    return (text + piece);
}
