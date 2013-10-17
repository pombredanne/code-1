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
std::string trokam::language::detect(const trokam::options &options,
                                     const std::string &text)
{
    void *h;
    std::string config_file;

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// The configuration file could be in a location specified by
    /// user or in its default location.
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    std::string config_dir= options.config();
    if(config_dir == "")
    {
        config_file= "/usr/local/etc/trokam/fpdb.conf";
    }
    else
    {
        config_file= config_dir + "/fpdb.conf";
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Initializing the library using the configuration file
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    h= textcat_Init(config_file.c_str());
    if(!h)
    {
        LOG(FATAL, "unable to init libexttextcat");
        exit(1);
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Analizing the text
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    std::string lang(textcat_Classify(h, text.c_str(), text.length()));
    LOG(DEBUG, "lang= " << lang);
    textcat_Done(h);

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// The result is typically "[en]", "[de]", etc.
    /// Trimming out the parentesis to get "en", "de", etc.
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    boost::algorithm::trim_if(lang, boost::algorithm::is_any_of("[]"));

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// If 'lang' is not two characters length then the language
    /// is not valid
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    if(lang.length() != 2)
    {
        LOG(WARNING, "language UNKNOWN");
    }
    return lang;
}
