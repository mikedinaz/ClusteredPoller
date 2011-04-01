#include <stdio.h>

#include "util.h"
#include "globals.h"
#include "monitor.h"
#include "poller.h"
#include "database.h"
#include "version.h"

void help();

// Setup and initialization.

// Display usage.
void help()
{
        fprintf(stderr, "clpoll %s\n", CLPOLL_VERSION);
        fprintf(stderr, " -c <file>   Specify configuration file [%s]\n", rtgconf_file);
        fprintf(stderr, " -D          Don't detach, run in foreground\n");
        fprintf(stderr, " -d          Disable database inserts\n");
        fprintf(stderr, " -t <file>   Specify target file [%s]\n", targets_file);
        fprintf(stderr, " -v          Increase verbosity\n");
        fprintf(stderr, " -z          Database zero delta inserts\n");
        fprintf(stderr, " -ql <num>   Maximum database queue length [%d]\n", max_queue_length);
        fprintf(stderr, " Copyright (c) 2009-2011 Jakob Borg\n");
}

void run_threads(rtgtargets *targets, rtgconf *config)
{
        // Calculate number of database writers needed. This is just a guess.
        unsigned num_dbthreads = config->threads / 8;
        num_dbthreads = num_dbthreads ? num_dbthreads : 1;

        cllog(1, "Starting %d poller threads.", config->threads);
        mt_threads *poller_threads = mt_threads_create(config->threads);
        for (unsigned i = 0; i < config->threads; i++) {
                poller_ctx *ctx = (poller_ctx *)malloc(sizeof(poller_ctx));
                ctx->stride = config->threads;
                ctx->targets = targets;
                poller_threads->contexts[i].param = ctx;
        }
        mt_threads_start(poller_threads, poller_run);

        cllog(1, "Starting %d database threads.", num_dbthreads);
        mt_threads *database_threads = mt_threads_create(num_dbthreads);
        for (unsigned i = 0; i < num_dbthreads; i++) {
                database_ctx *ctx = (database_ctx *)malloc(sizeof(database_ctx));
                ctx->config = config;
                database_threads->contexts[i].param = ctx;
        }
        mt_threads_start(database_threads, database_run);

        cllog(1, "Starting monitor thread.");
        mt_threads *monitor_threads = mt_threads_create(1);
        monitor_ctx *ctx = (monitor_ctx *)malloc(sizeof(monitor_ctx));
        ctx->interval = config->interval;
        monitor_threads->contexts[0].param = ctx;
        mt_threads_start(monitor_threads, monitor_run);

        mt_threads_join(database_threads);
        mt_threads_join(poller_threads);
        mt_threads_join(monitor_threads);
}

#ifndef TESTSUITE
// Parse command line, load caonfiguration and start threads.
int main (int argc, char *const argv[])
{
        if (argc < 2) {
                help();
                exit(0);
        }
        for (int i = 1; i < argc; i++) {
                char *arg = argv[i];
                if (!strcmp(arg, "-v"))
                        verbosity++;
                else if (!strcmp(arg, "-D"))
                        detach = 0;
                else if (!strcmp(arg, "-d"))
                        use_db = 0;
                else if (!strcmp(arg, "-z"))
                        allow_db_zero = 1;
                else if (!strcmp(arg, "-h")) {
                        help();
                        exit(0);
                } else if (!strcmp(arg, "-c")) {
                        i++;
                        rtgconf_file = argv[i];
                } else if (!strcmp(arg, "-t")) {
                        i++;
                        targets_file = argv[i];
                } else if (!strcmp(arg, "-ql")) {
                        i++;
                        max_queue_length = atoi(argv[i]);
                }
        }

        // Read rtg.conf
        rtgconf *config = rtgconf_create(rtgconf_file);
        // Read targets.cfg
        rtgtargets *targets = rtgtargets_parse(targets_file, config);

        if (targets->ntargets == 0) {
                cllog(0, "No targets, so nothing to do.");
                exit(-1);
        }

        cllog(1, "Polling every %d seconds.", config->interval);

        if (detach)
                daemonize();

        run_threads(targets, config);
        return 0;
}
#endif
