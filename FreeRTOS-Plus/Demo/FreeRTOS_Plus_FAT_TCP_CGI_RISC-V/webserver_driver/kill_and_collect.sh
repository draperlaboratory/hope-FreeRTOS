#/bin/bash

collect=$1

killall -q qemu-system-riscv32
echo "Killed QEMU"

# ------------------- Part two: collect performance results -------------- #
if [ "$collect" == "true" ]
then
    cd ..

    #echo "Done with experiments!"
    
    #echo "Now collecting perf data..."
    
    # Count up all misses of the last 50 lines
    cp build/isp-run-FreeRTOS-web-server.elf-compartmentalization-finite-${CACHE_SIZE}/sim.log ${COMP_DOMAINS}-${CACHE_SIZE}-${REQUEST_RATE}.log
    misses=$(grep "CACHE:" build/isp-run-FreeRTOS-web-server.elf-compartmentalization-finite-${CACHE_SIZE}/sim.log | tail -n 50 | cut -d " " -f 4 | paste -sd+ | bc)
    
    #echo "Total misses: $misses"
    
    # Turn into miss percent
    percent_miss=$(echo "$misses / 50 / 1000000 * 100.0" | bc -l)

    num_responses=$(grep "HTTP" webserver_driver/netcat_output.txt | wc -l)
    
    echo "$COMP_DOMAINS	$CACHE_SIZE	$REQUEST_RATE	$num_responses	$percent_miss" >> results.txt
else
    echo "no collection."
fi 
