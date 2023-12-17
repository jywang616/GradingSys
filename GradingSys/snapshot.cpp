#include"snapshot.h"
#include"os.h"
#include<iostream>
#include<stdio.h>
#include<time.h>
#include<vector>
#include<cstdio>
#include<string>
#include<fstream>
#include<ctime>
#include<dirent.h>
#include<unistd.h>

using namespace std;


bool fullBackup() {
	//找到之前的fullbackup文件删掉
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir("./")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (strncmp(ent->d_name, "Full", 4) == 0) {
				string path = "./" + (string)ent->d_name;
				if (unlink(path.c_str())) {
					printf("Delete %s\n", ent->d_name);
				}
			}
			else if (strncmp(ent->d_name, "Incre", 5) == 0) {
				string path = "./" + (string)ent->d_name;
				if (unlink(path.c_str())) {
					printf("Delete %s\n", ent->d_name);
				}
			}
		}
	}

	//创建新的fullbackup文件
	time_t cur_time;
	time(&cur_time);
	//string bSysName;
	char backupSysName[256];
	memset(backupSysName, '\0', sizeof(backupSysName));
	strftime(backupSysName, sizeof(backupSysName), "FullBackupSys %Y.%m.%d %H-%M-%S.sys", localtime(&cur_time));
	printf("%s\n", backupSysName);
	if ((bfr = fopen(backupSysName, "rb")) == NULL) {
		bfw = fopen(backupSysName, "wb");
		if (bfw == NULL) {
			printf("BackupSys File error！");
			return false;
		}
		bfr = fopen(backupSysName, "rb");
		printf("Successfully open");
	}
	else {
		bfw = fopen(backupSysName, "rb+");
		if (bfw == NULL) {
			printf("BackupSys File error！");
			return false;
		}
	}

	//把备份的空间初始化
	char buffer[Disk_Size];
	char temp[BLOCK_SIZE];
	memset(buffer, '\0', sizeof(buffer));
	int c_Addr = 0;
	int num = Disk_Size / BLOCK_SIZE;
	for (int i = 0; i < num; i++) {

		memset(temp, '\0', sizeof(temp));
		fseek(fr, c_Addr, SEEK_SET);
		fread(temp, BLOCK_SIZE, 1, fr);
		fflush(fr);
		fseek(bfw, c_Addr, SEEK_SET);
		fwrite(temp, sizeof(temp), 1, bfw);
		fflush(bfw);
		c_Addr += BLOCK_SIZE;
		//printf("%d\n", c_Addr);
	}


	//char originbuf[Disk_Size];
	//fseek(fr, 0, SEEK_SET);
	//fread(originbuf, sizeof(originbuf), 1, fr);
	//fflush(fr);

	//fseek(bfw, 0, SEEK_SET);
	//fwrite(originbuf, sizeof(originbuf), 1, bfw);

	//清除位图标记
	char tmp_inodeBitmap[INODE_NUM];
	memset(tmp_inodeBitmap, 0, sizeof(tmp_inodeBitmap));
	for (int i = 0; i < INODE_NUM; i++) {
		tmp_inodeBitmap[i] = 0;
	}

	fseek(fw, Modified_inodeBitmap_Start_Addr, SEEK_SET);
	fwrite(tmp_inodeBitmap, sizeof(tmp_inodeBitmap), 1, fw);
	fflush(fw);

	time_t nowtime;
	time(&nowtime);
	last_backup_time = nowtime;
	fclose(bfw);
	fclose(bfr);
	return true;
}

