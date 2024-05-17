from bcc import BPF

b = BPF(src_file="hello.c")
syscall = b.get_syscall_fnname("execve")
b.attach_kprobe(event=syscall, fn_name="hello")

b.trace_print()