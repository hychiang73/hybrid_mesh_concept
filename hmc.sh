#
# A script for creating bridge and hyrbid mesh core
#
# NOTE: DON'T change the order of commands
#
#!/bin/sh

BR_IP=192.168.90.10

sudo rmmod hmc
echo "Remove HMC module"
sudo rmmod bridge
echo "Remove bridge module"
sudo insmod bridge.ko
echo "Insert bridge module"

sleep 1

echo "Add br0"
sudo brctl addbr br0
echo "Bind eth0 with br0"
sudo brctl addif br0 eth0
echo "Bring up br0"
sudo ifconfig br0 up
echo "Clean up eth0's ip addr"
sudo ifconfig eth0 0.0.0.0
echo "Assign ip addr ($BR_IP) for br0"
sudo ifconfig br0 $BR_IP

sleep 1

echo "Insert HMC module "
sudo insmod hmc.ko
