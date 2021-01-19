
rm -r ./node_stats
mkdir ./node_stats

cd ./paxos_node
make
cd ..

cd ./paxos_client
make
cd ..
