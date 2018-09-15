#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <map>
#include <vector>
#include <list>

extern int verbosity;
// -----------------------------------

typedef uint64_t pointer;

typedef std::pair<pointer, pointer> addrpair;

size_t getPhysicalMemorySize();
void createMap();
size_t frameNumberFromPagemap(size_t value);
pointer getPhysicalAddress(pointer virtual_addr);
void getRandom(pointer *virtual_adress, pointer *physical_address);
uint64_t rdtsc_in();
uint64_t rdtsc_out();
uint64_t get_timing(uint64_t first, uint64_t second);
void initPagemap();
