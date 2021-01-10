#!/bin/bash

MULTICAST=239.255.0.0

if [ $# -ne 2 ]
then
	echo "Need ammount of nodes and error probability as argument"
	exit 1
fi

re='^[0-9]+$'
node_ammount=$1

if ! [[ $1 =~ $re ]] ; then
   echo "error: \"$node_ammount\" is not a number"
   exit 1
fi

if [ $node_ammount -le 0 ]
then
	echo "Node ammount must be greater than 0"
	exit 1
fi

percentage_error=$2

if ! [[ $2 =~ $re ]] ; then
   echo "error: \"$percentage_error\" is not a number"
   exit 1
fi

if [ $percentage_error -ge 100 ] || [ $percentage_error -lt 0 ]
then
	echo "Error percentage must be less than 100 and greater or equal to 0"
	exit 1
fi

echo "Launching $node_ammount nodes with packet drop probability of $percentage_error"

nodes=$(($node_ammount + 1))

# clean
function clean(){
	echo Cleaning
	# Sub interface route
	sudo route del \-net 127.0.0.0 netmask 255.255.255.255 dev lo
	# Multicast route
	sudo route del \-net MULTICAST netmask 255.255.255.255 dev lo

	for i in $( eval echo {2..$interfaces} )
	do
		sudo ifconfig lo:$i down
	done
	kill 0
}

trap 'clean' INT

# Sub interface route
sudo route add \-net 127.0.0.0 netmask 255.255.255.255 dev lo
# Multicast route
sudo route add \-net MULTICAST netmask 255.255.255.255 dev lo

# Set interface and respective sub-interfaces for multicast
sudo ifconfig lo multicast

interfaces=$(($nodes + 1))
for i in $( eval echo {2..$interfaces} )
do
	sudo ifconfig lo:$i 127.0.0.$i netmask 255.255.255.255 up
	sudo ifconfig lo:$i multicast
done

./make.sh

# turn nodes on, in separate terminals
for i in $( eval echo {2..$nodes} )
do
	#echo $i
	#if [ $i == 2 ]
	#then
	#	urxvt -title paxos_node_$i -e ./paxos_node/paxos_node $i Y $node_ammount &
	#else
	#	urxvt -title paxos_node_$i -e ./paxos_node/paxos_node $i N $node_ammount &
	#fi
	urxvt -title paxos_node_$i -e ./paxos_node/paxos_node $i $node_ammount $percentage_error &
done


# turn client on
./paxos_client/paxos_client $(($nodes + 1))







