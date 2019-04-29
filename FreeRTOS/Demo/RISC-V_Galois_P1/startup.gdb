define cpu_reset
	set *(0x6fff0000)=1
end

target remote localhost:3333
cpu_reset
disconnect

target remote localhost:3333
load
