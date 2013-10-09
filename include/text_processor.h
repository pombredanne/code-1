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

#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

/// C++
#include <string>

struct word_container
{
    std::string language;
    std::map<std::string, int> store;
};

/***********************************************************************
 *
 *
 **********************************************************************/
namespace trokam
{
    class text_processor
    {
        private:
            std::string content;

        public:
            text_processor(std::string file_name);
            bool word_frequency(word_container &words);
            static void get_line_match(const std::string &file_name, const std::string &word, std::string &line);
    };
}

#endif /// TEXT_PROCESSOR_H
