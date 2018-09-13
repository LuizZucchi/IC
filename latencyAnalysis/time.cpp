#include <iostream>
#include <stdio.h>
#include <stdlib.h>
//mede o começo do tempo
uint64_t rdtsc_in() {
    uint64_t a, d;
    asm volatile ("xor %%rax, %%rax\n" "cpuid"::: "rax", "rbx", "rcx", "rdx");
    asm volatile ("rdtscp" : "=a" (a), "=d" (d) : : "rcx");
    a = (d << 32) | a;
    return a;
}

// mede o final do tempo
uint64_t rdtsc_out() {
    uint64_t a, d;
    asm volatile ("rdtscp" : "=a" (a), "=d" (d) : : "rcx");
    asm volatile ("cpuid"::: "rax", "rbx", "rcx", "rdx");
    a = (d << 32) | a;
    return a;
}

//realiza a medida de tempo entre dois endereços.
uint64_t get_timing(uint64_t first, uint64_t second) {
    size_t number_of_reads = NUM_READS;
    volatile uint64_t *f = (volatile uint64_t *) first;
    volatile uint64_t *s = (volatile uint64_t *) second;

    uint64_t t0 = rdtsc_in();

    while (number_of_reads-- > 0) {
        *f;
        *s;
        asm volatile("clflush (%0)" : : "r" (f) : "memory");
        asm volatile("clflush (%0)" : : "r" (s) : "memory");
        asm volatile("mfence": : :);
    }
    return (rdtsc_out() - t0) / (NUM_READS);
}
