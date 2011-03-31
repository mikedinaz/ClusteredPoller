#include "UnitTest++.h"
#include "types.h"
#include "queryablehost.h"
#include "rtgconf.h"
#include "rtgtargets.h"

extern "C" void mock_set_speed(unsigned int newspeed); // From snmp-mock.cpp

SUITE(LongTests)
{
        TEST(MeasureOneHostsAt10MbpsForTenSeconds) {
                mock_set_speed(1000000 / 8);
                rtgconf* conf = rtgconf_create("test/example-rtg.conf");
                RTGTargets hosts("test/example-targets.cfg", conf);
                ResultCache cache;
                QueryableHost qh(hosts[0], cache);
                std::vector<std::string> queries = qh.get_inserts();
                CHECK_EQUAL((size_t)0, queries.size()); // No inserts first iteration
                sleep(10);
                queries = qh.get_inserts();
                CHECK_EQUAL((size_t)1, queries.size()); // One insert next iteration
                size_t pos = queries[0].find(", 1250000, 125000)");
                CHECK(pos != std::string::npos);
                pos = queries[0].find(", 1250000, 125000)", pos+1);
                CHECK(pos != std::string::npos);
        }

        TEST(MeasureOneHostAt100MbpsForOneInterval) {
                mock_set_speed(100000000 / 8);
                rtgconf* conf = rtgconf_create("test/example-rtg.conf");
                RTGTargets hosts("test/example-targets.cfg", conf);
                ResultCache cache;
                QueryableHost qh(hosts[0], cache);
                std::vector<std::string> queries = qh.get_inserts();
                CHECK_EQUAL((size_t)0, queries.size()); // No inserts first iteration
                sleep(conf->interval);
                queries = qh.get_inserts();
                CHECK_EQUAL((size_t)0, queries.size()); // No inserts next iteration due to too high speed
        }
}
