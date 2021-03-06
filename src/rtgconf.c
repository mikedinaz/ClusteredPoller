/*
 *  ClusteredPoller
 *
 *  Created by Jakob Borg.
 *  Copyright 2011 Nym Networks. See LICENSE for terms.
 */

#include "rtgconf.h"

#include "cllog.h"
#include "globals.h"
#include "xmalloc.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct rtgconf *rtgconf_create(const char *filename)
{
        char buffer[513];
        char *line;
        struct rtgconf *conf;
        FILE *fileptr;

        fileptr = fopen(filename, "rb");
        if (!fileptr) {
                cllog(0, "Couldn't open %s for reading.", filename);
                return NULL;
        }

        conf = (struct rtgconf *) xmalloc(sizeof(struct rtgconf));
        conf->interval = DEFAULT_INTERVAL;
        conf->threads = DEFAULT_THREADS;
        conf->dbhost = NULL;
        conf->database = NULL;
        conf->dbuser = NULL;
        conf->dbpass = NULL;
        conf->num_dbthreads = DEFAULT_NUM_DBTHREADS;

        while ((line = fgets(buffer, 512, fileptr))) {
                const char *sep = " \t\n";
                char *token;

                /* Terminate line at first comment character. */
                char *comment_begin = strchr(line, '#');
                if (comment_begin)
                        *comment_begin = '\0';

                /* Ignore empty lines. */
                if (strlen(line) == 0)
                        continue;

                token = strtok(line, sep);
                /* Lowercase token. */
                if (token) {
                        int i;
                        for (i = 0; token[i] != 0; i++)
                                token[i] = tolower(token[i]);
                        if (!strcmp(token, "interval"))
                                conf->interval = atoi(strtok(NULL, sep));
                        else if (!strcmp(token, "db_host"))
                                conf->dbhost = strdup(strtok(NULL, sep));
                        else if (!strcmp(token, "db_database"))
                                conf->database = strdup(strtok(NULL, sep));
                        else if (!strcmp(token, "db_user"))
                                conf->dbuser = strdup(strtok(NULL, sep));
                        else if (!strcmp(token, "db_pass"))
                                conf->dbpass = strdup(strtok(NULL, sep));
                        else if (!strcmp(token, "threads"))
                                conf->threads = atoi(strtok(NULL, sep));
                }
        }
        fclose(fileptr);
        return conf;
}

int rtgconf_verify(struct rtgconf *config)
{
        if (!config->dbhost) {
                cllog(0, "Missing directive for database host in RTG config file.");
                return 0;
        }
        if (!config->database) {
                cllog(0, "Missing directive for database name in RTG config file.");
                return 0;
        }
        if (!config->dbuser) {
                cllog(0, "Missing directive for database user in RTG config file.");
                return 0;
        }
        return 1;
}

void rtgconf_free(struct rtgconf *config)
{
        free(config->dbhost);
        free(config->dbuser);
        free(config->dbpass);
        free(config->database);
        free(config);
}
