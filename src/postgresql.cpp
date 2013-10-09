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
#include <cassert>
#include <iostream>

/// Trokam
#include "exception.h"
#include "postgresql.h"
#include "log.h"

trokam::postgresql::postgresql(std::string connection_parameters)
{
    db_connection= new pqxx::connection(connection_parameters);
    LOG(DEBUG, "connected to database");
    LOG(DEBUG, "backend version: " << db_connection->server_version());
    LOG(DEBUG, "protocol version: " << db_connection->protocol_version());
}

/***********************************************************************
 *
 *
 **********************************************************************/
void trokam::postgresql::exec_sql(const std::string &sentence, std::map<std::string, int> &data)
{
    try
    {
        /// Begin a transaction acting on our current connection.
        pqxx::work T(*db_connection, "execute");

        /// Perform a query on the database, storing result tuples in R.
        pqxx::result R(T.exec(sentence));

        /// Process each successive result tuple
        for(pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c)
        {
            data[c[0].as(std::string())]= c[1].as(int());
        }

        /// Tell the transaction that it has been successful. This is not really
        /// necessary if no modifications are made to commit.
        T.commit();
    }
    catch(const pqxx::sql_error &e)
    {
        /// The sql_error exception class gives us some extra information
        trokam::exception problem(e.what());
        throw problem;
    }
    catch(const std::exception &e)
    {
        /// All exceptions thrown by libpqxx are derived from std::exception
        trokam::exception problem(e.what());
        throw problem;
    }
    catch(...)
    {
        /// This is really unexpected (see above)
        trokam::exception problem("Something happened");
        throw problem;
    }
}

/***********************************************************************
 *
 *
 **********************************************************************/
void trokam::postgresql::exec_sql(const std::string &sentence)
{
    try
    {
        /// Begin a transaction acting on our current connection.
        pqxx::work T(*db_connection, "execute_no_answer");

        /// Perform a query on the database, storing result tuples in R.
        T.exec(sentence);

        /// Tell the transaction that it has been successful. This is not really
        /// necessary if no modifications are made to commit.
        T.commit();
    }
    catch(const pqxx::sql_error &e)
    {
        /// The sql_error exception class gives us some extra information
        std::cerr << "SQL error: " << e.what() << std::endl
                  << "Query was: " << e.query() << std::endl;
    }
    catch(const std::exception &e)
    {
        /// All exceptions thrown by libpqxx are derived from std::exception
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch(...)
    {
        /// This is really unexpected (see above)
        std::cerr << "Unhandled exception" << std::endl;
    }
}

/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// SELECT * FROM table WHERE condition
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string trokam::postgresql::select_all(std::string table, std::string condition)
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string sentence;
    sentence+= "SELECT * ";
    sentence+= "FROM " + table + " ";
    sentence+= "WHERE " + condition + " ";

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string answer;
    exec_sql(sentence, answer);
    return answer;
}

/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// SELECT field FROM table WHERE condition
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string trokam::postgresql::select(std::string field, std::string table, std::string condition)
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string sentence;
    sentence+= "SELECT " + field + " ";
    sentence+= "FROM " + table + " ";
    sentence+= "WHERE " + condition + " ";

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string answer;
    exec_sql(sentence, answer);
    return answer;
}

/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// INSERT INTO table VALUES(values)
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void trokam::postgresql::insert(std::string table, std::string values)
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string sentence;
    sentence+= "INSERT INTO " + table + " ";
    sentence+= "VALUES (" + values + ");";

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    exec_sql(sentence);
}

/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// UPDATE table SET field=value WHERE condition
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void trokam::postgresql::update(std::string table, std::string field_value, std::string condition)
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string sentence;
    sentence+= "UPDATE " + table + " ";
    sentence+= "SET " + field_value + " ";
    sentence+= "WHERE " + condition + " ";

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    exec_sql(sentence);
}

/***********************************************************************
 *
 *
 **********************************************************************/
void trokam::postgresql::exec_sql(const std::string &sentence,
                                  boost::scoped_ptr<pqxx::result> &answer)
{
    try
    {
        /// Begin a transaction acting on our current connection.
        pqxx::work T(*db_connection, "execute");

        /// Perform a query on the database, storing result tuples in R.
        answer.reset(new pqxx::result(T.exec(sentence)));

        /// Tell the transaction that it has been successful. This is not really
        /// necessary if no modifications are made to commit.
        T.commit();
    }
    catch(const pqxx::sql_error &e)
    {
        /// The sql_error exception class gives us some extra information
        std::cerr << "SQL error: " << e.what() << std::endl
                  << "Query was: " << e.query() << std::endl;
        trokam::exception problem(e.what());
        throw problem;
    }
    catch(const std::exception &e)
    {
        /// All exceptions thrown by libpqxx are derived from std::exception
        std::cerr << "Exception: " << e.what() << std::endl;
        trokam::exception problem(e.what());
        throw problem;
    }
    catch(...)
    {
        /// This is really unexpected (see above)
        std::cerr << "Unhandled exception" << std::endl;
        trokam::exception problem("Something happened");
        throw problem;
    }
}

/***********************************************************************
 *
 *
 **********************************************************************/
void trokam::postgresql::exec_sql(const std::string &sentence,
                                  std::string &answer)
{
    try
    {
        /// Begin a transaction acting on our current connection.
        pqxx::work T(*db_connection, "execute");

        /// Perform a query on the database, storing result tuples in R.
        pqxx::result R(T.exec(sentence));

        /// Put in the string 'answer' the element of the of the first
        /// column of the first row of the results.
        pqxx::result::const_iterator c= R.begin();
        if(c != R.end())
        {
            answer= c[0].as(std::string());
        }

        /// Tell the transaction that it has been successful. This is not really
        /// necessary if no modifications are made to commit.
        T.commit();
    }
    catch(const pqxx::sql_error &e)
    {
        /// The sql_error exception class gives us some extra information
        std::cerr << "SQL error: " << e.what() << std::endl
                  << "Query was: " << e.query() << std::endl;
        trokam::exception problem(e.what());
        throw problem;
    }
    catch(const std::exception &e)
    {
        /// All exceptions thrown by libpqxx are derived from std::exception
        std::cerr << "Exception: " << e.what() << std::endl;
        trokam::exception problem(e.what());
        throw problem;
    }
    catch(...)
    {
        /// This is really unexpected (see above)
        std::cerr << "Unhandled exception" << std::endl;
        trokam::exception problem("Something happened");
        throw problem;
    }
}
