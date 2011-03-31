#include <string>

#include "types.h"
#include "UnitTest++.h"
#include "rtgconf.h"
#include "rtgtargets.h"

using namespace std;

SUITE(QuickTests)
{
        TEST(ParseNonexistentTargets) {
                RTGConf conf("test/example-rtg.conf");
                RTGTargets hosts("test/none.cfg", &conf);
                CHECK_EQUAL((size_t)0, hosts.size());
        }

        TEST(ParseNewStyleTargets) {
                RTGConf conf("test/example-rtg.conf");
                RTGTargets hosts("test/example-targets.cfg", &conf);
                CHECK_EQUAL((size_t)2, hosts.size()); // Two hosts
                CHECK_EQUAL((size_t)2, hosts[0].rows.size()); // Two rows for host one
                CHECK_EQUAL((size_t)2, hosts[1].rows.size()); // Two rows for host two
                CHECK_EQUAL(2u, hosts.results.hosts);
                CHECK_EQUAL(4u, hosts.results.targets);
        }

        TEST(ParseOldStyleTargets) {
                RTGConf conf("test/example-rtg.conf");
                RTGTargets hosts("test/oldstyle-targets.cfg", &conf);
                CHECK_EQUAL((size_t)2, hosts.size()); // Two hosts
                CHECK_EQUAL((size_t)8, hosts[0].rows.size()); // Eight rows for host one
                CHECK_EQUAL((size_t)7, hosts[1].rows.size()); // Seven rows for host two
                CHECK_EQUAL(2u, hosts.results.hosts);
                CHECK_EQUAL(8u+7u, hosts.results.targets);
        }

        TEST(ParseNewStyleTargetsHost) {
                RTGConf conf("test/example-rtg.conf");
                RTGTargets hosts("test/example-targets.cfg", &conf);
                CHECK_EQUAL("172.16.1.1", hosts[0].host);
                CHECK_EQUAL("172.16.1.2", hosts[1].host);
                CHECK_EQUAL("c0mmun1ty", hosts[0].community);
                CHECK_EQUAL("f00barb4z", hosts[1].community);
                CHECK_EQUAL(1, hosts[0].snmpver);
                CHECK_EQUAL(2, hosts[1].snmpver);
        }

        TEST(ParseOldStyleTargetsHost) {
                RTGConf conf("test/example-rtg.conf");
                RTGTargets hosts("test/oldstyle-targets.cfg", &conf);
                CHECK_EQUAL("172.16.17.18", hosts[0].host);
                CHECK_EQUAL("172.16.17.20", hosts[1].host);
                CHECK_EQUAL("public", hosts[0].community);
                CHECK_EQUAL("otherc", hosts[1].community);
                CHECK_EQUAL(2, hosts[0].snmpver);
                CHECK_EQUAL(2, hosts[1].snmpver);
        }

        TEST(ParseNewsStyleTargetsRow) {
                RTGConf conf("test/example-rtg.conf");
                RTGTargets hosts("test/example-targets.cfg", &conf);
                CHECK_EQUAL(".1.3.6.1.2.1.2.2.1.16.1001002", hosts[0].rows[0].oid);
                CHECK_EQUAL("ifOutOctets_362", hosts[0].rows[0].table);
                CHECK_EQUAL(4309u, hosts[0].rows[0].id);
                CHECK_EQUAL(57120000u/conf.interval, hosts[0].rows[0].speed);
                CHECK_EQUAL(32u, hosts[0].rows[0].bits);
        }

        TEST(ParseOldStyleTargetsRow) {
                RTGConf conf("test/example-rtg.conf");
                RTGTargets hosts("test/oldstyle-targets.cfg", &conf);
                CHECK_EQUAL("1.3.6.1.2.1.2.2.1.10.13", hosts[0].rows[0].oid);
                CHECK_EQUAL("data", hosts[0].rows[0].table);
                CHECK_EQUAL(55u, hosts[0].rows[0].id);
                CHECK_EQUAL((unsigned)10e9/8/conf.interval, hosts[0].rows[0].speed);
                CHECK_EQUAL(32u, hosts[0].rows[0].bits);
        }
}
