from bcc import BPF
from time import sleep

b = BPF(src_file="buffer2.c")
syscall = b.get_syscall_fnname("execve")
b.attach_kprobe(event=syscall, fn_name="counter_map")
# b.trace_print()

while True:
    sleep(2)
    s = ""
    for k, v in b["comm_counter"].items():
        if v.uid == 0:
            s += f"X : {k.value}, PID : {v.pid}, UID : {v.uid}, COUNT : {v.count}, COMMAND : {v.command.decode()}\n"
    if s:  # Only print if s is not empty
        print(s)
