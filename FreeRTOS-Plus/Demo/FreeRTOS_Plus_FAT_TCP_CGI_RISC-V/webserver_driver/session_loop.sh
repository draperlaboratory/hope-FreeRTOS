#!/bin/bash

collect=$1

num_loops=$NUM_LOOPS

pages="index.html login.html search.html add-record.html dashboard.html register.html search-results.html update-user.html user-details.html"
timeout=$REQUEST_RATE

rm -f session_output.txt

echo "Running $num_lops loops." >> session_output.txt
echo "collect=$collect" >> session_output.txt

# Loop over pages once to get everything cached
for l in `seq 1 $num_loops` ;
do
    for p in $pages ;
    do
	echo "$p" >> session_output.txt
	cat request_page.txt | sed "s/PAGE/$p/"
	sleep $timeout
    done
done

echo "Now killing" >> session_output.txt

./kill_and_collect.sh $collect
