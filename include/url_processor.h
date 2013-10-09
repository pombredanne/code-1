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

#ifndef URL_PROCESSOR_H
#define URL_PROCESSOR_H

/// C++
#include <string>
#include <vector>

/***********************************************************************
 *
 *
 **********************************************************************/
namespace trokam
{
    class url_processor
    {
        public:
            url_processor();
            std::vector<std::string> extract(std::string local_file);
            void extract(std::string input_file, std::string output_file);
        private:
            void get_urls(std::string text);
    };
}

#endif  /// URL_PROCESSOR_H
