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

#ifndef TROKAM_POSTGRESQL_H
#define TROKAM_POSTGRESQL_H

/// C++
#include <string>
#include <memory>

/// Boost
#include <boost/scoped_ptr.hpp>

/// Postgresql
#include <pqxx/pqxx>

/***********************************************************************
 *
 *
 **********************************************************************/
namespace trokam
{
    class postgresql
    {
        public:
            postgresql(std::string connection_parameters);

            /// SELECT * FROM table WHERE condition
            std::string select_all(std::string table, std::string condition);

            /// SELECT field FROM table WHERE condition
            std::string select(std::string field, std::string table, std::string condition);

            /// INSERT INTO table VALUES(values)
            void insert(std::string table, std::string values);

            /// UPDATE table SET field=value WHERE condition
            void update(std::string table, std::string field_value, std::string condition);

            void exec_sql(const std::string &sentence);
            void exec_sql(const std::string &sentence, boost::scoped_ptr<pqxx::result> &answer);
            void exec_sql(const std::string &sentence, std::string &answer);
            void exec_sql(const std::string &sentence, std::map<std::string, int> &answer);

        private:
            pqxx::connection *db_connection;
    };
}

#endif /// TROKAM_POSTGRESQL_H
