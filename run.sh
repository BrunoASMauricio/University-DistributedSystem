#!/bin/bash

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

# Sub interface route
sudo route add \-net 127.0.0.0 netmask 255.255.255.0 dev lo
# Multicast route
sudo route add \-net 239.255.0.0 netmask 255.255.255.0 dev lo

# Set interface and respective sub-interfaces for multicast
sudo ifconfig lo multicast

for i in $( eval echo {2..$nodes} )
do
	sudo ifconfig lo:$i 127.0.0.$i netmask 255.0.0.0 up
	sudo ifconfig lo:$i multicast
done




exit 0

