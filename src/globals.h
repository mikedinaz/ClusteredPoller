#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <pthread.h>
#include "rtgconf.h"
#include "rtgtargets.h"
#include "cbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

// See globals.cpp for documentation on what these are used for.

        extern cbuffer *queries;
        extern unsigned long query_queue_depth;
        extern char *rtgconf_file;
        extern char *targets_file;
        extern int verbosity;
        extern int detach;
        extern int use_db;
        extern int allow_db_zero;
        extern unsigned max_queue_length;
        extern pthread_mutex_t global_lock;
        extern pthread_mutex_t db_list_lock;
        extern pthread_mutex_t cerr_lock;
        extern pthread_cond_t global_cond;
        extern unsigned active_threads;
        extern unsigned stat_inserts;
        extern unsigned stat_queries;
        extern unsigned stat_iterations;

#ifdef __cplusplus
}
#endif

#endif
