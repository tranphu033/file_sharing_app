#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mysql/mysql.h>

#define BUFF_SIZE 1024

void gen_gcode(char *code);
void get_curtime(char *output);
int str_split(char *str, char *pat, char **out);

int excute_query(MYSQL *conn, char *sql); //thuc thi 1 truy van (thong bao neu co loi xay ra)
//checking functions:
int check_role(MYSQL *conn, char *groupID, char *userID); //kiem tra quyen admin cua nhom
int check_gname_exist(MYSQL *conn, char *gcode); //kiem tra group name da ton tai
int check_gcode_exist(MYSQL *conn, char *gcode);
int check_mem_exist(MYSQL *conn, char *memID, char *groupID);
int check_folname_exist(MYSQL *conn, char *folname, char *pfolderID);
int check_uname_exist(MYSQL *conn, char *uname);
int check_userID_exist(MYSQL *conn, char *uname);

void gen_gcode(char *code){
    int i, t, j=0;
    char alpha[]="abcdefghijklmnopqrstuvwxyz";
    char digit[]="0123456789";
    
    srand((int)time(NULL));
    //sinh 4 chu cai:
    for(i=0;i<4;i++){
        t=rand()%26;
        code[j]=alpha[t];
        j++;
    }
    //srand((int)time(0));
    //sinh 3 chu so:
    for(i=0;i<3;i++){
        t=rand()%10;
        code[j]=digit[t];
        j++;
    }
    code[j]='\0';
}

void get_curtime(char *output){
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    sprintf(output, "%d-%d-%d %d:%d:%d",timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
}

int str_split(char *str, char *pat, char **out){
    char * p;
    int i=0;
    //Tách chuỗi con lần đầu tiên
    p = strtok(str, pat);
    strcpy(out[0], p);
    i++;
    //Tách chuỗi con từ lần thứ 2 trở đi
    //Bằng cách sử dụng hàm strok cho tới khi kết quả NULL được trả về.
    while(p != NULL) {
        //Chỉ dịnh đối số NULL trong hàm strtok để tiếp tục tách chuỗi ban đầu
        p = strtok(NULL, pat);       
        if(p != NULL) {
            strcpy(out[i], p);
            i++;
        }
    }
    return i; //so chuoi tach duoc
}

int excute_query(MYSQL *conn, char *sql){
	if (mysql_query(conn, sql)){
		printf("%s\n", mysql_error(conn));
		return 0;
	}
	return 1;
}

int check_role(MYSQL *conn, char *groupID, char *userID){
	char sql[BUFF_SIZE];
	MYSQL_RES *res;
	MYSQL_ROW row;

	sprintf(sql, "select role from group_member where groupID=%s and userID=%s;", groupID, userID);
	if(excute_query(conn, sql) == 0) return 0;	
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res); //row[0]: role cua member trong group
	mysql_free_result(res);

	if(strcmp(row[0], "1") == 0) return 1; // admin
	else if(strcmp(row[0], "2") == 0) return 2; //member
	return 0;	
}

int check_gname_exist(MYSQL *conn, char *gname){
	char sql[BUFF_SIZE];
	MYSQL_RES *res;
	int ret;

	sprintf(sql, "select *from fgroup where gname='%s';", gname);
	if(excute_query(conn, sql) == 0) return 0;
	res=mysql_store_result(conn);
	if(mysql_num_rows(res) == 0) ret=0;
	else ret=1; //da co trong db
	mysql_free_result(res);
	
	return ret;
}

int check_gcode_exist(MYSQL *conn, char *gcode){
	char sql[BUFF_SIZE];
	MYSQL_RES *res;
	int ret;

	sprintf(sql, "select *from fgroup where gcode='%s';", gcode);
	if(excute_query(conn, sql) == 0) return 0;
	res=mysql_store_result(conn);
	if(mysql_num_rows(res) == 0) ret=0;
	else ret=1; //da co trong db
	mysql_free_result(res);
	
	return ret;
}

int check_mem_exist(MYSQL *conn, char *memID, char *groupID){
	MYSQL_RES *res;
	char sql[BUFF_SIZE];
	int ret;

	sprintf(sql, "select *from group_member where groupID=%s and userID=%s;", groupID, memID);
	if(excute_query(conn, sql) == 0) return 0;
	res=mysql_store_result(conn);
	if(mysql_num_rows(res) == 0) ret=0;
	else ret=1;
	mysql_free_result(res);
	return ret;
}

int check_folname_exist(MYSQL *conn, char *folname, char *pfolderID){
	MYSQL_RES *res;
	char sql[BUFF_SIZE];
	int ret;

	sprintf(sql, "select fd.* from folder fd, parent_folder pfd where fd.folderID=pfd.folderID and pfolderID=%s and fol_name='%s';", pfolderID, folname);
	if(excute_query(conn, sql) == 0) return 0;
	res=mysql_store_result(conn);
	if(mysql_num_rows(res) == 0) ret=0;
	else ret=1;
	return ret;
}

int check_uname_exist(MYSQL *conn, char *uname){ //return 0 neu chua co
	char sql[BUFF_SIZE];
	MYSQL_RES *res;
	int ret;

	sprintf(sql, "select *from user where userID='%s';", uname);
	if(excute_query(conn, sql) == 0) return 0;
	res=mysql_store_result(conn);
	if(mysql_num_rows(res) == 0) ret=0;
	else ret=1; //da co trong db
	mysql_free_result(res);
	
	return ret;
}

int check_userID_exist(MYSQL *conn, char *userID){ //return 0 neu chua co
	char sql[BUFF_SIZE];
	MYSQL_RES *res;
	int ret;

	sprintf(sql, "select *from user where userID=%s;", userID);
	if(excute_query(conn, sql) == 0) return 0;
	res=mysql_store_result(conn);
	if(mysql_num_rows(res) == 0) ret=0;
	else ret=1; //da co trong db
	mysql_free_result(res);
	
	return ret;
}