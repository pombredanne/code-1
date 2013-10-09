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
#include <cstdlib>

/// C++
#include <iostream>
#include <exception>

/// Trokam
#include "exception.h"
#include "log.h"
#include "machine.h"
#include "web_handler.h"

/***********************************************************************
 *
 *
 **********************************************************************/
int trokam::web_handler::fetch(const std::string &url,
                               const std::string &destination)
{
    LOG(DEBUG, "getting '" << url << "' ... ");
    trokam::machine::rm_file(destination);
    std::string command= "wget -q --timeout=60 -O " + destination + " -k " + url;
    int exit_status= system(command.c_str());
    if (exit_status != 0)
    {
        LOG(WARNING, "fail during download, exit status= " << exit_status);
    }
    return exit_status;
}
