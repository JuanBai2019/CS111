#!/usr/local/cs/bin/python3

#NAME: Juan Bai
#Daisybai66@yahoo.com
#ID: 105364754

import sys
import csv

superBLK = None
GROUP_List = list()
BFREE_List = list()
IFREE_List = list()
INODE_List = list()
DIRENT_List = list()
INDIRECT_List = list()
duplicates = set() #store each duplicated blocks
legal_block_storage = dict() #maps each block to a class that contains its 
                            #location (direct, indirect, double indirect, triple indirect), 
                            #inode number, and offset
has_indirect_level_1 = False
has_indirect_level_2 = False
has_indirect_level_3 = False

isreported = {}

class superblock:
    def __init__(self, row):
        self.numBlocks = int(row[1])
        self.numInodes = int(row[2])
        self.blockSize = int(row[3])
        self.inodeSize = int(row[4])
        self.blocksPerGroup = int(row[5])
        self.inodesPerGroup = int(row[6])
        self.firstNonRevInode = int(row[7])

class group:
    def __init__(self, row):
        self.groupNum = int(row[1])
        self.numBlocks = int(row[2])
        self.numInodes = int(row[3])
        self.numFreeBlocks = int(row[4])
        self.numFreeInodes = int(row[5])
        self.blockNum_freeBlockBitmap = int(row[6])
        self.blockNum_freeInodeBitmap = int(row[7])
        self.firstBlockOfInode = int(row[8])

# class bfree:
#     def __init__(self, row):
#         self.numFreeBlocks = int(row[1])

# class ifree:
#     def __init__(self, row):
#         self.numFreeInodes = int(row[1])

class inode:
    def __init__(self, row):
        self.inodeNumber = int(row[1])
        self.fileType = row[2]
        self.mode = int(row[3])
        self.owner = int(row[4])
        self.group = int(row[5])
        self.linkCount = int(row[6])
        self.timeCreated = row[7]
        self.timeModification = row[8]
        self.timeLastAccess = row[9]
        self.fileSize = int(row[10])
        self.i_blocks = int(row[11])
        self.blockAddress = []
        for i in range(12,24):
            if i<len(row) :
                self.blockAddress.append(int(row[i]))
        global has_indirect_level_1
        if 24<len(row) :
            has_indirect_level_1 = True
            self.indirect_level_one = int(row[24])
        else:
            has_indirect_level_1 = False
        global has_indirect_level_2
        if 25<len(row) :
            has_indirect_level_2 = True
            self.indirect_level_two = int(row[25])
        else:
            has_indirect_level_2 = False
        global has_indirect_level_3
        if 26<len(row) :
            has_indirect_level_3 = True
            self.indirect_level_three = int(row[26])
        else:
            has_indirect_level_3 = False

class directory:
    def __init__(self, row):
        self.parentInodeNumber = int(row[1])
        self.logicalByteOffset = int(row[2])
        self.ref_file_inodeNumber = int(row[3])
        self.entryLength = int(row[4])
        self.nameLength = int(row[5])
        self.name = row[6]

class indirect_block_reference:
    def __init__(self, row):
        self.owningFileInodeNumber = int(row[1])
        self.levelIndirection = int(row[2])
        self.logicalBlockOffset = int(row[3])
        self.indirectBlockNumber = int(row[4])
        self.referencedBlockNumber = int(row[5])
        
class my_info:
    def __init__(self, loc, i, off):
        self.location = loc
        self.inode = i
        self.offset = off

