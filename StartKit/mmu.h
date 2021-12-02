#define FRAMES 256 
#define FRAME_SIZE 256

struct pageEntry {
    char frameNumber;
    int p; // page number m-n
    int d; // page offset n
    char valid; // valid page = 1, invalid page = 0
    char presence; // 1 = in memory, 0 = in BackingStore
    char reference; // 1= accessed, 0 = not accessed
};
struct logicalAddress {
    int p; // page number m-n
    int d; // page offset n
};

struct pageTable{
    //struct pageEntry[];
    int t;
};

struct process {
    int pid;
    int pageNumber;
    int offset;
    struct pageTable pTable;

};