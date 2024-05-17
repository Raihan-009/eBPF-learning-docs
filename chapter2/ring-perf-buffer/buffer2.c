BPF_HASH(comm_counter);

int counter_map(void *ctx) {

    u64 uid;
    u64 pid;
    u64 counter = 0;
    // char command[16];
    // char msg[32]="Hello Hunny!";
    u64 *p;
 
   pid = bpf_get_current_pid_tgid() >> 32;
   uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;
   
//    bpf_get_current_comm(&command, sizeof(command));
//    bpf_probe_read_kernel(&msg, sizeof(msg), msg); 


   p = comm_counter.lookup(&uid);
   if (p == 0){
    bpf_trace_printk("User Not Founded!");
   }else{
    counter = *p;
    bpf_trace_printk("User  Founded!");
   }
   counter++;
    comm_counter.update(&uid, &counter);
   return 0;
}