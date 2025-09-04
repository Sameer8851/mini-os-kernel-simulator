#ifndef FS_TYPES_HPP
#define FS_TYPES_HPP

#include <string>
#include <vector>
#include <map>

const int BLOCK_SIZE = 512;

// block of data on our simulated disk
struct DataBlock
{
    char data[BLOCK_SIZE];
};

// metadata for singe file
struct Inode
{
    int id;
    int size;
    std::vector<int> data_block_indices; // pointers to the data blocks
};

struct DirectoryEntry
{
    std::string filename;
    int inode_number;
};

#endif