./run_all.py -hh config/aws_hosts.yml -cc config/concurrent_50.yml -cc config/tpca_fixed.yml -cc config/tapir.yml -b tpca -m brq:brq -m 2pl_ww:multi_paxos -m occ:multi_paxos -m tapir:tapir -c 1 -c 2 -c 4 -c 8 -c 16 -c 32 -s 1 -r 3 -d 90 multi_datacenter_fixed
./run_all.py -hh config/aws_hosts.yml -cc config/concurrent_50.yml -cc config/tpcc.yml -cc config/tapir.yml -b tpcc -m brq:brq -m 2pl_ww:multi_paxos -m occ:multi_paxos -m tapir:tapir -c 1 -c 2 -c 4 -c 8 -c 16 -c 32 -s 1 -r 3 -d 90 multi_datacenter_tpcc
