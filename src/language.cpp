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

/// Boost
#include <boost/algorithm/string.hpp>

/// Trokam
#include "language.h"
#include "log.h"

/***********************************************************************
 *
 **********************************************************************/
std::string trokam::language::detect(std::string &text)
{
    void *h;

    h= textcat_Init("fpdb.conf" );
    if(!h)
    {
        LOG(FATAL, "unable to init libexttextcat");
        exit(1);
    }

    std::string lang(textcat_Classify(h, text.c_str(), text.length()));
    boost::algorithm::trim_if(lang, boost::algorithm::is_any_of("[]"));
    textcat_Done(h);

    if (lang.length() != 2)
    {
        LOG(WARNING, "language UNKNOWN");
    }
    return lang;
}
