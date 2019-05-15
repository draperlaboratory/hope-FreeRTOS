define cpu_reset
	print "Reseting the CPU"
	set *(0x6fff0000)=1
end

target remote besspin-fpga-4.proj.galois.com:3333
cpu_reset
disconnect

target remote besspin-fpga-4.proj.galois.com:3333
print "Loading binary"
load
