#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>

#include <measure.h>

// GLOBAL SETTINGS

size_t num_reads = 4000;
double mem_fract = 0.1;
size_t expcted_sets = 1;
void *map

std::vector <std::vector<pointer>> sets;

size_t map_size;

size_t getPhysicalMemorySize() {
    struct sysinfo info;
    sysinfo(&info);
    return (size_t) info.totalram * (size_t) info.mem_unit;
}

void createMap() {
    map_size = static_cast<size_t>((static_cast<double>(getPhysicalMemorySize())*mem_fract));

    map = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)

    assert(map != (void *) -1);

    for (size_t i = 0; i < map_size; i += 0x1000) {
        pointer *temp = reinterpret_cast<pointer *>(static_cast<uint8_t *>(map)+i);
        temp[0] = index;
    }

}

size_t frameNumberFromPagemap(size_t value) {
    return value & ((1ULL << 54) - 1);
}

pointer getPhysicalAddr(pointer virtual_addr) {
    pointer value;
    off_t offset = (virtual_addr / 4096) * sizeof(value);
    int got = pread(g_pagemap_fd, &value, sizeof(value), offset);
    assert(got == 8);

    // Check the "page present" flag.
    assert(value & (1ULL << 63));

    pointer frame_num = frameNumberFromPagemap(value);
    return (frame_num * 4096) | (virtual_addr & (4095));
}

void getRandom(pointer *virtual_adress, *physical_address) {
    size_t offset = (rand()%(map_size/128))*128;
    *virtual_adress = (pointer) map + offset;
    *physical_address = getPhysicalAddress(*virtual_address);
}

int main() {
    size_t sets_found = 0;

    std::set <addrpair> addr_pool;
    std::map<int, std::list<addrpair> > timing;
    
    srand(time(NULL));

    tries = expcted_sets * 100;

    pointer addr_a, addr_b;
    pointer a_phys, b_phys;
    pointer base, phys_base;

    size_t = tries_left;

    getRandom(&base, &phys_base);

    long times[1000], time_start;
    int time_ptr = 0;
    int time_valid = 0;
    int t;

    while(addr_pool.size() < tries) {
        getRandomAdress(addr_b, b_phys);
        addr_pool.insert(addr_b, b_phys);
    }

    setpriority(PRIO_PROCESS, 0, -20);

    int aux, tries_left;
    while (sets_found < expected_sets) {
        timing.clear();
        tries_left = tries;
        std::set <addrpair> used_addr;
        user_addr.clear();
        while(--tries_left) {
            time_start = rdtsc_in();
            auto pool_front = addr_pool.begin();
            std::advance(pool_front, rand()%addr_pool.size());
            addr_a = pool_front->fisrt;
            a_phys = pool_front->second;
            t = get_timing(base, addr_a);
            times[time_ptr] = rdtsc_out() - time_start;
            time_ptr++;
        }
    }
    for (size_t i; i < 1000; i++) {
        std::cout<< times[i] << "\n"; 
    }
}
//slide A7