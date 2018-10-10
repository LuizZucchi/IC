#include <bitset>
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <linux/kernel-page-flags.h>
#include <stdint.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <map>
#include <list>
#include <utility>
#include <fstream>
#include <set>
#include <algorithm>
#include <sys/time.h>
#include <sys/resource.h>
#include <sstream>
#include <iterator>
#include <math.h>

#include "measure.h"

#define HIST_SIZE 1500
// GLOBAL SETTINGS

size_t num_reads = 4000;
double mem_fract = 0.1;
size_t expected_sets = 1;
void *map;

std::vector <std::vector<pointer>> sets;
std::map<int, std::vector<pointer> > functions;

int g_pagemap_fd = -1;
size_t map_size;

size_t getPhysicalMemorySize() {
    struct sysinfo info;
    sysinfo(&info);
    return (size_t) info.totalram * (size_t) info.mem_unit;
}

void createMap() {
    map_size = static_cast<size_t>((static_cast<double>(getPhysicalMemorySize())*mem_fract));

    map = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    assert(map != (void *) -1);

    for (size_t i = 0; i < map_size; i += 0x1000) {
        pointer *temp = reinterpret_cast<pointer *>(static_cast<uint8_t *>(map)+i);
        temp[0] = i;
    }

}

size_t frameNumberFromPagemap(size_t value) {
    return value & ((1ULL << 54) - 1);
}

void initPagemap() {
    g_pagemap_fd = open("/proc/self/pagemap", O_RDONLY);
    assert(g_pagemap_fd >= 0);
}

pointer getPhysicalAddress(pointer virtual_addr) {
    pointer value;
    off_t offset = (virtual_addr / 4096) * sizeof(value);
    int got = pread(g_pagemap_fd, &value, sizeof(value), offset);
    assert(got == 8);
    // Check the "page present" flag.
    assert(value & (1ULL << 63));

    pointer frame_num = frameNumberFromPagemap(value);
    return (frame_num * 4096) | (virtual_addr & (4095));
}

void getRandom(pointer *virtual_address, pointer *physical_address) {
    size_t offset = (rand()%(map_size/128))*128;
    *virtual_address = (pointer) map + offset;
    *physical_address = getPhysicalAddress(*virtual_address);
}

int findThreshold(size_t *hist, int min, int max) { //refactor for just one return
    int flag = 0;
    for (int i = max; i >= min; i--) {
        if (flag == 20) {
            return i;//bad, bad practice =(
        }
        if (hist[i] == 0 || hist[i] <= 1) {
            flag++;
        } else {
            flag = 0;
        }
    }
    return 0;
}


int main() {
    size_t sets_found = 0;
    size_t hist[HIST_SIZE];

    std::set <addrpair> addr_pool;
    std::map<int, std::list<addrpair> > timing;
    std::map<int, std::list<addrpair> >::iterator it = timing.begin();
    srand(time(NULL));
    initPagemap();
    createMap();

    size_t tries = expected_sets * 100;
    
    pointer addr_a, addr_b;
    pointer a_phys, b_phys;
    pointer base, phys_base;

    getRandom(&base, &phys_base);
    
    long times[1000] = {0}, time_start;
    int time_ptr = 0;
    int time_valid = 0;
    int t;

    while(addr_pool.size() < tries) {
        getRandom(&addr_b, &b_phys);
        addr_pool.insert(std::make_pair(addr_b, b_phys));
    }
    
    setpriority(PRIO_PROCESS, 0, -20);

    int aux, tries_left;
    while (sets_found < expected_sets) {
        timing.clear();
        tries_left = tries;
        std::set <addrpair> used_addr;
        used_addr.clear();
        while(--tries_left) {
            auto pool_front = addr_pool.begin();
            std::advance(pool_front, rand()%addr_pool.size());
            addr_a = pool_front->first;
            a_phys = pool_front->second;
            t = get_timing(base, addr_a);
            times[time_ptr] = t;
            timing[t].push_back(std::make_pair(base, a_phys));
            //printf(">>%d\n", times[time_ptr]);
            time_ptr++;
        }
        sets_found++;
    }
    /*for (size_t i; i < 1000; i++) {
        std::cout<< times[i] << "\n"; 
    }*/

    std::vector <pointer> new_set;// set of addr that maybe are in the same bank
    std::map < int, std::list < addrpair > > ::iterator hit;
    
    int min = HIST_SIZE;
    int max = 0;
    int max_v = 0;

    for (hit = timing.begin(); hit != timing.end(); hit++) {
        hist[hit->first] = hit->second.size();
        if (hit->first > max) {
            max = hit->first;
        }
        if (hit->first < min) {
            min = hit->first;
        }
        if (hit->second.size() > max_v) {
            max_v = hit->second.size();
        }
    }

    double scale_v = (double)(100.0)/(max_v > 0 ? (double) max_v : 100.0);

    assert(scale_v >= 0);
    while(hist[++min] <= 1);
    while(hist[--max] <= 1);

    for (size_t i = min; i <= max; i++) {
        printf("%u: %u", i, hist[i]);
        assert(hist[i] >= 0);
        for (size_t j = 0; j < hist[i]*scale_v && j < 80; j++) {
            printf("+");
        }
        printf("\n");
    }
    printf("there is a threshold in: %d\n", findThreshold(hist, min, max));
}
// use the thresold to form a set