def inode_block_ptr_checker():
    for i in INODE_List:
        #direct block
        offset = 0 
        for directBLK in i.blockAddress:
            if directBLK == 0:
                continue
            elif directBLK < 0 or directBLK > superBLK.numBlocks: #INVALID
                print("INVALID BLOCK "+str(directBLK)+" IN INODE "+str(i.inodeNumber)+" AT OFFSET "+str(offset))
                exit_code = 2
            elif directBLK in range(1,8): #RESERVED (0 - 7)
                print("RESERVED BLOCK "+str(directBLK)+" IN INODE "+str(i.inodeNumber)+" AT OFFSET "+str(offset))
                exit_code = 2
            else: #legal blocks
                if directBLK in BFREE_List: #referenced but is also in free list
                    print("ALLOCATED BLOCK "+str(directBLK)+" ON FREELIST")
                    exit_code = 2
                elif directBLK not in legal_block_storage: #check for dupllicates
                    block_info = my_info("", i.inodeNumber, offset)
                    legal_block_storage[directBLK] = [block_info]
                elif directBLK in legal_block_storage:
                    block_info = my_info("", i.inodeNumber, offset)
                    legal_block_storage[directBLK].append(block_info)
                    duplicates.add(directBLK)
            offset += 1

        #first indirect block
        if has_indirect_level_1 == True and hasattr(i, 'indirect_level_one'):
            indirect = i.indirect_level_one
            offset = 12
            if indirect != 0:
                if indirect < 0 or indirect > superBLK.numBlocks: #INVALID
                    print("INVALID INDIRECT BLOCK "+str(indirect)+" IN INODE "+str(i.inodeNumber)+" AT OFFSET "+str(offset))
                    exit_code = 2
                elif indirect in range(1,8): #RESERVED (0 - 7)
                    print("RESERVED INDIRECT BLOCK "+str(indirect)+" IN INODE "+str(i.inodeNumber)+" AT OFFSET "+str(offset))
                    exit_code = 2
                elif indirect in BFREE_List: #referenced but is also in free list
                    print("ALLOCATED BLOCK "+str(indirect)+" ON FREELIST")
                    exit_code = 2
                elif indirect not in legal_block_storage: #check for dupllicates
                    block_info = my_info("INDIRECT ", i.inodeNumber, offset)
                    legal_block_storage[indirect] = [block_info]
                elif indirect in legal_block_storage:
                    block_info = my_info("INDIRECT ", i.inodeNumber, offset)
                    legal_block_storage[indirect].append(block_info)
                    duplicates.add(indirect)
            
        #second indirect block
        if has_indirect_level_2 == True and hasattr(i, 'indirect_level_two'):
            indirect = i.indirect_level_two
            offset = 12 + 256
            if indirect != 0:
                if indirect < 0 or indirect > superBLK.numBlocks: #INVALID
                    print("INVALID DOUBLE INDIRECT BLOCK "+str(indirect)+" IN INODE "+str(i.inodeNumber)+" AT OFFSET "+str(offset))
                    exit_code = 2
                elif indirect in range(1,8): #RESERVED (0 - 7)
                    print("RESERVED DOUBLE INDIRECT BLOCK "+str(indirect)+" IN INODE "+str(i.inodeNumber)+" AT OFFSET "+str(offset))
                    exit_code = 2
                elif indirect in BFREE_List: #referenced but is also in free list
                    print("ALLOCATED BLOCK "+str(indirect)+" ON FREELIST")
                    exit_code = 2
                elif indirect not in legal_block_storage: #check for dupllicates
                    block_info = my_info("DOUBLE INDIRECT ", i.inodeNumber, offset)
                    legal_block_storage[indirect] = [block_info]
                elif indirect in legal_block_storage:
                    block_info = my_info("DOUBLE INDIRECT ", i.inodeNumber, offset)
                    legal_block_storage[indirect].append(block_info)
                    duplicates.add(indirect)

        #third indirect block
        if has_indirect_level_3 == True and hasattr(i, 'indirect_level_three'):
            indirect = i.indirect_level_three
            offset = 12 + 256 + 65536
            if indirect != 0:
                if indirect < 0 or indirect > superBLK.numBlocks: #INVALID
                    print("INVALID TRIPLE INDIRECT BLOCK "+str(indirect)+" IN INODE "+str(i.inodeNumber)+" AT OFFSET "+str(offset))
                    exit_code = 2
                elif indirect in range(1,8): #RESERVED (0 - 7)
                    print("RESERVED TRIPLE INDIRECT BLOCK "+str(indirect)+" IN INODE "+str(i.inodeNumber)+" AT OFFSET "+str(offset))
                    exit_code = 2
                elif indirect in BFREE_List: #referenced but is also in free list
                    print("ALLOCATED BLOCK "+str(indirect)+" ON FREELIST")
                    exit_code = 2
                elif indirect not in legal_block_storage: #check for dupllicates
                    block_info = my_info("TRIPLE INDIRECT ", i.inodeNumber, offset)
                    legal_block_storage[indirect] = [block_info]
                elif indirect in legal_block_storage:
                    block_info = my_info("TRIPLE INDIRECT ", i.inodeNumber, offset)
                    legal_block_storage[indirect].append(block_info)
                    duplicates.add(indirect)

