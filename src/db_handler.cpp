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
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

/// C++
#include <iostream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

/// Boost
#include <boost/algorithm/string.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/tokenizer.hpp>

/// Postgresql
#include <pqxx/pqxx>

/// Trokam
#include "converter.h"
#include "db_handler.h"
#include "exception.h"
#include "html_handler.h"
#include "log.h"
#include "text_processor.h"
#include "token.h"

#define EMPTY    ""
#define FAIL     0
#define SUCCESS  1

/***********************************************************************
 *
 **********************************************************************/
trokam::db_handler::db_handler(trokam::options &options): opt(options)
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Connection parameters to database
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string parameters;
    if(opt.db_host() != "")
    {
        parameters+= "host=" + opt.db_host() + " ";
    }

    if(opt.db_name() != "")
    {
        parameters+= "dbname=" + opt.db_name() + " ";
    }

    if(opt.db_user() != "")
    {
        parameters+= "user=" + opt.db_user() + " ";
    }

    LOG(DEBUG, "Database connection parameters: " << parameters);
    db= new postgresql(parameters);

    if(opt.book_mode() == 1) /// DOMAIN
    {
        domain_index= get_domain_index(opt.domain());
        fixed_domain= opt.domain();
    }
}

/***********************************************************************
 *
 **********************************************************************/
