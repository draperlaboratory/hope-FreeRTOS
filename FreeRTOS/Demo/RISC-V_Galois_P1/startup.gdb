target remote localhost:3333
load

define fpga_reset
	set *(0x6fff0000)=1
end