def indirect_block_parser():
    #parse through blocks that are pointed to by those indirect blocks
    for indirect in INDIRECT_List:
        if indirect.referencedBlockNumber == 0:
            continue

        level = indirect.levelIndirection
        if level == 1: #indirect level 1
            storage = ""
        elif level == 2: #indirect level 2
            storage = "DOUBLE "
        elif level == 3: #indirect level 3
            storage = "TRIPLE "

        blockID = indirect.referencedBlockNumber
        offset = indirect.logicalBlockOffset
        if blockID < 0 or blockID > superBLK.numBlocks: #INVALID
            print("INVALID "+storage+"INDIRECT BLOCK "+str(blockID)+" IN INODE "+str(indirect.inodeNumber)+" AT OFFSET "+str(offset))
            exit_code = 2
        elif blockID in range(1,8): #RESERVED (0 - 7)
            print("RESERVED"+storage+"INDIRECT BLOCK "+str(blockID)+" IN INODE "+str(indirect.inodeNumber)+" AT OFFSET "+str(offset))
            exit_code = 2
        elif blockID in BFREE_List: #referenced but is also in free list
            print("ALLOCATED BLOCK "+str(blockID)+" ON FREELIST")
            exit_code = 2
        elif blockID not in legal_block_storage: #check for dupllicates
            block_info = my_info(storage, indirect.owningFileInodeNumber, offset)
            legal_block_storage[blockID] = [block_info]
        elif blockID in legal_block_storage:
            block_info = my_info(storage, indirect.owningFileInodeNumber, offset)
            legal_block_storage[blockID].append(block_info)
            duplicates.add(blockID)

def print_unreferenced_block():
    for num in range(8, superBLK.numBlocks):
        if num not in BFREE_List and num not in legal_block_storage:
            print("UNREFERENCED BLOCK "+str(num))
            exit_code = 2

def print_duplicates():
    for item in duplicates:
        for l in legal_block_storage[item]:
            print("DUPLICATE "+l.location+"BLOCK "+str(item)+" IN INODE "+str(l.inode)+" AT OFFSET "+str(l.offset))
            exit_code = 2

def inode_allocation_audit():
    for i in IFREE_List:
        global isreported
        isreported[i] = "unreported"
    #check for allocated inodes that are on free list
    #print(INODE_List)
    #print(IFREE_List)
    unused_inodes = list(range(11, superBLK.numInodes+1))
    #print(unused_inodes)
    for used_inode in INODE_List:
        #print(usedinode.inodeNumber)
        if used_inode.inodeNumber in unused_inodes:
            unused_inodes.remove(used_inode.inodeNumber)
        if used_inode.inodeNumber in IFREE_List:
            print("ALLOCATED INODE " + str(used_inode.inodeNumber) + " ON FREELIST")
            isreported[used_inode.inodeNumber] = "reported"
            exit_code = 2
    #print("UNALLOCATED INODE 17 NOT ON FREELIST")

    #check for free inodes that are not on free list
    for unused_inode in unused_inodes:
        if unused_inode not in IFREE_List:
            print("UNALLOCATED INODE " + str(unused_inode) + " NOT ON FREELIST")
            isreported[unused_inode] = "reported"
            exit_code = 2
            
          
