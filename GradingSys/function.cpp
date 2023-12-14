#include"os.h"
#include"function.h"
#include<cstring>
#include<cstdio>
#include<iostream>
#include"role.h"

using namespace std;

void cmd(char cmd_str[]) {
	char com1[100];
	char com2[100];
	char com3[100];
	char com4[100];
	sscanf(cmd_str, "%s", com1);
	if (strcmp(com1, "help") == 0) {
		help();
	}
	else if (strcmp(com1, "ls") == 0) {
		sscanf(cmd_str, "%s%s", com1, com2);
		ls(com2);
	}
	else if (strcmp(com1, "cd") == 0) {
		sscanf(cmd_str, "%s%s", com1, com2);
		cd_func(Cur_Dir_Addr, com2);
	}
	else if (strcmp(com1, "gotoRoot") == 0) {
		gotoRoot();
	}
	else if (strcmp(com1, "mkdir") == 0) {	
		sscanf(cmd_str, "%s%s", com1, com2);
		mkdir_func(Cur_Dir_Addr, com2);
	}
	else if (strcmp(com1, "rm") == 0) {
		sscanf(cmd_str, "%s%s%s", com1, com2, com3);
		rm_func(Cur_Dir_Addr, com3, com2);
	}
	else if (strcmp(com1, "touch") == 0) {
		sscanf(cmd_str, "%s%s", com1, com2);
		touch_func(Cur_Dir_Addr, com2, "");
	}
	else if (strcmp(com1, "echo") == 0) {
		sscanf(cmd_str, "%s%s%s%s", com1, com2, com3, com4);
		echo_func(Cur_Dir_Addr, com4, com3, com2);
	}
	else if (strcmp(com1, "cat") == 0) {
		sscanf(cmd_str, "%s%s", com1, com2);
		cat(Cur_Dir_Addr, com2);
	}
	else if (strcmp(com1, "chmod") == 0) {
		sscanf(cmd_str, "%s%s%s", com1, com2, com3);
		chmod_func(Cur_Dir_Addr, com2, com3);
	}
	else if (strcmp(com1, "chown") == 0) {
		sscanf(cmd_str, "%s%s%s", com1, com2, com3);
		chown_func(Cur_Dir_Addr, com2, com3);
	}

	else if (strcmp(com1, "useradd") == 0) {
		//useradd -g group -m user
		char group[100];
		char user[100];
		char passwd[100];
		sscanf(cmd_str, "%s%s%s%s%s", com1, com2, group, com3, user);
		if ((strcmp(com2, "-g") != 0) || ((strcmp(com3, "-m") != 0))) {
			printf("鍛戒护鏍煎紡閿欒!\n");
			return;
		}
		inPasswd(passwd);
		useradd(user,passwd, group);
	}
	else if (strcmp(com1, "userdel") == 0) {
		sscanf(cmd_str, "%s%s", com1, com2);
		userdel(com2);
	}
	else if (strcmp(com1, "groupadd") == 0) {
		sscanf(cmd_str, "%s%s", com1, com2);
		groupadd(com2);
	}
	else if (strcmp(com1, "groupdel") == 0) {
		sscanf(cmd_str, "%s%s", com1, com2);
		groupdel(com2);
	}
	else if (strcmp(com1, "passwd") == 0) {
		if (sscanf(cmd_str, "%s%s", com1, com2) == 1) {
			passwd_func("");
		}
		else {
			passwd_func(com2);
		}
	}
	else if (strcmp(com1, "logout") == 0) {
		logout();
	}
	//澶囦唤绯荤粺&鎭㈠绯荤粺
	else if (strcmp(com1, "exit") == 0) {
		cout << "閫�鍑烘垚缁╃鐞嗙郴缁燂紝鎷滄嫓锛�" << endl;
		exit(0);
	}
	
	//root缁勭壒鏈�
	if (strcmp(Cur_Group_Name, "root") == 0) {
		if (strcmp(com1, "batchadd") == 0) {
			sscanf(cmd_str, "%s", com1);
			add_users(STUDENT_COURSE_LIST);
		}
		else if (strcmp(com1, "fullbackup") == 0) {
			fullBackup();
		}
		else if (strcmp(com1, "recovery") == 0) {
			recovery();
		}
		else if (strcmp(com1, "increbackup") == 0) {
			incrementalBackup();
		}
	}
	
	//teacher缁勭壒鏈�
	if (strcmp(Cur_Group_Name, "teacher") == 0) {
		if (strcmp(com1, "publish_task") == 0) {
			sscanf(cmd_str, "%s%s%s", com1, com2, com3);
			publish_task(com2, com3);
		}
		else if (strcmp(com1, "judge_hw") == 0) {
			sscanf(cmd_str, "%s%s%s", com1, com2, com3);
			judge_hw(STUDENT_COURSE_LIST, com2, com3);
		}
	}
  
  //student缁勭壒鏈�
	if (strcmp(Cur_Group_Name, "student") == 0) {
		if (strcmp(com1, "check_hw_content") == 0) //check desription
		{
			// check lesson hw
			sscanf(cmd_str, "%s%s%s", com1, com2, com3);
			check_hw_content(com2, com3);
		}
		else if (strcmp(com1, "check_hw_score") == 0)
		{
			// check_hw_score lesson hw
			sscanf(cmd_str, "%s%s%s", com1, com2, com3);
			check_hw_score(com2, com3);
		}
		else if (strcmp(com1, "submit_hw_to") == 0)
		{
			// submit_hw_to lesson hwname
			sscanf(cmd_str, "%s%s%s", com1, com2, com3);
			submit_assignment(Cur_User_Name, com2, com3);
		}

	}

	return;
}

