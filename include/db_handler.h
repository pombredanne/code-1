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

#ifndef TROKAM_DB_HANDLER_H
#define TROKAM_DB_HANDLER_H

/// C++
#include <string>
#include <map>
#include <vector>

/// Boost
#include <boost/array.hpp>

/// Trokam
#include "options.h"
#include "postgresql.h"
#include "text_processor.h"

#define RANDOM            0
#define DOMAIN            1
#define LESS_POPULATED    2
#define SEED              3

/***********************************************************************
 *
 *
 **********************************************************************/
namespace trokam
{
    struct url_data
    {
        std::string url_index;
        std::string url;
    };

    struct pair
    {
        std::string word_index;
        int value;
    };

    class db_handler
    {
        public:
            db_handler(trokam::options &opt);
            ~db_handler();

            void initialize();
            bool is_db_empty();
            void set_client_identifier(const std::string &id);
            bool already_used_for_word_stat(std::string url);
            void mark_as_used_for_word_stat(std::string url);
            std::string pickup_one_url();
            void inject_urls(std::vector<std::string> url_container);
            std::string inject_one_url(std::string url, int seed= 0);

            void store_words(const std::string &url_index,
                             word_container &words);

            std::string get_domain_index(std::string domain);
            url_data book_url(int mode=RANDOM);
            void update_url_data(std::string index_url,
                                 std::string title,
                                 int number_of_words,
                                 int number_of_urls,
                                 int download_status);
            int insert_first_url(std::string seed_url);

            std::string insert_domain(std::string domain);

            std::string get_word_index(const std::string &word,
                                       const std::string &lang);
            std::string insert_word(const std::string &word,
                                    const std::string &lang);

            int get_number_of_words(std::string lang);
            void get_occurrency_of_words(std::string lang);
            void get_frequency_of_words(int word_total, std::string lang);

            void insert_event(const std::string &url_index,
                              const std::string &word_index,
                              const std::string &count);

            void get_occurences_per_path(std::string lang);
            void get_word_frequency_for_path(std::string lang);

            std::string get_word(const std::string index_word);
            std::string get_url(const std::string index_url);
            void fill_with_urls_in_file(std::string file_name);

            void insert_rank(std::string word_index, std::string path_index, int stat_rank);

            void query_word_set(const std::string &word_set, std::string &result);
            void query_one_word(const std::string &word, std::map<int, int> &pr);
            void query_test();
            void get_title(const std::string &path_index, std::string &title);
            void format_result(const std::string &url_index, const std::string &word, std::string &snippet);

        private:
            trokam::postgresql *db;
            trokam::options opt;

            std::string feeder;
            std::string domain_index;
            std::string fixed_domain;

            std::map<std::string, int> occurrency_of_words;
            std::map<std::string, int> total_word_count_per_path;
            std::map<std::string, float> word_frequency_global;

            void delete_word_freq(std::string lang);
            void store_words_freq(int word_total);
    };
}

#endif /// TROKAM_DB_HANDLER_H
