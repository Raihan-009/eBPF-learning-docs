
struct user_info {     
   u64 pid;
   u64 uid;
   u64 count;
};
BPF_HASH(comm_counter,u64,struct user_info);

int counter_map(void *ctx) {
    
    u64 X = 0;

    struct user_info user_i = {};
    u64 counter = 0;
    // char command[16];
    // char msg[32]="Hello Hunny!";
    struct user_info *p;
 
   user_i.pid = bpf_get_current_pid_tgid() >> 32;
   user_i.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;
   
//    bpf_get_current_comm(&command, sizeof(command));
//    bpf_probe_read_kernel(&msg, sizeof(msg), msg); 


   p = comm_counter.lookup(&X);
   if (p == 0){
    bpf_trace_printk("User Not Founded!");
   }else{
    counter = p->count;
    bpf_trace_printk("User  Founded!");
   }
   counter++;
   user_i.count = counter;
    comm_counter.update(&X, &user_i);
   return 0;
}