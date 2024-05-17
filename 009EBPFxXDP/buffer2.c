
struct user_info {     
   u64 pid;
   u64 uid;
   char command[32];  // Increased size to 32 bytes
   u64 count;
};
BPF_HASH(comm_counter, u64, struct user_info);

int counter_map(void *ctx) {
    u64 X = 0;
    struct user_info user_i = {};
    struct user_info *p;

    user_i.pid = bpf_get_current_pid_tgid() >> 32;
    user_i.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;

    bpf_get_current_comm(&user_i.command, sizeof(user_i.command));

    p = comm_counter.lookup(&X);
    if (p == 0) {
        bpf_trace_printk("User Not Found\n");
    } else {
        // Entry found, increment the count
        user_i.count = p->count + 1;
        bpf_trace_printk("User Found, incrementing count!\n");
    }
    comm_counter.update(&X, &user_i);
    
    return 0;
}
