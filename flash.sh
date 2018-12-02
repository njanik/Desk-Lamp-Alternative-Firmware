#!/bin/bash

IP="192.168.0.106"

if [ $# -eq 1 ]; then
  IP="$1"
fi


curl http://$IP/unlock?password=securitybyobscurity
echo
sleep 2

FW="$(ls /tmp/arduino_build_*/*.bin)" && curl -F "image=@$FW" $IP/update
echo
