#!/usr/bin/python

# NAME: Konner Macias,Cavan Stewart
# EMAIL: konnermacias@g.ucla.edu,cavanwstewart@gmail.com
# ID: 004603916,304654853

import sys
import csv
import os



superblock = {
    "num_blocks" : 0,
    "num_inodes" : 0,
    "block_size" : 0,
    "inode_size" : 0,
    "blocks_per_group" : 0,
    "inodes_per_group" : 0,
    "first_inode" : 0
}
    
    
group = []
bfree = []
ifree = []
dirent = []
inode = []
indirect = []
file_blocks = []
indirect_blocks = []
alloc_inodes = []
exit_code = 0;


def print_error(msg):
    sys.stderr.write(msg)
    sys.exit(1)


ind_level = ["", "INDIRECT ", "DOUBLE INDIRECT ", "TRIPLE INDIRECT "]


def block_handler(block_num, inode_num, level, offset):
    if block_num < 0 or block_num > superblock['num_blocks'] - 1:
        exit_code = 2
        print("INVALID {}BLOCK {} IN INODE {} AT OFFSET {}".format(ind_level[level], block_num,
                                                                inode_num, offset))

    elif block_num > 0 and block_num < 8:
        exit_code = 2
        print("RESERVED {}BLOCK {} IN INODE {} AT OFFSET {}".format(ind_level[level], block_num,
                                                                 inode_num, offset))
    if block_num in bfree:
        exit_code = 2
        print("ALLOCATED BLOCK {} ON FREELIST".format(block_num))

    dup = 0
    for k in range(len(file_blocks)):
        if block_num == file_blocks[k] and dup:
            exit_code = 2
            print("DUPLICATE {}BLOCK {} IN INODE {} AT OFFSET {}".format(ind_level[level], block_num, inode_num, offset))
            break
        if block_num == file_blocks[k]:
            dup = 1
    
        
              
    
    
def block_audit():
    #invalid
    #for i in inode
    #block = inode['num_dblocks']
    #if block != 0:
    #    if block > superblock[num_blocks] - 1:
    
    #for i, num_dblocks in enumerate(d['num_blocks'] for d in inode):
     #   block_handler(inode[''

    for dic in inode:
        if ((dic['file_size'] >= 60 and dic['file_type'] == 's') or dic['file_type'] == 'f' or dic['file_type'] == 'd'):
            for block in range(len(dic['blocks'])):
                if dic['blocks'][block] >= 8 and dic['blocks'][block] < superblock['num_blocks']:
                    file_blocks.append(dic['blocks'][block])
                    
            if dic['single_ind'] >= 8 and dic['single_ind'] < superblock['num_blocks']:
                file_blocks.append(dic['single_ind'])
            if dic['double_ind'] >= 8 and dic['double_ind'] < superblock['num_blocks']:
                file_blocks.append(dic['double_ind'])
            if dic['triple_ind'] >= 8 and dic['triple_ind'] < superblock['num_blocks']:
                file_blocks.append(dic['triple_ind'])
        else:
            continue
        
    for dic in indirect:
        if dic['ref_block'] >= 8 and dic['ref_block'] < superblock['num_blocks']:
            file_blocks.append(dic['ref_block'])
        

    #print('\n'.join(map(str,file_blocks)))
    for dic in inode:
        if dic['file_type'] != 's':
            for block in range(len(dic['blocks'])):
                #print dic['blocks'][block]
                block_handler(dic['blocks'][block], dic['inode_num'], 0, 0)
            block_handler(dic['single_ind'], dic['inode_num'], 1, 12)
            block_handler(dic['double_ind'], dic['inode_num'], 2, 268)
            block_handler(dic['triple_ind'], dic['inode_num'], 3, 65804)
            
        else:
            continue
    for dic in indirect:
        block_handler(dic['ref_block'], dic['inode_num'], 0, dic['offset'])

    for i in range(8,superblock['num_blocks']):
        if i not in bfree and i not in file_blocks:
            exit_code = 2
            print("UNREFERENCED BLOCK {}".format(i))

def dir_consis_audit():
	# enumerate discovered links within a directory (sum)
	# create a list of link counts so that we can index it by inode_num
	lc = dict()
	parents = dict()
	for i in range(superblock['num_inodes']+1):
		lc[i] = 0
		parents[i] = 0

	parents[2] = 2 # parent of root is root.

	for diren in dirent:
		# check if invalid
		if diren['inode_num'] > superblock['num_inodes'] or diren['inode_num'] < 1:
			exit_code = 2
      			print("DIRECTORY INODE {} NAME {} INVALID INODE {}".format(diren['parent'],diren['name'],diren['inode_num']))
		# check if unallocated
		elif diren['inode_num'] not in alloc_inodes:
			exit_code = 2
      			print("DIRECTORY INODE {} NAME {} UNALLOCATED INODE {}".format(diren['parent'],diren['name'],diren['inode_num']))
		else:
			# this directory node is valid and allocated
			# increment link count at this inode_num
			lc[diren['inode_num']] += 1
			if (diren['name'] != "'.'" and diren['name'] != "'..'"):
				parents[diren['inode_num']] = diren['parent']
			# check for correctness of two links
			if (diren['name'] == "'.'" and diren['parent'] != diren['inode_num']):
				exit_code = 2
				print("DIRECTORY INODE {} NAME '{}' LINK TO INODE {} SHOULD BE {}".format(diren['parent'],diren['name'],diren['inode_num'],diren['parent']))
			elif (diren['name'] == "'..'" and parents[diren['parent']] != diren['inode_num']):
				exit_code = 2
				print("DIRECTORY INODE {} NAME {} LINK TO INODE {} SHOULD BE {}".format(diren['parent'],diren['name'],diren['inode_num'],diren['parent']))
	
	# check for errors in link counts
	for node in inode:
		if node['link_count'] != lc[node['inode_num']]:
			exit_code = 2
      			print("INODE {} HAS {} LINKS BUT LINKCOUNT IS {}".format(node['inode_num'],lc[node['inode_num']],node['link_count']))




