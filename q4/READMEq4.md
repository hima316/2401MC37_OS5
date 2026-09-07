# Dining Philosophers Problem on xv6-riscv

## 1. Deadlock Avoidance Strategy: Asymmetric Resource Allocation

### The Circular Wait Condition
In the symmetric dining philosophers algorithm, every philosopher $i$ attempts to acquire their left fork first ($i$), followed by their right fork ($(i+1) \pmod 5$). Under simultaneous scheduling, all five philosophers can pick up their left fork simultaneously. At this point:
- Every fork is held by one philosopher.
- Every philosopher is blocked waiting for their right fork.
- The system enters a circular wait state ($P_0 \to P_1 \to P_2 \to P_3 \to P_4 \to P_0$), producing a permanent deadlock.

### How Asymmetry Eliminates Deadlock
We break Coffman's fourth necessary condition for deadlock—**circular wait**—by introducing asymmetry in the order in which forks are requested:
- **Even Philosophers ($P_0, P_2, P_4$)**: Acquire `left_fork` first, then `right_fork`.
- **Odd Philosophers ($P_1, P_3$)**: Acquire `right_fork` first, then `left_fork`.

#### Proof of Deadlock-Freedom:
Consider $P_0$ (even) and $P_1$ (odd):
- Fork 1 is $P_0$'s right fork and $P_1$'s right fork.
- $P_1$ attempts to pick up Fork 1 **first**.
- $P_0$ picks up Fork 0 first, and only then tries to pick up Fork 1.
- Because odd and even neighbors compete for the same fork first instead of forming a cyclic dependency directed along the circle, it is impossible for all philosophers to hold one fork while waiting on their second.
- Specifically, between $P_0$ and $P_1$, only one can hold Fork 1. If $P_1$ secures Fork 1, $P_0$ blocks on Fork 1, releasing nothing it does not have and keeping Fork 0 open for an adjacent philosopher or allowing $P_1$ to proceed once Fork 2 is resolved.
- Consequently, at least one philosopher is always guaranteed access to both adjacent forks, guaranteeing global forward progress.

## 2. Compilation and Execution Instructions

Compile and start the xv6 kernel inside QEMU:
```bash
make clean
make qemu