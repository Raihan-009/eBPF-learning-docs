from bcc import BPF
from time import sleep

b = BPF(src_file="buffer2.c")
syscall = b.get_syscall_fnname("execve")
b.attach_kprobe(event=syscall, fn_name="counter_map")
# b.trace_print()

while True:
    sleep(2)
    s = ""
    for k,v in b["comm_counter"].items():
        s += f"ID {k.value}: {v.value}\t"
    print(s)