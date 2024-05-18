# Chapter 3 - Anatomy of an eBPF Program

Created by: Raihan Islam  
Created time: May 18, 2024 11:52 AM

# `eBPF anatomy at a glance!`

![e-BPF.png](https://github.com/Raihan-009/eBPF-learning-docs/blob/feature/chapter3Doc/chapter3/diagrams/e-BPF.png?raw=true)

# Anatomy of an eBPF Program

This project discusses how to execute an eBPF program, detailing each step from compiling to attaching to a hook.

## Features!

1. Compiling an eBPF Code
2. Inspecting an eBPF Object File
3. Loading Program into the Kernel
4. Inspecting the Program
5. Attaching an Event
6. Detaching and Unloading

## **Technologies Used!**

- Ubuntu LTS 22.04
- BCC framework
- bpftool

## Installation & Dependencies!

### *Install BCC*

To install BCC, run the following commands:

```bash
sudo apt-get install bpfcc-tools linux-headers-$(uname -r)
sudo apt install -y zip bison build-essential cmake flex git libedit-dev \\
  libllvm12 llvm-12-dev libclang-12-dev python zlib1g-dev libelf-dev libfl-dev python3-setuptools \\
  liblzma-dev arping netperf iperf
```

### ***Install bpftool***

To install bpftool, follow these steps:

```bash
cd ..
git clone --recurse-submodules https://github.com/libbpf/bpftool.git
cd bpftool/src
make install
```

## **Usage Instructions!**

### ***Compiling an eBPF Code***

To compile an eBPF code, you need to use the LLVM and Clang toolchain. Here is an example command:

```bash
clang -O2 -target bpf -c xdp_pass.c -o xdp_pass.o
```

### ***Inspecting an eBPF Object File***

After compiling, you can inspect the eBPF object file using **`bpftool`**:

```bash
llvm-objdump -S xdp_pass.o
```

***Expected Output:***

![inspecting.png](https://github.com/Raihan-009/eBPF-learning-docs/blob/feature/chapter3Doc/chapter3/diagrams/inspecting.png?raw=true)

### ***Loading Program into the Kernel***

Load the compiled eBPF program into the kernel using the following command:

```bash
sudo bpftool prog load xdp_pass.o /sys/fs/bpf/xdp_pass
```

In order to show the loaded program with id, run `ls /sys/fs/bpf` :

***Expected Output:***

```bash
71: xdp  name hello  tag d35b94b4c0c10efb  gpl
        loaded_at 2024-05-18T20:26:07+0600  uid 0
        xlated 96B  jited 64B  memlock 4096B  map_ids 15,16
```

### ***Inspecting the Program***

Inspect the loaded program to ensure it is correctly loaded:

```bash
sudo bpftool prog show id 71 --pretty
```

***Expected Output:***

![prog-show.png](https://github.com/Raihan-009/eBPF-learning-docs/blob/feature/chapter3Doc/chapter3/diagrams/prog-show.png?raw=true)

### ***Attaching an Event***

Attach the eBPF program to a specific event, such as a tracepoint or kprobe:

```bash
sudo bpftool net attach xdp id 71 dev veth0
```

***Expected Output:***

![attach.png](https://github.com/Raihan-009/eBPF-learning-docs/blob/feature/chapter3Doc/chapter3/diagrams/attach.png?raw=true)

### ***Detaching and Unloading***

To detach and unload the eBPF program, use:

```bash
sudo bpftool net detach xdp dev eth0
rm /sys/fs/bpf/hello
```

---