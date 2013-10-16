# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#                            T R O K A M
#                  Free and Open Source Search Engine
#
# Documentation  http://www.trokam.com/
# Development    http://www.github.com/trokam
#
# Copyright (C) 2013, Nicolas Slusarenko
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CC=g++

CPPFLAGS=-c -Wall -I./include

LDFLAGS=-g

LDLIBS=-lboost_system -lboost_regex -lboost_program_options -lboost_thread -lpqxx -lexttextcat -lmagic

BIN=./bin

COMMON_BIN=/usr/local/bin

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# db feeder
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SOURCES_DB_FEEDER= src/converter.cpp \
		           src/db_handler.cpp \
                   src/exception.cpp \
                   src/machine.cpp \
                   src/file_converter.cpp \
                   src/file_management.cpp \
                   src/html_handler.cpp \
                   src/main_db_feeder.cpp \
                   src/language.cpp \
                   src/log.cpp \
                   src/options.cpp \
                   src/postgresql.cpp \
                   src/text_processor.cpp \
                   src/token.cpp \
                   src/url_processor.cpp \
                   src/web_crawler.cpp \
                   src/web_handler.cpp

OBJECTS_DB_FEEDER=$(SOURCES_DB_FEEDER:.cpp=.o)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# db init
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SOURCES_DB_INIT=   src/converter.cpp \
                   src/db_handler.cpp \
                   src/exception.cpp \
                   src/file_management.cpp \
                   src/html_handler.cpp \
                   src/machine.cpp \
                   src/main_db_init.cpp \
                   src/language.cpp \
                   src/log.cpp \
                   src/options.cpp \
                   src/postgresql.cpp \
                   src/text_processor.cpp \
                   src/token.cpp \

OBJECTS_DB_INIT=$(SOURCES_DB_INIT:.cpp=.o)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# db rank
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SOURCES_DB_RANK=   src/converter.cpp \
                   src/db_handler.cpp \
                   src/exception.cpp \
                   src/file_management.cpp \
                   src/html_handler.cpp \
                   src/machine.cpp \
                   src/main_db_rank.cpp \
                   src/language.cpp \
                   src/log.cpp \
				   src/text_processor.cpp \
                   src/options.cpp \
                   src/postgresql.cpp \
                   src/token.cpp \

OBJECTS_DB_RANK=$(SOURCES_DB_RANK:.cpp=.o)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# url_extract
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SOURCES_URL_EXTRACT= src/converter.cpp \
					 src/exception.cpp \
                     src/file_management.cpp \
                     src/log.cpp \
                     src/options.cpp \
                     src/main_url_extract.cpp \
                     src/token.cpp \
                     src/url_processor.cpp \

OBJECTS_URL_EXTRACT=$(SOURCES_URL_EXTRACT:.cpp=.o)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# query_server
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SOURCES_QUERY_SERVER= src/converter.cpp \
                      src/db_handler.cpp \
					  src/exception.cpp \
                      src/file_management.cpp \
                      src/html_handler.cpp \
                      src/log.cpp \
                      src/options.cpp \
                      src/postgresql.cpp \
                      src/main_query_server.cpp \
                      src/language.cpp \
					  src/text_processor.cpp \
                      src/token.cpp \
                      src/query_server.cpp \

OBJECTS_QUERY_SERVER=$(SOURCES_QUERY_SERVER:.cpp=.o)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# query_client
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SOURCES_QUERY_CLIENT= src/converter.cpp \
                      src/db_handler.cpp \
					  src/exception.cpp \
                      src/file_management.cpp \
                      src/html_handler.cpp \
                      src/log.cpp \
                      src/options.cpp \
                      src/main_query_client.cpp \
                      src/language.cpp \
					  src/text_processor.cpp \
                      src/postgresql.cpp \
                      src/query_client.cpp \
                      src/token.cpp \

OBJECTS_QUERY_CLIENT=$(SOURCES_QUERY_CLIENT:.cpp=.o)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# targets
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

all: make_dir db_feeder db_init db_rank query_server query_client

db_feeder: $(OBJECTS_DB_FEEDER)
	$(CC) $(LDFLAGS) $(OBJECTS_DB_FEEDER) -o $(BIN)/$@ $(LDLIBS)

db_init: $(OBJECTS_DB_INIT)
	$(CC) $(LDFLAGS) $(OBJECTS_DB_INIT) -o $(BIN)/$@ $(LDLIBS)

db_rank:  $(OBJECTS_DB_RANK)
	$(CC) $(LDFLAGS) $(OBJECTS_DB_RANK) -o $(BIN)/$@ $(LDLIBS)

query_server: $(OBJECTS_QUERY_SERVER)
	$(CC) $(LDFLAGS) $(OBJECTS_QUERY_SERVER) -o $(BIN)/$@ $(LDLIBS)

query_client: $(OBJECTS_QUERY_CLIENT)
	$(CC) $(LDFLAGS) $(OBJECTS_QUERY_CLIENT) -o $(BIN)/$@ $(LDLIBS)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# These are the list of files and the automatically
# generated rules to build them
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DEPENDENCIES= $(OBJECTS:.o=.d)

include $(DEPENDENCIES:.cpp=.d)

%.d: %.cpp
		@set -e; rm -f $@; \
		$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PHONY : make_dir
make_dir :
	mkdir -p $(BIN)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PHONY : install
install :
	sudo cp ./bin/* $(COMMON_BIN)
	sudo chmod a+x -R $(COMMON_BIN)
	sudo chown root:root -R $(COMMON_BIN)

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PHONY : clean
clean :
	-rm -f bin/*
	-rm -f src/*.o
