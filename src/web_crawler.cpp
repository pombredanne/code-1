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
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

/// C++
#include <iostream>
#include <exception>
#include <map>

/// Boost
#include <boost/bind.hpp>

/// Trokam
#include "exception.h"
#include "html_handler.h"
#include "log.h"
#include "machine.h"
#include "text_processor.h"
#include "token.h"
#include "web_crawler.h"

/***********************************************************************
 *
 *
 **********************************************************************/
bool trokam::web_crawler::stop;

/***********************************************************************
 *
 *
 **********************************************************************/
trokam::web_crawler::web_crawler(trokam::options &options): opt(options),
                                                            db(options)
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Install a catch for CRTL+C signal in order
    /// to quit the program in a known state
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    web_crawler::stop= false;
    signal(SIGINT, web_crawler::ctrl_c_signal);

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Creates a directory to keep the intermediate files of
    /// the processing. Using PID allows to have several instances
    /// of the same program simultaneously each one with its
    /// own set of intermediate files.
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string hp= trokam::machine::hostname_pid();
    base_directory= "/tmp/trokam/" + hp + "/";
    trokam::machine::mkdir(base_directory);

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// This file keep the file just downloaded
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    file_content_raw= base_directory + "file_content_raw";

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// This file keep the contents converted to text of the
    /// file downloaded
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    directory_files= opt.file_set() + "/";

    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Set a unique identifier for this client
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    db.set_client_identifier(hp);
}

/***********************************************************************
 *
 *
 **********************************************************************/
void trokam::web_crawler::ctrl_c_signal(int signal)
{
    web_crawler::stop= true;
    LOG(WARNING, "process is scheduled to stop");
}

/***********************************************************************
 *
 *
 **********************************************************************/
void trokam::web_crawler::run()
{
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Main loop for wep page fetching, analyzing and tracking of
    /// visited urls
    /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int page_count=0;
    while((page_count<opt.page_limit()) && !web_crawler::stop)
    {
        LOG(INFO, "page=" << page_count);

        /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        ///
        /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        page_count++;

        try
        {
            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            /// Book the URL for usage of this instance only
            /// it must be released if this do not reach the end of
            /// the procedure
            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            trokam::url_data ud;
            ud= db.book_url(opt.book_mode());
            LOG(INFO, "processing url= " << ud.url);
            LOG(INFO, "processing url index= " << ud.url_index);

            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            /// It fetches the web page and put into a local file
            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            int download_status= web.fetch(ud.url, file_content_raw);

            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            /// Different actions depending on the download_status.
            /// If download_status is 0 then it worked right, any
            /// other value means that it failed.
            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            if (download_status == 0)
            {
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// Extract the URLs of the content
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                std::vector<std::string> url_container;
                url_container= up.extract(file_content_raw);

                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// Inject the URLs into the database
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                db.inject_urls(url_container);

                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// Extract title
                /// Warning: this method works only on html files.
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                std::string title= trokam::html_handler::extract_title(file_content_raw);

                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// Convert the content (HTML, PDF, etc) to plain text
                /// and then process the content.
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                std::string file_content_text;
                file_content_text= directory_files + trokam::token::leftPadding(ud.url_index, 8, "0");
                LOG(DEBUG, "Data file: " << file_content_text);
                if(fc.convert_to_text(file_content_raw, file_content_text) == true)
                {
                    trokam::text_processor text(opt, file_content_text);
                    word_container words;
                    if(text.word_frequency(words))
                    {
                        db.store_words(ud.url_index, words);
                        db.update_url_data(ud.url_index, title, words.store.size(), url_container.size(), 0);
                    }
                    else
                    {
                        LOG(WARNING, "no words occurrence for url= " << ud.url);
                        LOG(WARNING, "no words occurrence for url index= " << ud.url_index);
                    }
                }
                else
                {
                    LOG(WARNING, "failed text conversion for url=" << ud.url);
                    LOG(WARNING, "failed text conversion for url index=" << ud.url_index);
                }
            }
            else
            {
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                /// Report failure of file fetching
                /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                LOG(WARNING, "failed download for url=" << ud.url);
                LOG(WARNING, "failed download for url index=" << ud.url_index);
                db.update_url_data(ud.url_index, "", 0, 0, download_status);
            }

            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            /// Mark the current URL has been processed
            /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            db.mark_as_used_for_word_stat(ud.url_index);
        }
        catch(trokam::exception& e)
        {
            LOG(ERROR, e.what());
        }
    }
    std::cout << "\n\n";
}
