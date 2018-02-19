
        .global end_simulation

end_simulation:
        csrr t0, mimpid
        bltz t0, end_sim_now
        ret
end_sim_now:
        li t0, 0x08
        slli t0, t0, 28
        csrw 0x7ff, t0
        j end_sim_now


        .global get_sim_time
get_sim_time:
        csrr a0, time
        li   t0, 1000
        div a0, a0, t0
        ret
        
        .global get_usec_time
get_usec_time:
        csrr a0, time
        ret
        .global get_inst_ret
get_inst_ret:
        csrr a0, instret
        ret
        
