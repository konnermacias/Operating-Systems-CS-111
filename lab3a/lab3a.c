// NAME: Konner Macias,Cavan Stewart
// EMAIL: konnermacias@g.ucla.edu,cavanstewart@gmail.com
// ID: 004603916,304654853

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "ext2_fs.h"

/* global variables */
int fd = 0;
uint32_t bl_size = 0;
uint32_t tot_blocks = 0;
uint32_t tot_inodes = 0;
uint32_t first_non = 0;
uint16_t gr_freeInodes = 0;
uint32_t gr_inodes = 0;
uint32_t bg_inode_table = 0;
uint32_t i_size = 0;
uint32_t i_group = 0;

void errorMe(char* str) {
	
	fprintf(stderr, "Error! %s: %s",str, strerror(errno));
	close(fd);
	exit(1);
}

void superB() {
	
	int sb_size = 1024;
	int sb_offset = 1024;
	struct ext2_super_block superblock;

	if (pread(fd, &superblock, sb_size, sb_offset) == -1) {
		errorMe("Problem reading superblock");
	}

	tot_blocks = superblock.s_blocks_count;
	tot_inodes = superblock.s_inodes_count;
	bl_size = 1024 << superblock.s_log_block_size;
	i_size = superblock.s_inode_size;
	uint32_t bl_group = superblock.s_blocks_per_group;
	i_group = superblock.s_inodes_per_group;
	first_non = superblock.s_first_ino;

	printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n",
							tot_blocks,
							tot_inodes,
							bl_size,
							i_size,
							bl_group,
							i_group,
							first_non);
}

void scanEntries(char *str, uint32_t bg_bitmap) {
	/* Scan entries */	
	/* Block N starts at byte (n*1024(bl_size)) */
	unsigned int i,j;
	for (i = 0; i < tot_blocks; i++) {
		int buf = 0; // 
		if (pread(fd,&buf,1,bg_bitmap*bl_size+i) == -1){
			errorMe("Problem reading bitmap");
		}
		/* we now have sequence of 8 bits */
		/* when bit is 1 -> used, when 0 -> free */
		/* to test a bit: (A & 1 << bit) != 0 */
		for (j = 0; j < 8; j++) {
			if ((buf & 1 << j) == 0) {
				/* print out index of block number times 8 bits
					plus however much we indexed +1 bc of offset */
				printf("%s,%d\n",
						str,
						i*8+j+1);
			}
		}
	}
}

void groupSum() {

	struct ext2_group_desc group;
	int gr_size = 32;
	int gr_offset = 1024 + bl_size; // should be 2048 in trivial
	
	if (pread(fd, &group, gr_size, gr_offset) == -1) {
		errorMe("Problem reading in group info");
	}

	uint32_t gr_num = 0; // we can assume just for single group
	uint32_t gr_blks = tot_blocks;
	gr_inodes = tot_inodes;
	uint16_t gr_freeblks = group.bg_free_blocks_count;
	gr_freeInodes = group.bg_free_inodes_count;
	uint32_t bg_blk_bitmap = group.bg_block_bitmap;
	uint32_t bg_inode_bitmap = group.bg_inode_bitmap;
	bg_inode_table = group.bg_inode_table;

	printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",
						gr_num,
						gr_blks,
						gr_inodes,
						gr_freeblks,
						gr_freeInodes,
						bg_blk_bitmap,
						bg_inode_bitmap,
						bg_inode_table);

	scanEntries("BFREE",bg_blk_bitmap);
	scanEntries("IFREE",bg_inode_bitmap);
}


int dirSum(int inode_num, int dirOff, uint32_t blk) {
	
	struct ext2_dir_entry dir;
	unsigned int offset = blk*bl_size;

	/* read all entries in directory */
	while (offset < (blk+1)*bl_size) {
		if (pread(fd, &dir, sizeof(struct ext2_dir_entry), offset) == -1) {
			errorMe("Problem with reading directory");
		}
		/* Make sure I-node is non-zero */
		char name[EXT2_NAME_LEN+1]; // account for null
		memcpy(name, dir.name, dir.name_len);
		name[dir.name_len] = 0;
		if (dir.inode != 0) {
			printf("DIRENT,%d,%d,%d,%d,%d,\'%s\'\n",
								inode_num,
								dirOff,
								dir.inode,
								dir.rec_len,
								dir.name_len,
								name);
		}
		/* move to next entry while updating offsets */
		offset += dir.rec_len;
		dirOff += dir.rec_len;
	}

	return dirOff;	
}

