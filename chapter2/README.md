# Chapter2 - eBPF’s “Hello World”

Created by: Raihan Islam  
Created time: May 16, 2024 11:11 PM

# Running a bpf program 👇🏻

In each example, the code will be divided into two parts: kernel space and user space. The eBPF program will run in the kernel space. However, to load it into the kernel space or NIC and read out the trace, we will need the user space code.

## `Running bpf program at a glance`

![Running BPF](https://github.com/Raihan-009/eBPF-learning-docs/blob/main/chapter2/running-bpf.png?raw=true)

# Example01: Running Hello World!

## `Kernel Space Code [ BPF Program ]`

```c
int hello(void *ctx) {
    bpf_trace_printk("Hello World!"); // helper function to write down a message.
    return 0;
}
```

💡This eBPF program simply prints "Hello World!" to the kernel's tracing subsystem whenever it is executed. Before that there is a lot of stuff to do in `user space`. Such as:

- `eBPF` programs need to be attached to an event.
- Need to trigger that event to trigger BPF program.
- Need to reads out those traces (if any).

## `User Space Code [ *.py Program ]`

1. Making a bpf obejct with bpf program

```python
b = BPF(text=program) # Passing `program`(c code) as a parameter
```

1. As we need to trigger that event to trigger BPF program.

```python
syscall = b.get_syscall_fnname("execve") # execve() executes the program referred to by pathname.
```

1. As `eBPF` programs need to be attached to an event.

```python
b.attach_kprobe(event=syscall, fn_name="hello")
```

`Kprobes` enables you to dynamically break into any kernel routine and collect debugging and performance information non-disruptively.

1. To read the tracing

```python
b.trace_print()
```

***Expected Output!!***

![Running hello.py](https://github.com/Raihan-009/eBPF-learning-docs/blob/main/chapter2/hello.png?raw=true)

---

# Example02: Running Hash Map!

## `Kernel Space Code [ BPF Program ]`

```c
BPF_HASH(counter_table);

int hellomap(void *ctx) {
   u64 uid;
   u64 counter = 0;
   u64 *p;

   uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;
   p = counter_table.lookup(&uid);
   if (p != 0) {
      counter = *p;
   }
   counter++;
   counter_table.update(&uid, &counter);
   return 0;
}
```

💡 This eBPF program maintains a count of some event associated with each user ID. It uses a hash table to store the counts, incrementing them each time the **`hellomap`** function is called.

## `User Space Code [ *.py Program ]`

```python
b = BPF(text=program)
syscall = b.get_syscall_fnname("execve")
b.attach_kprobe(event=syscall, fn_name="hellomap")
```

As usual, we needed to convert BPF code into an object and attach it to an event to trigger it conditionally.

```python
while True:
    sleep(2)
    s = ""
    for k,v in b["counter_table"].items():
        s += f"ID {k.value}: {v.value}\t"
    print(s)
```

Here, this Python script continuously queries the **`counter_table`** hash table created by that eBPF program every 2 seconds and prints the IDs and their associated values stored in the table. It's a way to monitor the state of the eBPF program in real-time

***Expected Output!!***

![Running hello-map.py](https://github.com/Raihan-009/eBPF-learning-docs/blob/main/chapter2/hello-map.png?raw=true)

---