//增量转存
bool incrementalBackup() {
	//创建新的sys文件
	time_t cur_time;
	char backupSysName[256] = { 0 };
	time(&cur_time);
	strftime(backupSysName, sizeof(backupSysName), "IncreBackupSys %Y.%m.%d %H-%M-%S.sys", localtime(&cur_time));
	string str(backupSysName);
	printf("%s", backupSysName);
	if ((bfr = fopen(backupSysName, "rb")) == NULL) {
		bfw = fopen(backupSysName, "wb");
		if (bfw == NULL) {
			printf("BackupSys File error！");
			return false;;
		}
		bfr = fopen(backupSysName, "rb");
		printf("BackupSys File successful open");
	}
	else {
		bfw = fopen(backupSysName, "rb+");
		if (bfw == NULL) {
			printf("BackupSys File error！");
			return false;
		}
	}

	//读出上一次增量转储后做修改的inode（区间改变）
	char new_modify_inodeBitmap[INODE_NUM];
	memset(new_modify_inodeBitmap, 0, sizeof(new_modify_inodeBitmap));
	//对每一个修改过的文件&全部目录在位图中做标记
	for (int i = 0; i < INODE_NUM; i++) {
		inode tmp_inode;
		fseek(fr, Inode_Start_Addr + i * sizeof(inode), SEEK_SET);
		fread(&tmp_inode, sizeof(inode), 1, fr);
		if (tmp_inode.file_modified_time > last_backup_time) {
			new_modify_inodeBitmap[i] = 1;
		}
		else {
			continue;
		}
	}
	//在备份文件中记录区间改变
	fseek(bfw, Backup_Start_Addr, SEEK_SET);
	fwrite(new_modify_inodeBitmap, sizeof(new_modify_inodeBitmap), 1, bfw);

	//读出上一次的modify_inodeBitmap
	char modify_inodeBitmap[INODE_NUM];
	memset(modify_inodeBitmap, 0, sizeof(modify_inodeBitmap));
	fseek(fr, Modified_inodeBitmap_Start_Addr, SEEK_SET);
	fread(modify_inodeBitmap, sizeof(modify_inodeBitmap), 1, fr);
	for (int i = 0; i < INODE_NUM; i++) {
		modify_inodeBitmap[i] |= new_modify_inodeBitmap[i];
	}
	//更新：在OS中记录modified inode bitmap
	fseek(fw, Modified_inodeBitmap_Start_Addr, SEEK_SET);
	fwrite(modify_inodeBitmap, sizeof(modify_inodeBitmap), 1, fw);


	//去每一个被修改的inode里找被对应修改的直接块（区间改变）
	int Cur_Addr = Backup_Block_Start_Addr;
	for (int i = 0; i < INODE_NUM; i++) {
		inode tmp_inode;
		if (new_modify_inodeBitmap[i] == 1) {
			char tmp[128];
			memset(tmp, '\0', sizeof(tmp));
			//读修改过的inode
			fseek(fr, Inode_Start_Addr + i * INODE_SIZE, SEEK_SET);//?
			fread(&tmp_inode, sizeof(inode), 1, fr);
			fflush(fr);
			//将修改过的inode写入增量备份系统
			fseek(bfw, Cur_Addr, SEEK_SET);
			fwrite(&tmp_inode, sizeof(inode), 1, bfw);
			fflush(bfw);
			Cur_Addr += sizeof(inode);
			//找inode直接块
			//int tmp_i_dirBlock[10];
			for (int j = 0; j < 10; j++) {
				if (tmp_inode.i_dirBlock[j] != -1) {
					//存直接块
					char tmp_block[BLOCK_SIZE];
					memset(tmp_block, '\0', sizeof(tmp_block));
					int block_addr = tmp_inode.i_dirBlock[j];
					fseek(fr, block_addr, SEEK_SET);
					fread(tmp_block, sizeof(tmp_block), 1, fr);
					fflush(fr);
					fseek(bfw, Cur_Addr, SEEK_SET);
					fwrite(tmp_block, sizeof(tmp_block), 1, bfw);
					Cur_Addr += sizeof(tmp_block);
				}
				else {
					continue;
				}
			}

		}
	}
	time_t curtime;
	time(&curtime);
	last_backup_time = curtime;
	return true;
}

