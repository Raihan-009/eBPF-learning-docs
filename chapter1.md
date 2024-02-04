# Chapter1 - *What Is eBPF, and Why Is It Important?*


Created by: Raihan Islam  
Created time: February 5, 2024 2:01 AM



## *Description*

The term eBPF stands for ***extended Berkeley Packet Filter,*** is a technology that can run programs in a privileged context or allow developers to write custom code within the operating system kernel. This enables a new generation of highly performant networking, observability, and security tools.

Some key points about eBPF features

- Performance tracing
- Networking with visibility
- Detecting and Preventing malicious activity

![ebpf.png](Chapter1%20-%20Raihan%20ca8ad884a1b74b76a5baed0278677442/ebpf.png)

## *Linux Kernel Space and User Space*

***The Linux kernel*** serves as an intermediary between applications and hardware, facilitating functions like file access, networking, and process management, allowing multiple applications to run concurrently.

![kernel.png](Chapter1%20-%20Raihan%20ca8ad884a1b74b76a5baed0278677442/kernel.png)

***User space*** refers to the area of a computer's memory where user applications and processes run. Basically applications run in user space, unable to directly access hardware. They makes requests using the system call (syscall) interface to request the kernel to act on its behalf.

![user-space.png](Chapter1%20-%20Raihan%20ca8ad884a1b74b76a5baed0278677442/user-space.png)

## *Adding New Functionality to the Kernel*

Adding new functionality to the kernel we may approach in two different way.

1. ***Upstream Development:*** This method involves submitting code changes directly to the official Linux kernel maintained by the kernel development community.  The process can be time-consuming, as patches need to go through rigorous review and acceptance criteria before being merged into the mainline kernel.
2. ***Kernel Modules:*** Alternatively, developers can create kernel modules, which are pieces of code that can be *dynamically loaded* into the running kernel to add or extend functionality without the need to modify the kernel source code.

## *Dynamic Loading of eBPF Programs*

***eBPF*** programs can be dynamically loaded into the kernel and triggered by specific events, such as ***syscalls***, without requiring a reboot. This flexibility allows for on-the-fly kernel functionality updates and event-based execution, eliminating the need for system reboots after kernel upgrades.

## ***eBPF in Cloud Native Environments***

In modern cloud-native approaches like containers and Kubernetes, applications are distributed across servers automatically, all of which run a kernel. When using eBPF, these kernels can be instrumented to provide visibility into all containerized workloads on a node, enabling powerful monitoring and performance analysis without the need for individual workload instrumentation.