def inode_alloc_audit():
	# generate list of allocated inodes
	
	#unalloc_inodes = []
	#for node in inode:
	#	if (node['file_type'] == 'f' or node['file_type'] == 'd' or node['file_type'] == 's'): # make sure allocated
	#		alloc_inodes.append(node['inode_num'])
	#	elif (node['file_type'] == 0):
	#		unalloc_inodes.append(node['inode_num'])

	for node in inode:
		if node['file_type'] != 0:
			alloc_inodes.append(node['inode_num'])

	# now compare against free list
	# alloc -> free list search
	# ALLOCATED INODE # ON FREELIST
	for node in alloc_inodes:
		if node in ifree:
			exit_code = 2
			print("ALLOCATED INODE {} ON FREELIST".format(node))
	# check if unallocated inode is not on free list
	for node in range(superblock['first_inode'],superblock['num_inodes']):
		if node not in alloc_inodes and node not in ifree:
			exit_code = 2
			print("UNALLOCATED INODE {} NOT ON FREELIST".format(node))            
    
if __name__ == '__main__':
    if(len(sys.argv)) != 2:
        print_error("Usage: ./lab3b FILENAME\n")

    try:
	file = open(sys.argv[1],'r')
    except IOError:
	print_error("Could not open file\n")

    with file as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            
            if len(row) <= 0:
                print_error("Error: blank line\n")
                
            if row[0] == 'SUPERBLOCK':
                superblock['num_blocks'] = int(row[1])
                superblock['num_inodes'] = int(row[2])
                superblock['block_size'] = int(row[3])
                superblock['inode_size'] = int(row[4])
                superblock['blocks_per_group'] = row[5]
                superblock['inodes_per_group'] = row[6]
                superblock['first_inode'] = int(row[7])
            elif row[0] == 'GROUP':
                group.append({ "group_num" : row[1], "num_blocks" : row[2], "num_inodes" : row[3],
                               "free_blocks" : row[4], "free_inodes" : row[5], "num_bmap" : row[6],
                               "num_imap" : int(row[7]), "num_first_inode" : row[8]})
            elif row[0] == 'BFREE':
                bfree.append(int(row[1]))                
            elif row[0] == 'IFREE':
                ifree.append(int(row[1]))
            elif row[0] == 'INODE':
                if ((int(row[10]) >= 60 and row[2] == 's') or row[2] == 'f' or row[2] == 'd'):
                    inode.append({ "inode_num" : int(row[1]),
                                   "file_type" : row[2],
                                   "mode" : row[3],
                                   "owner" : row[4],
                                   "group" : row[5],
                                   "link_count" : int(row[6]),
                                   "last_change" : row[7],
                                   "mod_time" : row[8],
                                   "last_access" : row[9],
                                   "file_size" : int(row[10]),
                                   "num_blocks" : int(row[11]),
                                   "blocks" : map(int, row[12:24]),
                                   "single_ind" : int(row[24]),
                                   "double_ind" : int(row[25]),
                                   "triple_ind" : int(row[26])
                    })
                else:
                    inode.append({ "inode_num" : int(row[1]),
                                   "file_type" : row[2],
                                   "mode" : row[3],
                                   "owner" : row[4],
                                   "group" : row[5],
                                   "link_count" : int(row[6]),
                                   "last_change" : row[7],
                                   "mod_time" : row[8],
                                   "last_access" : row[9],
                                   "file_size" : int(row[10]),
                                   "num_blocks" : int(row[11]),
                    })
            elif row[0] == 'DIRENT':
                dirent.append({ "parent" : int(row[1]),
                                "offset" : int(row[2]),
                                "inode_num" : int(row[3]),
                               "length" : int(row[4]),
                                "name_length" : int(row[5]),
                                "name" : row[6]})
            elif row[0] == 'INDIRECT':
                indirect.append({ "inode_num" : int(row[1]),
                                  "level" : int(row[2]),
                                  "offset" : row[3],
                                  "indir_block" : row[4],
                                  "ref_block" : int(row[5])})
            else:
                print_error("Error: Unrecognized line\n")
        
                
        block_audit()
        inode_alloc_audit()
        dir_consis_audit()
        
	sys.exit(exit_code)

            
