# Example01: Running Hello World!

This code will be divided into two part. One is for kernel space and other one is for user space. The idea is the eBPF program will be run in kernel space but in order to load it kernel space or NIC and reads out the trace user space code will be needed.

## `Kernel Space Code [ BPF Program ]`
```c
int hello(void *ctx) {
    bpf_trace_printk("Hello World!"); // helper function to write down a message.
    return 0;
}
```

This eBPF program simply prints "Hello World!" to the kernel's tracing subsystem whenever it is executed. Before that there is a lot of stuff to do in `user space`. Such as

- `eBPF` programs need to be attached to an event.
- Need to trigger that event to trigger BPF program.
- Need to reads out those traces (if any).

## `User Space Code [ *.py Program ]`

1. Making a bpf obejct with bpf program

```python
b = BPF(text=program) # Passing `program`(c code) as a parameter
```
2. As we need to trigger that event to trigger BPF program.
```python
syscall = b.get_syscall_fnname("execve")  # execve() executes the program referred to by pathname.
```
3. As `eBPF` programs need to be attached to an event.
```python
b.attach_kprobe(event=syscall, fn_name="hello")
```
Kprobes enables you to dynamically break into any kernel routine and collect debugging and performance information non-disruptively. 

4. To read the tracing
```python
b.trace_print()
```