void indirSum(int inode_num, int level, int dirOff, uint32_t blk) {
	
	uint32_t buf;
	unsigned int i, indirOffset;
	uint32_t n_references = bl_size/sizeof(uint32_t);

	for (i = 0; i < n_references; i++) {
		/* Use i to select next entry within blk */
		indirOffset = blk*bl_size + i*sizeof(uint32_t);
		if (pread(fd, &buf, sizeof(uint32_t), indirOffset) == -1) {
			errorMe("Problem with indirect entry");
		}
		/* for each non-zero block pointer... */
		if (buf != 0) {
			/* based on level, we need to adjust offset */
			if (level == 3) {
				dirOff += i*65536;
			} else if (level == 2) {
				dirOff += i*256;
			} else {
				dirOff += i;
			}
			printf("INDIRECT,%d,%d,%d,%d,%d\n",
								inode_num,
								level,
								dirOff,
								blk,
								buf);

			/* scan recursively */
			if (level > 1) {
				indirSum(inode_num,level-1,dirOff,buf);
			}
		}
	}
}

int dirIndir(int inode_num, int level, int dirOff, uint32_t blk) {
	
	unsigned int i, indirOffset;
	uint32_t buf;
	uint32_t n_references = bl_size/sizeof(uint32_t);

	for (i = 0; i < n_references; i++) {
		indirOffset = blk*bl_size + i*sizeof(uint32_t);
		if (pread(fd, &buf, sizeof(uint32_t), indirOffset) == -1) {
			errorMe("Problem with reading indirect entry");
		}
		/* make sure block pointer is non-zero */
		if (buf != 0) {
			if (level > 0) {
				dirOff = dirSum(inode_num,dirOff,buf);
			}
			else {
				/* recurse */
				dirOff = dirIndir(inode_num,level-1,dirOff,buf);
			}
		}
	}
	return dirOff;
}


void inodeSum() {

	/* 1st inode is always inode 2 */
	unsigned int i, j;
	struct ext2_inode node;
	for (i = 0; i < tot_inodes; i++) {
		if (pread(fd, &node, sizeof(struct ext2_inode), bg_inode_table*bl_size + i_size*i) == -1) {
			errorMe("Problem reading inode table");
		}
		/* check i_mode and i_links_count are non-zero */
		if (node.i_mode == 0 || node.i_links_count == 0){
			continue;
		}

		/* check file type */
		char c;
		if (S_ISDIR(node.i_mode)) {
			c = 'd';
		}else if (S_ISREG(node.i_mode)) {
			c = 'f';
		}else if (S_ISLNK(node.i_mode)) {
			c = 's';
		}else{
			c = '?';
		}

		/* mode (low order 12 bits) */
		unsigned mode = node.i_mode & 0xFFF;
		uint16_t i_uid = node.i_uid;
		uint16_t i_gid = node.i_gid;
		uint16_t lnkCount = node.i_links_count;
		uint32_t i_file_size = node.i_size;
		uint32_t i_blocks = node.i_blocks;

		/* 
			need to convert time to:
			mm/dd/yy hh:mm:ss, GMT
		*/

		char cBuf[20], mBuf[20], aBuf[20];

		time_t ctime, mtime, atime;
		struct tm ctm, mtm, atm;
		ctime = node.i_ctime;
		mtime = node.i_mtime;
		atime = node.i_atime;

		ctm = *gmtime(&ctime);
		mtm = *gmtime(&mtime);
		atm = *gmtime(&atime);

		strftime(cBuf,20,"%D %H:%M:%S", &ctm);
		strftime(mBuf,20,"%D %H:%M:%S", &mtm);
		strftime(aBuf,20,"%D %H:%M:%S", &atm);

		/*
			owner,group,link count, time of last I-node change
			modif time, time of last access, file size, number
			blocks of disk space taken up by this file
		*/

		printf("INODE,%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d",
							i+1,
							c,
							mode,
							i_uid,
							i_gid,
							lnkCount,
							cBuf,
							mBuf,
							aBuf,
							i_file_size,
							i_blocks);


		if (c == 'f' || c == 'd' || (c == 's' && i_file_size > 60*sizeof(char))) {
			for(j=0; j< EXT2_N_BLOCKS; j++) {
				printf(",%u",node.i_block[j]);
			}
		} else if (c == 's') {
			printf(",%u", node.i_block[0]); // for trivial csv
		}
		printf("\n");

		/* Directory entries */
		uint32_t dirOff = 0;
		if (c == 'd') {
			for (j = 0; j < 15; j++) {
				if (node.i_block[j] != 0) {
					if (j < 12) {
						dirOff = dirSum(i+1, dirOff, node.i_block[j]);
					}
					else {
						dirOff = dirIndir(i+1, j-11, dirOff, node.i_block[j]);
					}
				}				
			}
		}

		if (c == 'd' || c == 'f') {
			indirSum(i+1,1,12,node.i_block[12]);
			indirSum(i+1,2,268,node.i_block[13]); // 256 + 12
			indirSum(i+1,3,65804,node.i_block[14]); // 65536 + 256 + 12
		}

	}	
}

int main(int argc, char **argv) {

	if (argc != 2) {
		errorMe("Incorrect number of arguments");
	}

	if ((fd = open(argv[1], O_RDONLY)) == -1) {
		errorMe("Problem with opening file image");
	}

	superB();
	groupSum();
	inodeSum();
	close(fd);
	return 0;
}