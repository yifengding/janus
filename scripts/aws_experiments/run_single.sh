./run_all.py -hh config/aws_hosts.yml -cc config/concurrent_10.yml -cc config/tpca_fixed.yml -cc config/tapir.yml -b tpca -m brq:brq -m 2pl_ww:multi_paxos -m occ:multi_paxos -m tapir:tapir -c 1 -c 2 -c 4 -c 8 -c 16 -c 32 -c 64 -s 3 -r 3 -d 90 single_datacenter_fixed
./run_all.py -hh config/aws_hosts.yml -cc config/concurrent_10.yml -cc config/tpca_fixed.yml -cc config/tapir.yml -b tpca -m brq:brq -m 2pl_ww:multi_paxos -m occ:multi_paxos -m tapir:tapir -c 1 -c 2 -c 4 -c 8 -c 16 -c 32 -c 64 -z 0.0 0.25 0.5 0.75 1.0 -s 3 -r 3 -d 90 single_datacenter_zipf
./run_all.py -hh config/aws_hosts.yml -cc config/concurrent_10.yml -cc config/tpcc.yml -cc config/tapir.yml -b tpcc -m brq:brq -m 2pl_ww:multi_paxos -m occ:multi_paxos -m tapir:tapir -c 1 -c 2 -c 4 -c 8 -c 16 -c 32 -c 64 -s 3 -r 3 -d 90 single_datacenter_tpcc
