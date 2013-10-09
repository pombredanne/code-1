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

/// C++
#include <iostream>
#include <map>

/// Boost
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

/// Trokam
#include "text_processor.h"
#include "postgresql.h"
#include "converter.h"
#include "file_management.h"
#include "language.h"
#include "log.h"

/***********************************************************************
 *
 **********************************************************************/
trokam::text_processor::text_processor(std::string file_name)
{
    content= file_management::get_file_contents(file_name);
}

/***********************************************************************
 *
 **********************************************************************/
bool trokam::text_processor::word_frequency(word_container &words)
{
    std::string lang;
    int count= 0;

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Language detection
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    lang= language::detect(content);
    words.language= lang;

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Counting word frequency
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if ((lang != "SHORT") && (lang != "UNKNOWN") && (lang.length()==2))
    {
        boost::tokenizer<> tok(content);
        for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg)
        {
            std::string token= *beg;

            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            /// All words are converted to lower case.
            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            boost::algorithm::to_lower(token);

            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            /// It skip all strings of three or less characters
            /// that have numbers .
            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (token.length() <= 3)
			{
				unsigned int loc= token.find_first_of("1234567890");
				if (loc != std::string::npos)
				{
					continue;
				}
			}

            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            /// It skip all words with some strange characters.
            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			unsigned int loc= token.find_first_of("°¬!¡#$%&/()=?¿*+~[]{}^;:., <>©");
			if (loc != std::string::npos)
			{
				continue;
			}

            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            ///
            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            words.store[token]++;
            count++;
        }
		LOG(DEBUG, "tokens=" << count);
		LOG(DEBUG, "words=" << words.store.size());
		LOG(DEBUG, "lang=" << words.language);
        return true;
    }
    else
    {
       	LOG(WARNING, "tokens and words not identified, language UNKNOWN: " << lang);
        return false;
	}
}

/***********************************************************************
 *
 **********************************************************************/
void trokam::text_processor::get_line_match(const std::string &file_name,
                                            const std::string &word,
                                            std::string &line)
{
    START;
    LOG(DEBUG, "file name= " << file_name);
    std::string content= file_management::get_file_contents(file_name);
    if(content != "")
    {
        unsigned int ini= 0;
        unsigned int loc= content.find(word);

        if(loc != std::string::npos)
        {
            if(loc < 40)
            {
                ini= 0;
            }
            else
            {
                ini= loc-40;
            }

            line= content.substr(ini, 300);

            boost::replace_all(line, "<", "");
            boost::replace_all(line, ">", "");
            boost::replace_all(line, "\n", " ");
            boost::replace_all(line, "\r", " ");
            boost::replace_all(line, "\t", " ");
            boost::replace_all(line, "\v", " ");
            boost::replace_all(line, "\f", " ");
            boost::replace_all(line, "&", "and");
            boost::replace_all(line, "             ", " ");
            boost::replace_all(line, "            ", " ");
            boost::replace_all(line, "           ", " ");
            boost::replace_all(line, "          ", " ");
            boost::replace_all(line, "         ", " ");
            boost::replace_all(line, "        ", " ");
            boost::replace_all(line, "       ", " ");
            boost::replace_all(line, "      ", " ");
            boost::replace_all(line, "     ", " ");
            boost::replace_all(line, "    ", " ");
            boost::replace_all(line, "   ", " ");
            boost::replace_all(line, "  ", " ");

            // std::string word_tag= "<strong>" + word + "</strong>";
            // boost::replace_all(line, word, word_tag);
            // boost::replace_first(line, word, word_tag);
            line= "<em><small>" + line + "</small></em>";
        }
    }
    END;
}