void help() {
	cout.setf(ios::left);
	cout.width(30);
	cout << "ls [-l]" << "Display the current directory listing" << endl;
	cout.width(30);
	cout << "cd [path]" << "Enter the specific directory " << endl;
	cout.width(30);
	cout << "gotoRoot " << "Return to the root directory " << endl;
	cout.width(30);
	cout << "mkdir [path]" << "Create directory" << endl;
	cout.width(30);
	cout << "rm [-rf dir_path] | [-f file_path]" << "Delete directory or file" << endl;
	cout.width(30);
	cout << "touch" << "Create a blank file" << endl;
	cout.width(30);
	cout << "echo \"content\" > [path] | \"content\" >> [path]" << "Write or add a file" << endl;
	cout.width(30);
	cout << "cat [file_path]" << "View the file content" << endl;
	cout.width(30);
	cout << "chmod [ugoa]*[-+=]*[rwx] | [0-7]+" << "Modify the access right" << endl;
	cout.width(30);
	cout << "chown [user:group] [path]" << "Modify user and group in file" << endl;
	cout.width(30);

	cout << "useradd [-g group] [-m] [user]" << "Add user" << endl;
	cout.width(30);
	cout << "userdel [user]" << "Delete user" << endl;
	cout.width(30);
	cout << "groupadd [group]" << "Add group" << endl;
	cout.width(30);
	cout << "groupdel [group]" << "Delete group" << endl;
	cout.width(30);
	cout << "passwd [user]" << "Modify the password" << endl;
	cout.width(30);
	cout << "logout" << "Logout the account" << endl;
	cout.width(30);
	cout << "exit" << "Exit the system" << endl;
	cout.width(30);
}

bool cd_func(int CurAddr, char* str) {
	int pro_cur_dir_addr = Cur_Dir_Addr;
	char pro_cur_dir_name[310];
    memset(pro_cur_dir_name, '\0', 310);
    strcpy(pro_cur_dir_name, Cur_Dir_Name);
	int flag = 1;
	if (strcmp(str, "/") == 0) {
		gotoRoot();
		return true;
	}
	if (str[0] == '/') {
		gotoRoot();
		str += 1;
	}
	char name[strlen(str) + 1];
	while (strlen(str) != 0) {
		int i = 0;
		memset(name, '\0', sizeof(name));
		while (((*str) != '/') && (strlen(str) != 0)) {
			name[i++] = str[0];
			str += 1;
		}
		if ((*str) == '/') str += 1;
		if (strlen(name) != 0) {
			if (cd(Cur_Dir_Addr, name) == false) {
				flag = 0;
				break;
			}
		}
		else {
			break;
		}
	}

	if (flag == 0) {
		Cur_Dir_Addr = pro_cur_dir_addr;
		strcpy(Cur_Dir_Name, pro_cur_dir_name);
		return false;
	}
	return true;
}

bool mkdir_func(int CurAddr, char* str) {//在任意目录下创建目录
	//绝对,相对,直接创建
	char* p = strrchr(str, '/');
	if (p == NULL) {	//直接创建
		if (mkdir(CurAddr, str)) { return true; }
		else { return false; }
	}
	else {
		char name[File_Max_Size];
		memset(name, '\0', sizeof(name));
		p++;
		strcpy(name, p);
		*p = '\0';
		if (cd_func(CurAddr, str)) {
			if (mkdir(Cur_Dir_Addr, name))
				return true;
		}
		return false;
	}
}