trokam::db_handler::~db_handler()
{
    delete db;
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::set_client_identifier(const std::string &id)
{
    feeder= id;
}

/***********************************************************************
 *
 **********************************************************************/
bool trokam::db_handler::already_used_for_word_stat(std::string url)
{
    std::string url_index;
    std::string protocol;
    std::string domain;
    std::string path;

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Split the url in protocol, domain and path.
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    trokam::token::split_protocol_domain_path(url, protocol, domain, path);

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Searches for the domain and path
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string sentence;
    sentence=  "SELECT path.index ";
    sentence+= "FROM path, domain ";
    sentence+= "WHERE path.id='" + path + "' ";
    sentence+= "AND path.processed=false ";
    sentence+= "AND domain.id='" + domain + "' ";
    sentence+= "AND path.domain_index=domain.index ";
    db->exec_sql(sentence, url_index);

    if(url_index == "")
    {
        return true;
    }
    return false;
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::mark_as_used_for_word_stat(std::string url_index)
{
    db->update("path", "processed=true, date_processing=now()", "index='" + url_index + "'");
    db->update("path", "booked=false", "index='" + url_index + "'");
}

/***********************************************************************
 * Pick up one random url not used yet for word statistics
 **********************************************************************/
std::string trokam::db_handler::pickup_one_url()
{
    std::string sentence;
    std::string url;

    sentence=  "SELECT id ";
    sentence+= "FROM path ";
    sentence+= "WHERE processed=false ";
    sentence+= "AND weird=false ";
    sentence+= "ORDER BY RANDOM() ";
    sentence+= "LIMIT 1";
    db->exec_sql(sentence, url);
    return url;
}

/***********************************************************************
 * Insert the URLs in the url_container into the database
 **********************************************************************/
void trokam::db_handler::inject_urls(std::vector<std::string> url_container)
{
    START;
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// It goes on one by one of the URLs
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for(unsigned int i=0; i<url_container.size(); i++)
    {
        inject_one_url(url_container[i]);
    }
    END;
}

/***********************************************************************
 *
 **********************************************************************/
std::string trokam::db_handler::inject_one_url(std::string url, int seed)
{
    std::string protocol;
    std::string domain;
    std::string path;
    std::string null_answer("-1");
    bool weird_url= false;

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(url == "")
    {
        LOG(ERROR, "URL is empty.");
        return null_answer;
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// If the URL has the char ' it is rejected because
    /// method fetch fails.
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(trokam::token::it_has_any_chars_of(url, "'"))
    {
        LOG(WARNING, "URL rejected because it has a unwanted character: " << url);
        return null_answer;
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Set the URL as weird if has strange chars.
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(trokam::token::it_has_any_chars_of(url, "?+$;=!"))
    {
        LOG(WARNING, "URL labeled as weird because it has a unwanted character: "  << url);
        weird_url= true;
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Split the protocol, the domain and the path.
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(trokam::token::split_protocol_domain_path(url, protocol, domain, path) == 1)
    {
        LOG(ERROR, "URL could not be split in protocol, domain and path: " << url);
        return null_answer;
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Must be not have spaces and should have at least one dot
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if((domain == "") ||
       (trokam::token::it_has_any_chars_of(domain, " ")) ||
       !(trokam::token::it_has_any_chars_of(domain, ".")))
    {
        LOG(ERROR, "Rejected domain: '" << domain << "'.");
        return null_answer;
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// The URL must start with 'http://' otherwise it is not
    /// stored in the database.
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(protocol != "http://")
    {
        LOG(WARNING, "Protocol '" << protocol << "' rejected.");
        return null_answer;
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Get the index of domain
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string domain_index= get_domain_index(domain);
    if(domain_index == "")
    {
        domain_index= insert_domain(domain);
    }

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Find out if the URL exists already in the database
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string sentence;
    std::string url_index;

    sentence=  "SELECT index ";
    sentence+= "FROM path ";
    sentence+= "WHERE id='" + path + "' ";
    sentence+= "AND domain_index=" + domain_index;
    db->exec_sql(sentence, url_index);

    std::string s;
    if(seed == 1)
    {
        s= "true";
    }
    else
    {
        s= "false";
    }

    try
    {
        if(url_index != "")
        {
            sentence=  "UPDATE path ";
            sentence+= "SET refs=refs+1 ";
            sentence+= "WHERE index=" + url_index;
            db->exec_sql(sentence);
            LOG(INFO, "URL exists in database: " << url);
        }
        else
        {
            std::string w= converter::to_string(weird_url);
            sentence=  "INSERT INTO path ";
            sentence+= "VALUES (";
            sentence+= "DEFAULT, ";            ///  index           | integer                     | not null default nextval('db_handler_index_seq'::regclass)
            sentence+= domain_index + ", ";    ///  domain_index    | integer                     |
            sentence+= "'" + path + "', ";     ///  id              | character varying(1000)     |
            sentence+= w + ", ";               ///  weird           | boolean                     |
            sentence+= s + ", ";               ///  seed            | boolean                     |
            sentence+= "-1,";                  ///  download_state  | integer                     |
            sentence+= "'', ";                 ///  language        | character varying(10)       |
            sentence+= "'', ";                 ///  title           | character varying(1000)     |
            sentence+= "-1, ";                 ///  refs            | integer                     |
            sentence+= "-1, ";                 ///  word_count      | integer                     |
            sentence+= "-1, ";                 ///  url_count       | integer                     |
            sentence+= "'', ";                 ///  file_type       | character varying(10)       |
            sentence+= "'" + feeder + "', ";   ///  feeder          | character varying(10)       |
            sentence+= "now(), ";              ///  date_fed        | timestamp without time zone |
            sentence+= "false, ";              ///  processed       | boolean                     |
            sentence+= "NULL, ";               ///  date_processing | timestamp without time zone |
            sentence+= "false, ";              ///  booked          | boolean                     |
            sentence+= "NULL";                 ///  date_booking    | timestamp without time zone |
            sentence+= ") ";
            sentence+= "RETURNING index";
            db->exec_sql(sentence, url_index);
        }
    }
    catch (trokam::exception& e)
    {
        throw e;
    }
    return url_index;
}

/***********************************************************************
 *
 **********************************************************************/
std::string trokam::db_handler::get_word_index(const std::string &word,
                                               const std::string &lang)
{
    std::string word_index;
    try
    {
        std::string sentence;
        sentence=  "SELECT word.index ";
        sentence+= "FROM word ";
        sentence+= "WHERE id='" + word + "' ";
        sentence+= "AND word.language='" + lang + "' ";
        db->exec_sql(sentence, word_index);
    }
    catch(const trokam::exception &e)
    {
        LOG(ERROR, "SQL Query: " << e.what());
    }
    return word_index;
}

/***********************************************************************
 *
 **********************************************************************/
std::string trokam::db_handler::insert_word(const std::string &word,
                                            const std::string &lang)
{
    std::string word_index;
    try
    {
        std::string sentence;
        sentence=  "INSERT INTO word ";
        sentence+= "VALUES (";
        sentence+= "DEFAULT, ";
        sentence+= "'" + word + "', ";
        sentence+= "'" + lang + "'";
        sentence+= ") ";
        sentence+= "RETURNING index";
        db->exec_sql(sentence, word_index);
    }
    catch(const trokam::exception &e)
    {
        LOG(ERROR, HERE << SEPARATOR << e.what());
    }
    return word_index;
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::insert_event(const std::string &url_index,
                                      const std::string &word_index,
                                      const std::string &count)
{
    try
    {
        std::string sentence;
        sentence=  "INSERT INTO occurrence ";
        sentence+= "VALUES (";
        sentence+= "DEFAULT, ";
        sentence+= word_index + ", ";
        sentence+= url_index + ", ";
        sentence+= count;
        sentence+= ")";
        db->exec_sql(sentence);
    }
    catch(const trokam::exception &e)
    {
        LOG(ERROR, HERE << SEPARATOR << e.what());
    }
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::store_words(const std::string &url_index,
                                     word_container &words)
{
    START;
    int new_words= 0;
    int total_words= 0;

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    ///
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::map<std::string, int>::iterator it;
    for(it= words.store.begin(); it!=words.store.end(); ++it)
    {
        std::string lang= words.language;
        std::string word= it->first;
        std::string count= converter::to_string(it->second);

        if(!trokam::token::it_has_any_chars_of(word, " .¡!¿?#&+$%/()*;=!☯™►◄▾▲←”“＼♣⇨●♦♥☠▌☼☆†<>\""))
        {
            std::string word_index= get_word_index(word, lang);
            if(word_index == EMPTY)
            {
                word_index= insert_word(word, lang);
                new_words++;
            }
            insert_event(url_index, word_index, count);
            total_words++;
        }
    }
    LOG(DEBUG, "number of new words= " << new_words);
    LOG(DEBUG, "number of total words= " << total_words);
    END;
}

/***********************************************************************
 *
 **********************************************************************/
std::string trokam::db_handler::get_domain_index(std::string domain)
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Find out if the domain exists already in the database
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string answer;
    std::string sentence;
    sentence=  "SELECT index ";
    sentence+= "FROM domain ";
    sentence+= "WHERE id='" + domain + "'";
    db->exec_sql(sentence, answer);
    return answer;
}

/***********************************************************************
 *
 **********************************************************************/
std::string trokam::db_handler::insert_domain(std::string domain)
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// The field 'id' of table 'domain' should accept unique values
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string answer;
    std::string sentence;
    sentence=  "INSERT INTO domain ";
    sentence+= "VALUES (";
    sentence+= "DEFAULT, ";
    sentence+= "'" + domain + "') ";
    sentence+= "RETURNING index";
    db->exec_sql(sentence, answer);
    return answer;
}

/***********************************************************************
 *
 **********************************************************************/
trokam::url_data trokam::db_handler::book_url(int mode)
{
    trokam::url_data ud;
    std::string nested;
    std::string sentence;
    std::string url_index;
    std::string domain;
    std::string path;

    switch (mode)
    {
        case RANDOM:
        {
            nested=  "(SELECT index ";
            nested+= "FROM path ";
            nested+= "WHERE processed=false ";
            nested+= "AND weird=false ";
            nested+= "AND booked=false ";
            nested+= "AND processed=false ";
            nested+= "ORDER BY RANDOM() ";
            nested+= "LIMIT 1) ";

            sentence=  "UPDATE path ";
            sentence+= "SET booked=true, ";
            sentence+= "date_booking=now() ";
            sentence+= "WHERE index=" + nested;
            sentence+= "RETURNING index; ";
            db->exec_sql(sentence, url_index);

            sentence=  "SELECT id ";
            sentence+= "FROM path ";
            sentence+= "WHERE index=" + url_index;
            db->exec_sql(sentence, path);

            sentence=  "SELECT domain.id ";
            sentence+= "FROM domain, path ";
            sentence+= "WHERE path.index=" + url_index + " ";
            sentence+= "AND path.domain_index=domain.index";
            db->exec_sql(sentence, domain);
        } break;

        case SEED:
        {
            nested=  "(SELECT index ";
            nested+= "FROM path ";
            nested+= "WHERE processed=false ";
            nested+= "AND weird=false ";
            nested+= "AND booked=false ";
            nested+= "AND processed=false ";
            nested+= "AND seed=true ";
            nested+= "ORDER BY RANDOM() ";
            nested+= "LIMIT 1) ";

            sentence=  "UPDATE path ";
            sentence+= "SET booked=true, ";
            sentence+= "date_booking=now() ";
            sentence+= "WHERE index=" + nested;
            sentence+= "RETURNING index; ";
            db->exec_sql(sentence, url_index);

            sentence=  "SELECT id ";
            sentence+= "FROM path ";
            sentence+= "WHERE index=" + url_index;
            db->exec_sql(sentence, path);

            sentence=  "SELECT domain.id ";
            sentence+= "FROM domain, path ";
            sentence+= "WHERE path.index=" + url_index + " ";
            sentence+= "AND path.domain_index=domain.index";
            db->exec_sql(sentence, domain);
        } break;
    }

    ud.url_index= url_index;
    ud.url= "http://" + domain + path;

    LOG(DEBUG, "booked URL: " << ud.url);
    LOG(DEBUG, "booked URL index=" << ud.url_index);

    return ud;
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::update_url_data(std::string url_index,
                                         std::string title,
                                         int number_of_words,
                                         int number_of_urls,
                                         int download_state)
{
    START;
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Remove the character ' because it causes problems
    /// in the sql sentence
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    boost::replace_all(title, "'", " ");

    std::string sentence;
    sentence=  "UPDATE path ";
    sentence+= "SET title='" + title + "', ";
    sentence+= "word_count=" + converter::to_string(number_of_words) + ", ";
    sentence+= "url_count=" + converter::to_string(number_of_urls) + ", ";
    sentence+= "download_state=" + converter::to_string(download_state) + " ";
    sentence+= "WHERE index=" + url_index;
    db->exec_sql(sentence);
    END;
}

/***********************************************************************
 *
 **********************************************************************/
bool trokam::db_handler::is_db_empty()
{
    START;
    std::string sentence;
    std::string answer;

    sentence=  "SELECT index ";
    sentence+= "FROM domain ";
    db->exec_sql(sentence, answer);
    if(answer != EMPTY)
    {
        LOG(ERROR, "table 'domain' is not empty");
        RETURN(FAIL);
    }
    LOG(DEBUG, "success: table 'domain' is empty");

    sentence=  "SELECT index ";
    sentence+= "FROM path ";
    db->exec_sql(sentence, answer);
    if(answer != EMPTY)
    {
        LOG(ERROR, "table 'path' is not empty");
        RETURN(FAIL);
    }
    LOG(DEBUG, "success: table 'path' is empty");

    sentence=  "SELECT index ";
    sentence+= "FROM word ";
    db->exec_sql(sentence, answer);
    if(answer != EMPTY)
    {
        LOG(ERROR, "table 'word' is not empty");
        RETURN(FAIL);
    }
    LOG(DEBUG, "success: table 'word' is empty");

    sentence=  "SELECT index ";
    sentence+= "FROM occurrence ";
    db->exec_sql(sentence, answer);
    if(answer != EMPTY)
    {
        LOG(ERROR, "table 'occurrence' is not empty");
        RETURN(FAIL);
    }
    LOG(DEBUG, "success: table 'occurrence' is empty");
    RETURN(SUCCESS);
}

/***********************************************************************
 *
 **********************************************************************/
int trokam::db_handler::get_number_of_words(std::string lang)
{
    std::string answer;
    std::string sentence;
    sentence=  "SELECT SUM(occurrence.events) ";
    sentence+= "FROM word, occurrence ";
    sentence+= "WHERE word.index=occurrence.word_index ";
    sentence+= "AND word.language='" + lang + "'";
    db->exec_sql(sentence, answer);
    return converter::string_to_int(answer);
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::get_occurrency_of_words(std::string lang)
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// 'occurrency_of_words' must be cleaned up before
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string sentence;
    sentence=  "SELECT word.index, SUM(occurrence.events) ";
    sentence+= "FROM word, occurrence ";
    sentence+= "WHERE word.index=occurrence.word_index ";
    sentence+= "AND word.language='" + lang + "' ";
    sentence+= "GROUP BY word.index ";
    db->exec_sql(sentence, occurrency_of_words);
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::get_frequency_of_words(int word_total, std::string lang)
{
    delete_word_freq(lang);
    store_words_freq(word_total);
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::delete_word_freq(std::string lang)
{
    std::string sentence;
    sentence=  "DELETE FROM stat ";
    sentence+= "USING word ";
    sentence+= "WHERE word.index=stat.word_index ";
    sentence+= "AND word.language='en'";
    db->exec_sql(sentence);
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::store_words_freq(int word_total)
{
    float total= word_total;

    for (std::map<std::string, int>::iterator it= occurrency_of_words.begin(); it!=occurrency_of_words.end(); ++it)
    {
        float freq= float(it->second)/total;
        std::string word_index= it->first;
        word_frequency_global[word_index]= freq;
    }
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::get_occurences_per_path(std::string lang)
{
    try
    {
        std::string sentence;
        sentence=  "SELECT path_index, SUM(events) ";
        sentence+= "FROM word, occurrence ";
        sentence+= "WHERE word.index=occurrence.word_index ";
        sentence+= "AND word.language='" + lang + "' ";
        sentence+= "GROUP BY occurrence.path_index ";
        sentence+= "HAVING SUM(events) > 200 ";
        sentence+= "ORDER BY sum DESC ";
        db->exec_sql(sentence, total_word_count_per_path);
    }
    catch(const trokam::exception &e)
    {
        LOG(ERROR, HERE << SEPARATOR << e.what());
    }
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::get_word_frequency_for_path(std::string lang)
{
    START;
    try
    {
        /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        ///
        /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        for (std::map<std::string, int>::iterator it= total_word_count_per_path.begin();
             it!=total_word_count_per_path.end();
             ++it)
        {
            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            ///
            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            std::map<std::string, int> word_occ_per_path;
            std::string sentence;
            sentence=  "SELECT occurrence.word_index, occurrence.events ";
            sentence+= "FROM occurrence ";
            sentence+= "WHERE occurrence.path_index=" + it->first;
            db->exec_sql(sentence, word_occ_per_path);

            LOG(DEBUG, "Processing path index= " << trokam::token::leftPadding(it->first, 8, " ") << "  url= " << get_url(it->first));

            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            ///
            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            for (std::map<std::string, int>::iterator nested_it= word_occ_per_path.begin();
                 nested_it!=word_occ_per_path.end();
                 ++nested_it)
            {
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// This is the 'local frequency'.
                /// Is the ratio of 'events of specific word' to
                /// the 'total count of words', for a specific path.
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                float file_freq= float(nested_it->second)/float(it->second);

                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// This 'global frequency'.
                /// This frequency of a word the taken into account
                /// all the words of all paths.
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                float global_freq= word_frequency_global[nested_it->first];

                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// This is the ratio of 'local frequency' to the
                /// 'global frequency'.
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                float ratio= file_freq/global_freq;

                std::string global_freq_text= converter::to_string(global_freq);
                std::string file_freq_text= converter::to_string(file_freq);
                std::string ratio_text= converter::to_string(ratio);

                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// word_index, path_index, stat_rank
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                insert_rank(nested_it->first, it->first, int(ratio));
            }
        }
    }
    catch(const trokam::exception &e)
    {
        LOG(ERROR, e.what());
    }
    END;
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::insert_rank(std::string word_index, std::string path_index, int stat_rank)
{
    try
    {
        std::string sentence;
        sentence=  "INSERT INTO rank ";
        sentence+= "VALUES (";
        sentence+= "DEFAULT, ";
        sentence+= word_index + ", ";
        sentence+= path_index + ", ";
        sentence+= converter::to_string(stat_rank) + ", ";
        sentence+= "0";
        sentence+= ")";
        db->exec_sql(sentence);
    }
    catch(const trokam::exception &e)
    {
        LOG(ERROR, HERE << SEPARATOR << e.what());
    }
}

/***********************************************************************
 *
 **********************************************************************/
std::string trokam::db_handler::get_word(const std::string word_index)
{
    std::string answer;
    std::string sentence;
    sentence=  "SELECT id ";
    sentence+= "FROM word ";
    sentence+= "WHERE word.index=" + word_index;
    db->exec_sql(sentence, answer);
    return answer;
}

/***********************************************************************
 *
 **********************************************************************/
std::string trokam::db_handler::get_url(const std::string url_index)
{
    std::string path;
    std::string domain;
    std::string sentence;

    sentence=  "SELECT id ";
    sentence+= "FROM path ";
    sentence+= "WHERE path.index=" + url_index;
    db->exec_sql(sentence, path);

    sentence=  "SELECT domain.id ";
    sentence+= "FROM domain, path ";
    sentence+= "WHERE domain.index=path.domain_index ";
    sentence+= "AND path.index=" + url_index;
    db->exec_sql(sentence, domain);

    std::string answer= "http://" + domain + path;
    return answer;
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::initialize()
{
    if(is_db_empty())
    {
        if(opt.seed_url() != "")
        {
            inject_one_url(opt.seed_url(), 1);
        }
        else if(opt.url_file() != "")
        {
            fill_with_urls_in_file(opt.url_file());
        }
        else
        {
            LOG(FATAL, "there is no seed URL or file with URLs supplied to initalize the database");
        }
    }
    else
    {
        LOG(FATAL, "database is not empty, could not initialize");
    }
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::fill_with_urls_in_file(std::string file_name)
{
    START;
    int count= 0;
    std::ifstream file_source(file_name.c_str());

    for(std::string line; std::getline(file_source, line);)
    {
        if(inject_one_url(line, 1) == "-1")
        {
            LOG(ERROR, "could not insert seed URL '" <<  line << "' into database");
        }
        else
        {
            LOG(TRACE, "success inserting seed URL '" <<  line << "' into database");
        }
        count++;
    }
    LOG(DEBUG, "URL inserted=" << count);
    END;
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::db_handler::query_word_set(const std::string &word_set, std::string &result)
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Limited to just two words
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int word_count=0;
    std::string word;
    typedef std::map<int, int> path_rank;
    std::vector<path_rank> query_result;
    boost::tokenizer<> tok(word_set);
    for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end(); ++beg)
    {
        word_count++;
        std::string token= *beg;
        word= token;

        path_rank pr;
        /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        /// All words are converted to lower case.
        /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        boost::algorithm::to_lower(token);
        query_one_word(token, pr);
        query_result.push_back(pr);
        if(word_count==2) break;
    }

    if(query_result.size()==1)
    {
        LOG(DEBUG, "results for one word");
        for(std::map<int, int>::reverse_iterator rit=query_result[0].rbegin(); rit!=query_result[0].rend(); ++rit)
        {
            std::string url_index= converter::to_string(rit->second);
            std::string snippet;
            format_result(url_index, word, snippet);
            result+= snippet;
        }
    }
    if(query_result.size()==2)
    {
        LOG(DEBUG, "results for two words");
        std::map<int, int> two_words_result;
        for(std::map<int, int>::iterator it= query_result[0].begin(); it!=query_result[0].end(); ++it)
        {
            for(std::map<int, int>::iterator nested= query_result[1].begin(); nested!=query_result[1].end(); ++nested)
            {
                if(it->second == nested->second)
                {
                    two_words_result[int(sqrt(float(it->first)*float(nested->first)))]= it->second;
                }
            }
        }

        for(std::map<int, int>::reverse_iterator rit=two_words_result.rbegin(); rit!=two_words_result.rend(); ++rit)
        {
            std::string url_index= converter::to_string(rit->second);

            std::string snippet;
            format_result(url_index, word, snippet);
            result+= snippet;
        }
    }

}

/***********************************************************************
 *
 *
 **********************************************************************/
void trokam::db_handler::query_one_word(const std::string &word, std::map<int, int> &pr)
{
    boost::scoped_ptr<pqxx::result> answer;
    try
    {
        LOG(DEBUG, "searching for= " << word);

        std::string sentence;
        sentence=  "SELECT path.index, rank.stat_rank ";
        sentence+= "FROM rank, word, path, domain ";
        sentence+= "WHERE rank.word_index=word.index ";
        sentence+= "AND rank.path_index=path.index ";
        sentence+= "AND path.domain_index=domain.index ";
        sentence+= "AND stat_rank >= 20 ";
        sentence+= "AND word.id='" + word + "' ";
        sentence+= "ORDER BY stat_rank DESC ";
        db->exec_sql(sentence, answer);

        for(pqxx::result::const_iterator c= answer->begin(); c != answer->end(); ++c)
        {
            pr[c[1].as(int())]= c[0].as(int());
        }
    }
    catch(const trokam::exception &e)
    {
        LOG(ERROR, HERE << SEPARATOR << e.what());
    }
}

/***********************************************************************
 *
 *
 **********************************************************************/
void trokam::db_handler::query_test()
{
    START;
    try
    {
        std::string answer;
        std::string sentence;
        sentence=  "SELECT COUNT(*) ";
        sentence+= "FROM path ";
        db->exec_sql(sentence, answer);

        LOG(DEBUG, "Result of query= " << answer);
    }
    catch(const trokam::exception &e)
    {
        LOG(ERROR, HERE << SEPARATOR << e.what());
    }
    END;
}

/***********************************************************************
 *
 *
 **********************************************************************/
void trokam::db_handler::get_title(const std::string &path_index, std::string &title)
{
    START;
    try
    {
        std::string sentence;
        sentence=  "SELECT title ";
        sentence+= "FROM path ";
        sentence+= "WHERE index=" + path_index;
        db->exec_sql(sentence, title);
        LOG(DEBUG, "path index= " << path_index);
        LOG(DEBUG, "title= " << title);
    }
    catch(const trokam::exception &e)
    {
        LOG(ERROR, HERE << SEPARATOR << e.what());
    }
    END;
}

/***********************************************************************
 *
 *
 **********************************************************************/
void trokam::db_handler::format_result(const std::string &url_index, const std::string &word, std::string &snippet)
{
    START;
    std::string url, title, link, file_name, line;

    url= get_url(url_index);
    html_handler::generate_link(url, link);
    get_title(url_index, title);
    if(title != "")
    {
        boost::replace_all(title, "\n", " ");
        boost::replace_all(title, "\r", " ");
        boost::replace_all(title, "\t", " ");
        boost::replace_all(title, "\v", " ");
        boost::replace_all(title, "\f", " ");
        boost::replace_all(title, "&", "and");
        boost::replace_all(title, "        ", " ");
        boost::replace_all(title, "       ", " ");
        boost::replace_all(title, "      ", " ");
        boost::replace_all(title, "     ", " ");
        boost::replace_all(title, "    ", " ");
        boost::replace_all(title, "   ", " ");
        boost::replace_all(title, "  ", " ");
        snippet+= "<strong>" + title + "</strong><br/>\n";
    }
    snippet+= link + "<br/>";

    file_name= opt.file_set() + "/" + trokam::token::leftPadding(url_index, 8, "0");

    text_processor::get_line_match(file_name, word, line);
    if(line != "")
    {
        snippet+= "<p>" + line + "</p>";
    }
    else
    {
        snippet+= "<br/>";
    }
    END;
}
