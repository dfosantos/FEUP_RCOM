#! /bin/bash
/etc/init.d/networking restart
ifconfig eth0 up
ifconfig eth0 172.16.11.1/24
route add default gw 172.16.11.254
route add -net 172.16.10.0/24 gw 172.16.11.253
route add -net 172.16.1.0/24 gw 172.16.11.254
