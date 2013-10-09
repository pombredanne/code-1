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
#include <string>
#include <vector>

/// Trokam
#include "db_handler.h"
#include "options.h"
#include "log.h"

/***********************************************************************
 *
 *
 **********************************************************************/
int main(int argc, const char* argv[])
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Process command options
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    trokam::options opt(argc, argv);

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int word_total= 0;

    std::vector<std::string> lang;
    lang.push_back("en");
    lang.push_back("pt");
    lang.push_back("ar");
    lang.push_back("de");
    lang.push_back("ru");
    lang.push_back("ja");
    lang.push_back("es");

    for (std::vector<std::string>::iterator it= lang.begin() ; it != lang.end(); ++it)
    {
        trokam::db_handler db(opt);

        word_total= db.get_number_of_words(*it);
        LOG(INFO, "Total words in language '" << *it << "' are '" << word_total << "'");

        db.get_occurrency_of_words(*it);
        LOG(INFO, "Completed 'get_occurrency_of_words(..)'");

        db.get_frequency_of_words(word_total, *it);
        LOG(INFO, "Completed 'get_frequency_of_words(..)'");

        db.get_occurences_per_path(*it);
        LOG(INFO, "Completed 'get_occurences_per_path(..)'");

        db.get_word_frequency_for_path(*it);
        LOG(INFO, "Completed 'get_word_frequency_for_path(..)'");
    }
    return 0;
}
