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
#include <fstream>
#include <string>
#include <cerrno>

/// Trokam
#include "file_management.h"

/***********************************************************************
 *
 **********************************************************************/
std::string trokam::file_management::get_file_contents(std::string filename)
{
    std::string content;
    std::ifstream input_file(filename.c_str(), std::ios::in | std::ios::binary);
    if(input_file)
    {
        input_file.seekg(0, std::ios::end);
        content.resize(input_file.tellg());
        input_file.seekg(0, std::ios::beg);
        input_file.read(&content[0], content.size());
        input_file.close();
    }
    return(content);
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::file_management::get_file_contents(const std::string &file_name, std::string &content)
{
    std::ifstream input_file(file_name.c_str(), std::ios::in | std::ios::binary);
    if(input_file)
    {
        input_file.seekg(0, std::ios::end);
        content.resize(input_file.tellg());
        input_file.seekg(0, std::ios::beg);
        input_file.read(&content[0], content.size());
        input_file.close();
    }
    throw(errno);
}

/***********************************************************************
 *
 **********************************************************************/
bool trokam::file_management::is_empty(std::string file_name)
{
    std::ifstream input_file(file_name.c_str());
    return input_file.peek() == std::ifstream::traits_type::eof();
}

/***********************************************************************
 *
 **********************************************************************/
std::string trokam::file_management::get_first_line(std::string file_name)
{
    std::string line;
    std::ifstream input_file(file_name.c_str(), std::ios::in | std::ios::binary);
    if(input_file)
    {
        std::getline(input_file, line);
    }
    return line;
}
