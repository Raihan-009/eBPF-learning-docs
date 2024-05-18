# Chapter2 - eBPF’s “Hello World”

Created by: Raihan Islam  
Created time: May 16, 2024 11:11 PM

# Running a bpf program 👇🏻

In each example, the code will be divided into two parts: kernel space and user space. The eBPF program will run in the kernel space. However, to load it into the kernel space or NIC and read out the trace, we will need the user space code.

## `Running bpf program at a glance`

![Running BPF](https://github.com/Raihan-009/eBPF-learning-docs/blob/main/chapter2/00-bpf-diagrams/running-bpf.png?raw=true)

# Example01: Running Hello World!

## `Kernel Space Code [ BPF Program ]`

```c
int hello(void *ctx) {
    bpf_trace_printk("Hello World!");   // helper function to 
                                       // write down a message.
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

![Running hello.py](https://github.com/Raihan-009/eBPF-learning-docs/blob/main/chapter2/00-bpf-diagrams/hello.png?raw=true)

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

![Running hello-map.py](https://github.com/Raihan-009/eBPF-learning-docs/blob/main/chapter2/00-bpf-diagrams/hello-map.png?raw=true)

---

# Example03: Running Buffer!

## `Ring Buffer`

A ring buffer, also known as a circular buffer or cyclic buffer, is a data structure that uses a single, fixed-size buffer as if it were connected end-to-end. It is particularly useful for buffering data streams and is often used in scenarios where the producer and consumer of data operate at different speeds.

```mathematica
            +---+---+---+---+---+---+---+
            | 0 | 1 | 2 | 3 | 4 | 5 | 6 |   <-- Index
            +---+---+---+---+---+---+---+
               ^                   ^
               |                   |
            Head                Tail
```

💡 In this diagram:

- Each box represents an element in the ring buffer.
- The numbers at the top indicate the index of each element.
- The head pointer points to the next location where an element will be added.
- The tail pointer points to the next element to be removed.
- As elements are added (enqueued), the head pointer moves to the right.
- As elements are removed (dequeued), the tail pointer moves to the right.

If the head or tail pointer reaches the end of the buffer, it wraps around to the beginning, creating a circular behavior. This allows for efficient overwriting of old elements when the buffer is full.

![Ring Buffer](https://github.com/Raihan-009/eBPF-learning-docs/blob/main/chapter2/00-bpf-diagrams/ring-buffer.png?raw=true)

## `Kernel Space Code [ BPF Program ]`

```c
BPF_PERF_OUTPUT(output); // BPF_PERF_OUTPUT for creating a map \n
								// that will be used to pass messages 
                        // from the kernel to user space
 
struct data_t {     
   int pid;
   int uid;
   char command[16];
   char message[12];
};

int hello(void *ctx) {
   struct data_t data = {}; 
   char message[12] = "Hello World";
 
   data.pid = bpf_get_current_pid_tgid() >> 32;
   data.uid = bpf_get_current_uid_gid() & 0xFFFFFFFF;
   
   bpf_get_current_comm(&data.command, sizeof(data.command));
   bpf_probe_read_kernel(&data.message, sizeof(data.message), message); 
 
   output.perf_submit(ctx, &data, sizeof(data)); // output.perf_submit() puts 
																// that data into the map.
 
   return 0;
}
```

💡This eBPF program collects information about processes (such as PID, UID, command name) and sends it to user space using the perf event output mechanism. 

## `User Space Code [ *.py Program ]`

```python
from bcc import BPF

b = BPF(src_file="buffer.c") 
syscall = b.get_syscall_fnname("execve")
b.attach_kprobe(event=syscall, fn_name="hello")
 
def print_event(cpu, data, size):  
   data = b["output"].event(data)
   print(f"{data.pid} {data.uid} {data.command.decode()} {data.message.decode()}")
 
b["output"].open_perf_buffer(print_event) 
while True:   
   b.perf_buffer_poll()
```

💡This Python script prints that information to the console whenever the **`execve`** syscall is called. For details 👇🏻

## **Relationship Among `perf_submit`, `open_perf_buffer`, and `perf_buffer_poll`**

Among, the buffer acts as a bridge between the kernel-space eBPF program and the user-space Python script. When an event occurs, the eBPF program collects relevant data and uses `perf_submit` to send this data to the `output` perf event buffer. In user space, the Python script sets up this buffer with `open_perf_buffer`, registering a callback function to handle incoming data. The script then continuously polls the buffer using `perf_buffer_poll`. When new data arrives, `perf_buffer_poll` triggers the callback function, which processes and prints the data. This setup allows efficient real-time transfer and processing of data between kernel space and user space.

***For better visualization***

```mathematica
+------------------------+        +------------------------+        +--------------------------+
| Kernel Space (eBPF)    |        | User Space (Python)    |        | User Space (Python)      |
|                        |        |                        |        |                          |
|                        |        |                        |        |                          |
|  +------------------+  |        |  +------------------+  |        |  +--------------------+  |
|  |                  |  |        |  |                  |  |        |  |                    |  |
|  | Event Occurs     |  |        |  | Setup Buffer     |  |        |  | Poll Buffer        |  |
|  |                  |  |        |  |                  |  |        |  |                    |  |
|  +--------+---------+  |        |  +--------+---------+  |        |  +---------+----------+  |
|           |            |        |           |            |        |            |             |
|           |            |        |           |            |        |            |             |
|           |            |        |           |            |        |            |             |
|           v            |        |           v            |        |            v             |
|  +------------------+  |        |  +------------------+  |        |  +--------------------+  |
|  |                  |  |        |  |                  |  |        |  |                    |  |
|  | perf_submit      +----------->  | open_perf_buffer +----------->  | perf_buffer_poll   |  |
|  |                  |  |        |  | (print_event)    |  |        |  | (triggers          |  |
|  +------------------+  |        |  +------------------+  |        |  |  print_event)      |  |
|                        |        |                        |        |  +--------------------+  |
+------------------------+        +------------------------+        +--------------------------+

```

***Expected Output!!***

![Running hello-buffer.py](https://github.com/Raihan-009/eBPF-learning-docs/blob/main/chapter2/00-bpf-diagrams/hello-buffer.png?raw=true)

---