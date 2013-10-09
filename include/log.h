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

#ifndef TROKAM_LOG_H
#define TROKAM_LOG_H

/// C++
#include <string>
#include <sstream>
#include <ostream>

/// Trokam
#include "options.h"

#define HERE   __PRETTY_FUNCTION__
#define SEPARATOR "  "

/***********************************************************************
 *
 *
 **********************************************************************/
#define LOG(X,Y)                                     \
{                                                    \
    trokam::log::location << __PRETTY_FUNCTION__;    \
    trokam::log::report   << Y;                      \
    trokam::log::msg(trokam::X);                     \
}

#define START                                        \
{                                                    \
    trokam::log::report   << __PRETTY_FUNCTION__     \
                          << "  "                    \
                          << "START";                \
    trokam::log::msg(trokam::TRACE);                 \
}

#define END                                          \
{                                                    \
    trokam::log::report   << __PRETTY_FUNCTION__     \
                          << "  "                    \
                          << "END";                  \
    trokam::log::msg(trokam::TRACE);                 \
}

#define RETURN(Z)                                    \
{                                                    \
    trokam::log::report   << __PRETTY_FUNCTION__     \
                          << "  "                    \
                          << "END";                  \
    trokam::log::msg(trokam::TRACE);                 \
    return(Z);                                       \
}

/***********************************************************************
 *
 *
 **********************************************************************/
namespace trokam
{
    enum format
    {
        SIMPLE,
        FULL
    };

    class log
    {
        public:
            static std::stringstream location;
            static std::stringstream report;
            static void setup(trokam::options &opt);
            static void msg(trokam::severity level);

        private:
            static severity display_min;
            static format display_format;
    };
}

#endif /// TROKAM_LOG_H
