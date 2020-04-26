#!/bin/bash
# This script runs the webserver for collecting tracing data: working set files and coresidency files

# Interpret input, set environment variables:
domains=$1
cache_size=$2
request_rate=$3

# Ten loops through the input files for higher privilege coverage
num_loops=10

export COMP_DOMAINS=$domains
export CACHE_SIZE=$cache_size
export REQUEST_RATE=$request_rate
export NUM_LOOPS=$num_loops

echo "Running with domains=$domains, cache_size=$cache_size, request_rate=$request_rate, num_loops=$num_loops"

# Kill old sessions, only 1 at a time
echo "Killing any potential stale old driving processes..."
killall -q drive_session.sh
killall -q qemu-system-riscv32

# Cleanup files
rm -f result.cmap result_weighted.cmap

# Configure this as a non-tracing, non-prefetching run
touch TRACING_ENABLE
rm -f PREFETCH_ENABLE

# Run webserver driver as configured
echo "Launching the session driver now"
cd webserver_driver
./drive_session.sh ./session_loop.sh false &

cd ..
echo "Foreground, running a make clean and then booting server..."

# First there's a dummy make that dies after 30s. Required if policy is rebuilt to get new identifiers...
timeout 30s make sim-qemu
#  Then launch the real server
make sim-qemu-cache

rm -f TRACING_ENABLE

cp coresidency_latest.txt FreeRTOS-web-server.elf.coresidency
cp working_sets_latest.txt FreeRTOS-web-server.elf.working_sets
