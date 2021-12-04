#define LOGICAL_ADDRESS_BITS 128 // 32 bits since the addresses are 32 bits by definition
#define PHASE_ONE_FRAMES 256 
#define FRAME_SIZE 256
#define PHASE_TWO_FRAMES 128 
#define TLB_SIZE 16
struct PageTable {
    int p; // page number m-n
    int d; // page offset n
    int virtualAddress;
    int physicalAddress; // [p * PHASE_(ONE/TWO)] * d
    int valid; // valid page = 1, invalid page = 0
    int presence; // 1 = in memory, 0 = in BackingStore
    int reference; // 1= accessed, 0 = not accessed
    int lastUse;
};

struct AddressTable {
    int page; // page number m-n
    int virtualAddress;
    int offset; // page offset n
};

struct TLB {
    int pageNumber;
    int frameNumber;
};

struct Memory{
    signed char data[FRAME_SIZE];
};



void innit_Memory(int frames);
void innit_PageTable(int frames);
void innit_AddressTable(int frames);
void innit_TLB();
void dectobin(int n);
void getp(int n);
void getd(int n);
int PageTableAdd(int pageNumber, int offset, int count, signed char diskData[]);
int AddressTableAdd(int tempAddress);
int PageTableGetFrame(int tempAddress);
int TLBGetFrame(int pageNum);
int TLBAdd(int pageNum, int frameNum);