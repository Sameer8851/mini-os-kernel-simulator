#include "filesystem.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>


void FileSystem::setLogLevel(LogLevel level) {
    current_log_level = level;
}

void FileSystem::log(LogLevel level, const std::string& message) {
    if (current_log_level >= level) {
        std::cout << message << std::endl;
    }
}


// --- Constructor ---
FileSystem::FileSystem(int num_blocks) : current_log_level(NORMAL){
    disk.resize(num_blocks);
    free_block_bitmap.resize(num_blocks,false);
    next_inode_id = 0;

    format();
}

// --- Format Method ---
void FileSystem::format(){
    log(NORMAL, "Formatting the file system...");

    inode_table.clear();
    root_directory.clear();
    std::fill(free_block_bitmap.begin(), free_block_bitmap.end(), false);
    
    Inode root_inode;
    root_inode.id = next_inode_id++;
    root_inode.size = 0;
    inode_table[root_inode.id] = root_inode;
    
    free_block_bitmap[0] = true; 
    
    log(NORMAL, "File system formatted. Root directory created with Inode 0.");

}


int FileSystem::findFreeInode() {
    return next_inode_id++;
}

int FileSystem::create(const std::string& filename){
    // 1. Check if the file already exists in the root directory
    if(root_directory.count(filename)){
        log(NORMAL, "Error: File '" + filename + "' already exists.");
        return -1;
    }

    // 2. Get a new inode for the file
    int inode_id = findFreeInode();
    if(inode_id == -1){
        log(NORMAL, "Error: No free inodes available.");
        return -1;
    }
    Inode new_inode;
    new_inode.id = inode_id;
    new_inode.size = 0;

    // 3. Add the new inode to the inode table
    inode_table[inode_id] = new_inode;

    // 4. Add the new file entry to the root directory
    root_directory[filename] = inode_id;

    log(VERBOSE, "Created file '" + filename + "' with Inode " + std::to_string(inode_id));
    return inode_id;
}

int FileSystem::findFreeBlock(){
    for(size_t i = 0;i < free_block_bitmap.size();i++){
        if(!free_block_bitmap[i]){
            return i;
        }
    }
    return -1;
}


int FileSystem::write(int inode_number,const std::string& data){
    // 1. Find the inode
    if(inode_table.find(inode_number) == inode_table.end()){
        log(NORMAL, "Error: Inode " + std::to_string(inode_number) + " not found.");
        return -1;
    }
    Inode& inode = inode_table.at(inode_number);

    // 2. Clear old data blocks associated with the inode
    for(int block_index : inode.data_block_indices){
        free_block_bitmap[block_index] = false;
    }
    inode.data_block_indices.clear();

    // 3. Calculate how many blocks are needed for the new data
    int data_len = data.length();
    int blocks_needed = (data_len + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // 4. Allocate new blocks and copy data
    int bytes_written = 0;
    for(int i = 0;i < blocks_needed; i++){
        int block_index = findFreeBlock();
        if(block_index == -1){
            log(NORMAL, "Error: Out of disk space.");
            for(int b_idx : inode.data_block_indices) free_block_bitmap[b_idx] = false;
            inode.data_block_indices.clear();
            return -1;
        }

        free_block_bitmap[block_index] = true;
        inode.data_block_indices.push_back(block_index);

        int bytes_to_copy = std::min((int)data.length()- bytes_written, BLOCK_SIZE);
        strncpy(disk[block_index].data, data.c_str() + bytes_written, bytes_to_copy);
        bytes_written += bytes_to_copy;
    }

    // 5. Update inode metadata
    inode.size = data_len;

    log(VERBOSE, "Wrote " + std::to_string(data_len) + " bytes to Inode " + std::to_string(inode_number));
    return data_len;
}

std::string FileSystem::read(int inode_number) {
    // 1. Find the inode
    if (inode_table.find(inode_number) == inode_table.end()) {
        log(NORMAL, "Error: Inode " + std::to_string(inode_number) + " not found.");
        return ""; // Return empty string on error
    }
    const Inode& inode = inode_table.at(inode_number);

    // 2. Reconstruct the data from the data blocks
    std::string data = "";
    int bytes_to_read = inode.size;

    for (int block_index : inode.data_block_indices) {
        int bytes_from_this_block = std::min(bytes_to_read, BLOCK_SIZE);
        data.append(disk[block_index].data, bytes_from_this_block);
        bytes_to_read -= bytes_from_this_block;
    }

    log(VERBOSE, "Read " + std::to_string(inode.size) + " bytes from Inode " + std::to_string(inode_number));
    return data;
}

void FileSystem::remove(const std::string& filename){
    // 1. Find the file in the root directory
    if(root_directory.find(filename) == root_directory.end()){
        log(NORMAL, "Error: Cannot remove file '" + filename + "', not found.");
        return;
    }
    int inode_number = root_directory.at(filename);

    // 2. Get the inode
    if(inode_table.find(inode_number) == inode_table.end()){
        log(NORMAL, "Error: Inode " + std::to_string(inode_number) + " is corrupted or missing.");
        return;
    }
    const Inode& inode = inode_table.at(inode_number);

    // 3. Free up all the data blocks used by the file
    for(int block_index : inode.data_block_indices){
        if(block_index >= 0 && block_index < free_block_bitmap.size()){
            free_block_bitmap[block_index] = false;
        }
    }

    // 4. Remove the inode from the inode table
    inode_table.erase(inode_number);

    // 5. Remove the file's entry from the root directory
    root_directory.erase(filename);

    log(VERBOSE, "Removed file '" + filename + "' and freed its resources.");
}

