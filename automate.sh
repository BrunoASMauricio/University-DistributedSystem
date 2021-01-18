#!/bin/bash

nodes_min=3
nodes_max=4

prob_min=0
# how much to increase in range
prob_incr=25
prob_max=25

runs=5

mkdir -p ./test_output

rm -rf ./test_output_old
mv ./test_output/* ./test_output_old
rm -rf ./test_output

function clean(){
	echo Cleaning
	./clean.sh $nodes_max
	kill 0
	exit 0
}
trap 'clean' SIGINT
for nodes in $( eval echo {$nodes_min..$nodes_max} )
do
	for prob in $( eval echo {$prob_min..$prob_max..$prob_incr} )
	do
		echo Starting simulation with $nodes nodes and packet loss chance of $prob%
		for run in $( eval echo {0..$runs} )
		do
			dir=./test_output/$nodes\_$prob/T_$run
			# Clean dir
			rm -rf $dir
			mkdir -p $dir

			urxvt -title paxos -e ./launch.sh $nodes $prob &

			sleep 40

			pkill paxos
			# Always clean to remove pending messages
			./clean.sh $nodes

			# Move log files
			mv ./node_* $dir

		done
	done
done
