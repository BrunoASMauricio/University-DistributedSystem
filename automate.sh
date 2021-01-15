#!/bin/bash

nodes_min=1
nodes_max=2

prob_min=0
# how much to increase in range
prob_incr=2
prob_max=4

mkdir -p ./test_output
if [[ "$EUID" = 0 ]]; then
    echo "(1) already root"
else
    sudo -k # make sure to ask for password on next sudo
    if sudo true; then
        echo "(2) correct password"
    else
        echo "(3) wrong password"
        exit 1
    fi
fi

rm -rf ./test_output_old
mv ./test_output/* ./test_output_old
rm -rf ./test_output

for nodes in $( eval echo {$nodes_min..$nodes_max} )
do
	for prob in $( eval echo {$prob_min..$prob_max..$prob_incr} )
	do
		echo Starting simulation with $nodes nodes and packet loss chance of $prob%
		dir=./test_output/$nodes\_$prob
		# Clean dir
		rm -rf $dir
		mkdir -p $dir

		urxvt -title paxos -e ./launch.sh $nodes $prob &

		sleep 5

		pkill paxos
		# Always clean to remove pending messages
		./clean.sh $nodes

		# Move log files
		mv node_* $dir
	done
done
