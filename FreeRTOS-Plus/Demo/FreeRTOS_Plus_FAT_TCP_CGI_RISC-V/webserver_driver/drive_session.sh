#!/bin/bash
# This script runs a session script (something that generates HTTP requests)
# and feeds it into the webserver using netcat

session_script=$1
flag=$2
webserver_timeout=220

rm -f netcat_output.txt

echo "First, sleeping for $webserver_timeout seconds to let webserver get up..."

sleep $webserver_timeout

echo "Driving netcat with session script $session_script!"

sleep 1

$session_script $flag | nc -vv 127.0.0.1 5555 > netcat_output.txt
