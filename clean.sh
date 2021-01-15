#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Need ammount of nodes"
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

interfaces=$(($node_ammount + 2))
echo Cleaning $interfaces

sudo route del \-net 127.0.0.0 netmask 255.255.255.255 dev lo
# Multicast route
sudo route del \-net MULTICAST netmask 255.255.255.255 dev lo

for i in $( eval echo {2..$interfaces} )
do
	sudo ifconfig lo:$i down
done
