#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include "fs_types.hpp"
#include "core/types.hpp"
#include <vector>
#include <map>
#include <string>

class FileSystem {
    public:
        FileSystem(int num_blocks);
        void format();

        void setLogLevel(LogLevel level);

        // Core file operations
        int create(const std::string& filename);
        int write(int inode_number,const std::string& data);
        std::string read(int inode_number);
        void remove(const std::string& filename);

        const std::map<std::string, int>& getRootDirectory() const { return root_directory; }
        const std::map<int, Inode>& getInodeTable() const { return inode_table; }

    private:
        std::vector<DataBlock> disk;
        std::vector<bool> free_block_bitmap;

        // File system Metadata
        std::map<int, Inode> inode_table;
        int next_inode_id;

        // The root directoy
        std::map<std::string,int> root_directory;

        int findFreeBlock();
        int findFreeInode();

        LogLevel current_log_level;
        void log(LogLevel level, const std::string& message);

};

#endif