def directory_consistency_audit():
    #for each directory entry, update number of links for each inode number
    numlinks = {}
    for inode_no in range(1, superBLK.numInodes+1):
        numlinks[inode_no] = 0

    #print(DIRENT_List)
    for dir_entry in DIRENT_List:
        if dir_entry.ref_file_inodeNumber not in numlinks: #invalid inode number
            print("DIRECTORY INODE " + str(dir_entry.parentInodeNumber) +
            " NAME " + dir_entry.name + " INVALID INODE " + str(dir_entry.ref_file_inodeNumber))
            exit_code = 2
        elif dir_entry.ref_file_inodeNumber in IFREE_List and (isreported[dir_entry.ref_file_inodeNumber] == "unreported"): #unallocated inode
            numlinks[dir_entry.ref_file_inodeNumber]+=1
            print("DIRECTORY INODE " + str(dir_entry.parentInodeNumber) +
            " NAME " + dir_entry.name + " UNALLOCATED INODE " + str(dir_entry.ref_file_inodeNumber))
            exit_code = 2
        else:
            numlinks[dir_entry.ref_file_inodeNumber]+=1
    
    #for each allocated inode, check if number of links equals linkcount
    directoryinodes = {}
    for used_inode in INODE_List:
        if used_inode.linkCount != numlinks[used_inode.inodeNumber]: #link count and numlinks doesnt match
            print("INODE " + str(used_inode.inodeNumber) + " HAS " + 
            str(numlinks[used_inode.inodeNumber]) + " LINKS BUT LINKCOUNT IS " + str(used_inode.linkCount))
            exit_code = 2
        if used_inode.fileType == "d":
            directoryinodes[used_inode.inodeNumber] = 0

    #go through directories and map directory:parent
    parentof = {}
    for mydir_entry in DIRENT_List:
        if ( (mydir_entry.name == "'.'") and (mydir_entry.parentInodeNumber != mydir_entry.ref_file_inodeNumber) ):
            print("DIRECTORY INODE " + str(mydir_entry.parentInodeNumber) +
             " NAME " + mydir_entry.name + " LINK TO INODE " + str(mydir_entry.ref_file_inodeNumber) + 
             " SHOULD BE " + str(mydir_entry.parentInodeNumber) )
            exit_code = 2
        elif (mydir_entry.name != "'.'") :
            if mydir_entry.name != "'..'" and (mydir_entry.ref_file_inodeNumber in directoryinodes):
                parentof[mydir_entry.ref_file_inodeNumber] = mydir_entry.parentInodeNumber

    for mydir_entry in DIRENT_List:
        if mydir_entry.parentInodeNumber in parentof:
            if ( (mydir_entry.name == "'..'") and (parentof[mydir_entry.parentInodeNumber] != mydir_entry.ref_file_inodeNumber) ):
                print("DIRECTORY INODE " + str(mydir_entry.parentInodeNumber) +
                " NAME " + mydir_entry.name + " LINK TO INODE " + str(mydir_entry.ref_file_inodeNumber) + 
                " SHOULD BE " + str(parentof[mydir_entry.parentInodeNumber]) )
                exit_code = 2
        else:
            if ( (mydir_entry.name == "'..'") and (mydir_entry.ref_file_inodeNumber != mydir_entry.parentInodeNumber) ):
                print("DIRECTORY INODE " + str(mydir_entry.parentInodeNumber) +
                " NAME " + mydir_entry.name + " LINK TO INODE " + str(mydir_entry.ref_file_inodeNumber) + 
                " SHOULD BE " + str(mydir_entry.parentInodeNumber) )
                exit_code = 2


def main():
    exit_code = 0

    if len(sys.argv) != 2:
        sys.stderr.write("Error, incorrect number of commands\n")
        exit(1)
    try:
        inputFile = open(sys.argv[1], "r")
    except:
        sys.stderr.write("Error, unable to open the input file\n")
        exit(1)

    #parse through the file
    csv_iterator = csv.reader(inputFile)
    for row in csv_iterator:
        name = row[0]
        #print(row)
        if name == 'SUPERBLOCK':
            global superBLK 
            superBLK = superblock(row)
            #print(superBLK.numBlocks)
        elif name == 'GROUP':
            groupInfo = group(row)
            GROUP_List.append(groupInfo)
        elif name == 'BFREE':
            numFreeBlocks = int(row[1]) #convert string to integer
            BFREE_List.append(numFreeBlocks)
        elif name == 'IFREE':
            numFreeInodes = int(row[1]) #convert string to integer
            IFREE_List.append(numFreeInodes)
        elif name == 'INODE':
            INODE_List.append(inode(row))
        elif name == 'DIRENT':
            DIRENT_List.append(directory(row))
        elif name == 'INDIRECT':
            INDIRECT_List.append(indirect_block_reference(row))

    inode_block_ptr_checker()
    indirect_block_parser()
    print_unreferenced_block()
    print_duplicates()
    inode_allocation_audit()
    directory_consistency_audit()

    # print(exit_code)
    exit(exit_code)

if __name__ == "__main__":
    main()    