bool recovery() {
	//获取当前文件夹下的所有文件
	DIR* dir;
	struct dirent* ent;
	//恢复全量备份
	if ((dir = opendir("./")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			printf("%s\t", ent->d_name);
			printf("\n");
			if (strncmp(ent->d_name, "Full", 4) == 0) {
				printf("%s", ent->d_name);
				//打开文件
				if ((bfr = fopen(ent->d_name, "rb")) == NULL) {
					bfw = fopen(ent->d_name, "wb");
					if (bfw == NULL) {
						printf("Error！");
						return false;;
					}
					bfr = fopen(ent->d_name, "rb");
					printf("Successful");
				}
				else {
					bfw = fopen(ent->d_name, "rb+");
					if (bfw == NULL) {
						printf("Error");
						return false;
					}
				}

				//从备份的文件中读
				//memset(buffer, '\0', sizeof(buffer));
				//fseek(bfr, 0, SEEK_SET);
				//fread(buffer, sizeof(buffer), 1, bfr);
				char buffer[Disk_Size];
				char temp[BLOCK_SIZE];
				memset(buffer, '\0', sizeof(buffer));
				int c_Addr = 0;
				int num = Disk_Size / BLOCK_SIZE;
				for (int i = 0; i < num; i++) {
					/*if (c_Addr == 11904) {
						inode tmp;
						fseek(fr, c_Addr, SEEK_SET);
						fread(&tmp, sizeof(inode), 1, fr);
						printf("%s", tmp.i_gname);
					}*/
					memset(temp, '\0', sizeof(temp));
					fseek(bfr, c_Addr, SEEK_SET);
					fread(temp, BLOCK_SIZE, 1, bfr);
					fflush(bfr);
					fseek(fw, c_Addr, SEEK_SET);
					fwrite(temp, sizeof(temp), 1, fw);
					fflush(fw);
					c_Addr += BLOCK_SIZE;
					//printf("%d\n", c_Addr);
				}
				break;
			}
			else {
				continue;
			}

		}
	}

	//找所有增量转储
	//统计有多少个增量转储
	vector<string> files;
	int incre_backup_count = 0;
	if ((dir = opendir("./")) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (strncmp(ent->d_name, "Incre", 5) == 0) {
				incre_backup_count++;
				files.push_back(ent->d_name);
			}
		}
	}
	//按修改时间从早到晚排序（从小到大）
	int size = files.size();
	for (int m = 0; m < size - 1; ++m) {
		for (int n = 0; n < size - 1; ++n) {
			if (strcmp(files[n].c_str(), files[n + 1].c_str()) > 0) {
				string tmp = files[n];
				files[n] = files[n + 1];
				files[n + 1] = tmp;
			}
		}
	}

	//从早到晚按顺序打开增量转储的文件
	for (int i = 0; i < incre_backup_count; i++) {
		//打开备份文件
		if ((bfr = fopen(files[i].c_str(), "rb")) == NULL) {
			bfw = fopen(files[i].c_str(), "wb");
			if (bfw == NULL) {
				printf("备份文件打开失败！");
				return false;;
			}
			bfr = fopen(files[i].c_str(), "rb");
			printf("备份文件打开成功");
		}
		else {
			bfw = fopen(files[i].c_str(), "rb+");
			if (bfw == NULL) {
				printf("备份文件打开失败");
				return false;
			}
		}

		int Cur_Addr = 0;
		//取备份文件中的inode位图
		char tmp_bitmap[1024];
		memset(tmp_bitmap, '\0', sizeof(tmp_bitmap));
		fseek(bfr, Cur_Addr, SEEK_SET);
		fread(&tmp_bitmap, sizeof(tmp_bitmap), 1, bfr);
		Cur_Addr += sizeof(tmp_bitmap);
		for (int k = 0; k < INODE_NUM; k++) {
			if (tmp_bitmap[k] == 1) {
				inode tmp;
				//把inode从增量备份文件系统中读出
				fseek(bfr, Cur_Addr, sizeof(inode));
				fread(&tmp, sizeof(inode), 1, bfr);
				Cur_Addr += sizeof(inode);
				//将inode写入原文件系统
				fseek(fw, Inode_Start_Addr + k * sizeof(inode), SEEK_SET);
				fwrite(&tmp, sizeof(inode), 1, fw);
				//处理直接块
				for (int i = 0; i < 10; i++) {
					if (tmp.i_dirBlock[i] != -1) {
						//把备份文件中的直接块读出
						char tmp_block[512];
						memset(tmp_block, '\0', sizeof(tmp_block));
						fseek(bfr, Cur_Addr, SEEK_SET);
						fread(&tmp_block, sizeof(tmp_block), 1, bfr);
						Cur_Addr += sizeof(tmp_block);
						//写入源文件对应块地址
						fseek(fw, tmp.i_dirBlock[i], SEEK_SET);
						fwrite(&tmp_block, sizeof(tmp_block), 1, fw);
					}
				}
			}
		}
	}
	return true;
}