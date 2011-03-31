#ifndef DATABASE_H_
#define DATABASE_H_

#include <string>
#include "rtgconf.h"
#include "multithread.h"

namespace mysqlpp
{
class Connection;
}

class Database : public Multithread
{
private:
        static std::string dequeue_query();
        static unsigned queries_size();
        static mysqlpp::Connection* connection(int my_id);
        static rtgconf* config;

protected:
        void create_thread(pthread_t* thread, int* thread_id);
        static void* run(void* id_ptr);

public:
        Database(int num_threads, rtgconf* config);
        virtual ~Database();
};

#endif /* DATABASE_H_ */