bool rm_func(int CurAddr, char* str, char* s_type) {//在任意目录下删除
	//文件类型
	int type = -1;
	if (strcmp(s_type, "-rf") == 0) {
		type = 1;
	}
	else if (strcmp(s_type, "-f") == 0) {
		type = 0;
	}
	else {
		printf("Invalid command! Please try again!\n");
		return false;
	}

	char* p = strrchr(str, '/');
	if (p == NULL) {
		if (rm(CurAddr, str, type))	return true;
		return false;
	}
	else {
		char name[File_Max_Size];
		memset(name, '\0', sizeof(name));
		p++;
		strcpy(name, p);
		*p = '\0';
		if (cd_func(CurAddr, str)) {
			if (rm(Cur_Dir_Addr, name, type))
				return true;
		}
		return false;
	}
}

bool touch_func(int CurAddr, char* str, char* buf) {
	char* p = strrchr(str, '/');
	if (p == NULL) {
		if (mkfile(CurAddr, str, buf))	return true;
		return false;
	}
	else {
		char name[File_Max_Size];
		memset(name, '\0', sizeof(name));
		p++;
		strcpy(name, p);
		*p = '\0';
		if (cd_func(CurAddr, str))
        {
			if (mkfile(Cur_Dir_Addr, name, buf)) {
                return true;
            }
		}
		return false;
	}
}

bool echo_func(int CurAddr, char* str, char* s_type, char* buf) {//在任意目录下创建or覆盖写入or追加
	//判断类型 0：覆盖写入 1：追加
	int type = -1;
	if (strcmp(s_type, ">") == 0) {
		type = 0;
	}
	else if (strcmp(s_type, ">>") == 0) {
		type = 1;
	}
	else {
		printf("echo输入格式错误，请输入正确格式!\n");
		return false;
	}

	//寻找直接地址
	char* p = strrchr(str, '/');
	char name[File_Max_Size];
	memset(name, '\0', sizeof(name));
	if (p != NULL) {
		p++;
		strcpy(name, p);
		*p = '\0';
		if (cd_func(CurAddr, str) == false) {
			return false;
		}
	}
	else {
		strcpy(name, str);
	}

	//类型执行
	if (echo(Cur_Dir_Addr, name, type, buf))	return true;
	return false;
}
bool chmod_func(int CurAddr, char* pmode, char* str) {
	char* p = strrchr(str, '/');
	char name[File_Max_Size];
	memset(name, '\0', sizeof(name));
	if (p != NULL) {
		p++;
		strcpy(name, p);
		*p = '\0';
		if (cd_func(CurAddr, str) == false) {
			return false;
		}
	}
	else {
		strcpy(name, str);
	}

	//类型执行
	if (chmod(CurAddr, name, pmode))	return true;
	return false;
}
bool chown_func(int CurAddr, char* u_g, char* str) {
	char* p = strrchr(str, '/');
	char file[File_Max_Size];
	memset(file, '\0', sizeof(file));
	if (p != NULL) {
		p++;
		strcpy(file, p);
		*p = '\0';
		if (cd_func(CurAddr, str) == false) {
			return false;
		}
	}
	else {
		strcpy(file, str);
	}


	//获取用户和用户组
	p = strstr(u_g, ":");
	char name[20], group[20];
	memset(name, '\0', strlen(name));
	memset(group, '\0', strlen(group));
	if (p == NULL) {
		strcpy(name, u_g);
	}
	else {
		strncpy(name, u_g, p - u_g);
		p += 1;
		strcpy(group, p);
	}
	if (chown(CurAddr, file, name, group))	return true;
	return false;
}
bool passwd_func(char* username) {
	if ((strcmp(Cur_Group_Name, "root") != 0) && (strlen(username) != 0)) {
		printf("Only root can change user's password!\n");
		return false;
	}

	char pwd[100];
	printf("Changing password for user %s\n", Cur_User_Name);
	printf("New password: ");
	gets(pwd);
	char re_pwd[100];
	printf("Retype new password:");
	gets(re_pwd);

	if (strcmp(pwd, re_pwd) == 0) {
		if (passwd(username, pwd) == 0) {
			return true;
		}
	}
	return false;
}
