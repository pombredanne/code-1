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

#ifndef TROKAM_EXCEPTION_H
#define TROKAM_EXCEPTION_H

/// C++
#include <string>
#include <iostream>
#include <exception>

/***********************************************************************
 *
 *
 **********************************************************************/
namespace trokam
{
    class exception: public std::exception
    {
        private:
            std::string description;

        public:
            exception(std::string what_happened);
            ~exception() throw() {}
            virtual const char* what() const throw()
            {
                return description.c_str();
            }
    };
}

#endif /// TROKAM_EXCEPTION_H
