#!/bin/bash

MULTICAST=239.255.0.0

if [ $# -ne 1 ]
then
	echo "Need ammount of nodes as argument"
	exit 1
fi

re='^[0-9]+$'
nodes=$1

if ! [[ $1 =~ $re ]] ; then
   echo "error: \"$nodes\" is not a number"
   exit 1
fi

if [ $nodes -le 0 ]
then
	echo "Node ammount must be greater than 0"
	exit 1
fi

nodes=$(($nodes + 1))

# clean
function clean(){
	echo Cleaning
	# Sub interface route
	sudo route del \-net 127.0.0.0 netmask 255.255.255.0 dev lo
	# Multicast route
	sudo route del \-net MULTICAST netmask 255.255.255.0 dev lo

	for i in $( eval echo {2..$interfaces} )
	do
		sudo ifconfig lo:$i down
	done
	kill 0
}

trap 'clean' INT

# Sub interface route
sudo route add \-net 127.0.0.0 netmask 255.255.255.0 dev lo
# Multicast route
sudo route add \-net MULTICAST netmask 255.255.255.0 dev lo

# Set interface and respective sub-interfaces for multicast
sudo ifconfig lo multicast

interfaces=$(($nodes + 1))
for i in $( eval echo {2..$interfaces} )
do
	sudo ifconfig lo:$i 127.0.0.$i netmask 255.0.0.0 up
	sudo ifconfig lo:$i multicast
done

./make.sh

# turn nodes on, in separate terminals
for i in $( eval echo {2..$nodes} )
do
	echo $i
	if [ $i == 2 ]
	then
		urxvt -title paxos_node_$i -e ./paxos_node/paxos_node $i Y &
	else
		urxvt -title paxos_node_$i -e ./paxos_node/paxos_node $i N &

	fi
done


# turn client on
./paxos_client/paxos_client $(($nodes + 1))







