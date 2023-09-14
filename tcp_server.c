#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h> 
#include <dirent.h>

#include <sys/stat.h> //mkdir()

#include "define.h"
#include "functions.h"

#define PORT 5550
#define BACKLOG 20
#define BUFF_SIZE 1024

void sig_chld(int signo); /* Handler process signal*/

int sendData(int s, void *buf, int size, int flags);
int recvData(int s, void *buf, int size, int flags);

void process(int sockfd, MYSQL *conn); // implement main program

int psignup(MYSQL *conn, int sockfd, message msg);
int plogin(int sockfd, MYSQL *conn, message msg);
int pcreate_group(MYSQL *conn,int sockfd, message msg);
int pjoin_group(MYSQL *conn,int sockfd, message msg);
int pshow_groups(MYSQL *conn,int sockfd);
int pview_group(MYSQL *conn,int sockfd, char *gname); //msg chua thong tin xd nhom
int pview_folder(MYSQL *conn,int sockfd, char *gID, char *folderID);

int pshow_folders(MYSQL *conn, int sockfd, char *folID);
int pshow_files(MYSQL *conn, int sockfd, char *folID);
int pshow_members(MYSQL *conn, int sockfd, char *gID);
int padd_member(MYSQL *conn, int sockfd, message msg, char *groupID);
int pdelete_member(MYSQL *conn, int sockfd, char *memID, char *groupID);
int pcreate_folder(MYSQL *conn, int sockfd, message msg, char *pfolderID);
int pdown_file(MYSQL *conn, int sockfd, message msg, char *folID);
int pup_file(MYSQL *conn, int sockfd, message msg, char *folID);
int pdelete_file(MYSQL *conn, int sockfd, message msg,char *gID, char *folID);
int pdelete_fol(MYSQL *conn, int sockfd,message msg, char *gID);
int remove_directory(const char *path) ;

char uID[3];
char root_path[]="./file_groups/";

int main(int argc, char **argv){
	
	int listen_sock, conn_sock; /* file descriptors */
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	pid_t pid;
	int sin_size;
	MYSQL *connect;
	
	char *server_name = "localhost";
	char *user = "root";
	char *password = "ngoc123"; 
	char *database = "FILE_SHARING";
	
	connect = mysql_init(NULL);
	
	/* Connect to database */
	if (!mysql_real_connect(connect, server_name, user, password, 
                                      database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(connect));
		exit(1);
	}
	else printf("Success to connect to db!\n");

	/* calls socket() */
	if ((listen_sock=socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  
		printf("socket() error\n");
		return 0;
	}
	
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   

	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ 
		perror("\nError: ");
		return 0;
	}     

	if(listen(listen_sock, BACKLOG) == -1){  
		perror("\nError: ");
		return 0;
	}
	
	/* Establish a signal handler to catch SIGCHLD */
	signal(SIGCHLD, sig_chld);

	while(1){
		sin_size=sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, (socklen_t*)&sin_size))==-1){
			if (errno == EINTR)
				continue;
			else{
				perror("\nError: ");			
				return 0;
			}
		}
		
		/* For each client, fork spawns a child, and the child handles the new client */
		pid = fork();
		
		/* fork() is called in child process */
		if(pid  == 0){
			close(listen_sock);
			printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */
			process(conn_sock, connect);	
			close(conn_sock);	
			//printf("closed\n");			
			exit(0);
		}
		
		/* The parent closes the connected socket since the child handles the new client */
		close(conn_sock);
	}
	//close mysql connection
	mysql_close(connect);
	//
	close(listen_sock);
	return 0;
}

void sig_chld(int signo){
	pid_t pid;
	int stat;
	
	/* Wait the child process terminate */
	while((pid = waitpid(-1, &stat, WNOHANG))>0) //doc tat ca tien trinh con xem co thay doi trang thai k?
		printf("\nChild %d terminated\n",pid);
}

int sendData(int s, void *buf, int size, int flags){
	int n;
	n = send(s, buf, size, flags);
	if(n < 0){
		perror("Error: ");
		//close(s);
	}
	return n;
}

int recvData(int s, void *buf, int size, int flags){
	int n;
	n = recv(s, buf, size, flags);
	if(n < 0)
		perror("Error: ");
	else if (n == 0)
		printf("Connection closed.\n");
	//if(n<=0) close(s);
	return n;
}

void process(int sockfd, MYSQL *conn){
	int ret;
	message msg0, msg;
	int bytes_recv;
	fd_set readfds;	
	FD_ZERO(&readfds); 
	FD_SET(sockfd, &readfds);

	while(1){
	bytes_recv = recvData(sockfd, &msg0, sizeof(message), 0);
	if (bytes_recv <= 0) return;
	if(msg0.code==SIGNUP){
		psignup(conn, sockfd, msg0);
	}

	if(msg0.code==LOGIN){
	ret=plogin(sockfd, conn, msg0);
	if(ret==1){ //login thanh cong
	do{
		select(sockfd + 1, &readfds, NULL, NULL, NULL);
		if (FD_ISSET(sockfd, &readfds))	{
			bytes_recv = recvData(sockfd, &msg, sizeof(message), 0);
			if (bytes_recv <= 0) return;
			switch (msg.code){
				case SHW_GRPS:
					pshow_groups(conn, sockfd);
					break;
				case CRT_GRP:
					pcreate_group(conn, sockfd, msg);
					break;
				case JOIN_GRP:
					pjoin_group(conn, sockfd, msg);
					break;
			}
		}
	}while(msg.code!=LOGOUT);
	}
	}
	}
}

int psignup(MYSQL *conn, int sockfd, message msg){
	//msg: chua cac thong tin dang ky (uname, passwd, name, phone, email)
	char sql[BUFF_SIZE];
	message msg1;

	if(check_uname_exist(conn, msg.payld[0]) != 0){
		strcpy(msg1.payld[0], "Username already exists!");
	}
	else{
	//them ban ghi vao table user:
		sprintf(sql, "insert into user(uname,passwd,name,phone,email) values ('%s','%s','%s','%s','%s');",msg.payld[0], msg.payld[1], msg.payld[2], msg.payld[3], msg.payld[4]);
		if(excute_query(conn, sql) == 0) return 0;

		//phan hoi cho client:
		strcpy(msg1.payld[0], "Signup success!");
	}
	if(sendData(sockfd, &msg1, sizeof(msg1), 0) < 0) return 0;
	return 1;
}

int plogin(int sockfd, MYSQL *conn, message msg) {
	char query[BUFF_SIZE];
	message msg_send;
	MYSQL_RES *res;
	MYSQL_ROW row;	
	
	sprintf(query, "select uname,passwd,userID from user where uname='%s';", msg.payld[0]);
	if(excute_query(conn, query) == 0) return 0;	
	res = mysql_store_result(conn);	//luu ket qua truy van
		
	if(mysql_num_rows(res) > 0){ //if username exist
		row = mysql_fetch_row(res);
		strcpy(uID,row[2]); //lay ID cua nguoi dung		
		//check password
		if(strcmp(msg.payld[1],row[1])==0){		
			msg_send.code = SUCC;					
		}		
		else {
			msg_send.code=ERR;
			strcpy(msg_send.payld[0], "Password is incorrect!");
		}
		}else {
			msg_send.code=ERR;
			strcpy(msg_send.payld[0], "Account does't exist!");
		}
	if(send(sockfd, &msg_send, sizeof(msg_send), 0) < 0) return 0;
	mysql_free_result(res);
	if(msg_send.code==ERR) return 0;
	return 1;
}

int pcreate_group(MYSQL *conn, int sockfd, message msg){
	//msg chua thong tin: group name (msg.payld[0]), description (msg.payld[1])
	char sql[BUFF_SIZE], path[200], gcode[8], cur_time[30];
	message msg1;
	int ret;
	MYSQL_RES *res;
	MYSQL_ROW row;

	// check group name co bi trung lap?
	ret=check_gname_exist(conn, msg.payld[0]);
	if(ret == 1){ // group name da ton tai
		msg1.code=ERR;
		strcpy(msg1.payld[0], "This group name already exists!");	
		if (sendData(sockfd, &msg1, sizeof(msg1), 0) < 0) return 0;
		return 0;	
	}
	else if (ret == 0){
		sprintf(path, "%s%s", root_path, msg.payld[0]);
		mkdir(path, 0777);	   // tao folder moi
		get_curtime(cur_time); // lay thoi gian hien tai
		// tao folder moi trong db:
		sprintf(sql, "insert into folder(fol_name,fol_path,crtTime,ownerID) values ('%s','%s','%s',%s);", msg.payld[0], path, cur_time, uID);
		if (excute_query(conn, sql) == 0)
		return 0;

		strcpy(sql, "select max(folderID) from folder;");
		if (excute_query(conn, sql) == 0) return 0;
		res = mysql_store_result(conn);
		row = mysql_fetch_row(res); // row[0]: folderID moi them
		do{
			gen_gcode(gcode); // sinh ngau nhien group code
			// check group code moi tao co bi trung lap:
			ret=check_gcode_exist(conn, gcode);
		}while(ret==1);
		// tao group moi:
		sprintf(sql, "insert into fgroup(gname,detail,folderID,gcode,ownerID) values ('%s','%s',%s,'%s',%s);", msg.payld[0], msg.payld[1], row[0], gcode, uID);
		if (excute_query(conn, sql) == 0) return 0;

		strcpy(sql, "select max(groupID) from fgroup;");
		if (excute_query(conn, sql) == 0) return 0;
		res = mysql_store_result(conn);
		row = mysql_fetch_row(res); // row[0]: groupID moi them
		// dang ky admin cua nhom:
		sprintf(sql, "insert into group_member() values (%s,%s,1);", row[0], uID); // 1: role of admin
		if (excute_query(conn, sql) == 0) return 0;

		// gui lai phan hoi cho client:
		msg1.code=SUCC;
		strcpy(msg1.payld[0], "Create group successful!");
		if (sendData(sockfd, &msg1, sizeof(msg1), 0) < 0) return 0;

		// check xem client co thuc hien Add member:
		if (recvData(sockfd, &msg1, sizeof(message), 0) <= 0) return 0;
		if (msg1.code == ADD_MEM){
			sprintf(sql, "select max(groupID) from fgroup;");
			if (excute_query(conn, sql) == 0) return 0;
			res = mysql_store_result(conn);
			row = mysql_fetch_row(res); // row[0]:groupID moi tao

			padd_member(conn, sockfd, msg1, row[0]);
		}
	}
	mysql_free_result(res);
	return 1;
}

int pjoin_group(MYSQL *conn,int sockfd, message msg){
	//msg chua thong tin group code
	MYSQL_RES *res, *res2;
	MYSQL_ROW row;
	message msg1;
	char sql[BUFF_SIZE], gID[5];

	sprintf(sql, "select groupID from fgroup where gcode='%s';", msg.payld[0]);
	if(excute_query(conn, sql) == 0) return 0;
	res=mysql_store_result(conn);
	if(mysql_num_rows(res) > 0){ //tim thay groupID tuong ung
		row=mysql_fetch_row(res); //row[0]: groupID
		strcpy(gID, row[0]);
		//check da la thanh vien nhom?
		sprintf(sql, "select *from group_member where groupID=%s and userID=%s;", gID, uID);
		if(excute_query(conn, sql) == 0) return 0;
		res2 = mysql_store_result(conn);
		if (mysql_num_rows(res2) == 0){ //->chua duoc dang ky trong table group_member		
			// insert vao table group_member:
			sprintf(sql, "insert into group_member() values (%s,%s,2);", row[0], uID);
			if(excute_query(conn, sql) == 0) return 0;
			strcpy(msg1.payld[0], "Join group successful!");
		}
		else strcpy(msg1.payld[0],"You joined this group!");
	}
	else {
		strcpy(msg1.payld[0], "Group code is incorrect!");
	}
	if(sendData(sockfd, &msg1, sizeof(msg1), 0) < 0) return 0;
	
	mysql_free_result(res);
	return 1;
}

int pshow_groups(MYSQL *conn,int sockfd){
	MYSQL_RES *res;
	MYSQL_ROW row;
	message msg;
	char sql[BUFF_SIZE];
	int i, nrow;

	sprintf(sql, "select g.groupID, gname from fgroup g, group_member gm where g.groupID=gm.groupID and userID=%s;", uID);
	if(excute_query(conn, sql) == 0) return 0;
	res=mysql_store_result(conn);
	nrow=mysql_num_rows(res);
	//thong bao ve so luong nhom cho client:
	if(sendData(sockfd, &nrow, sizeof(int), 0) < 0) return 0;
	//gui ket qua:
	for(i=0;i<nrow;i++){
		row=mysql_fetch_row(res);
		strcpy(msg.payld[0], row[0]);
		strcpy(msg.payld[1], row[1]);	
		if(sendData(sockfd, &msg, sizeof(msg), 0) < 0) return 0;
	}
	mysql_free_result(res);
	//nhan thong tin ve lua chon tiep theo:
	if(recvData(sockfd, &msg, sizeof(message), 0) <= 0) return 0;
	if(msg.code==BACK) return 1;
	if(msg.code==VIEW_GRP) pview_group(conn, sockfd, msg.payld[0]);
	return 1;
}//okk

int pview_group(MYSQL *conn,int sockfd, char *gname){
	//msg chua group name
	MYSQL_RES *res;
	MYSQL_ROW row;
	message msg1;
	char sql[BUFF_SIZE], gfolID[5], gID[5];
	
	//tim folderID cua nhom:
	sprintf(sql, "select folderID, groupID from fgroup where gname='%s';", gname);
	if(excute_query(conn, sql) == 0) return 0; // loi truy van
	res=mysql_store_result(conn);
	row=mysql_fetch_row(res); //row[0]: folderID cua nhom
	strcpy(gfolID, row[0]);
	strcpy(gID, row[1]);
	//xu ly hien thi cac folder va file trong nhom:
	pshow_folders(conn, sockfd, gfolID);
	pshow_files(conn, sockfd, gfolID);
	mysql_free_result(res);
	//nhan thong tin ve lua chon tiep theo cua client:
	do{
		if (recvData(sockfd, &msg1, sizeof(message), 0) <= 0)
			return 0;
		switch (msg1.code){
		case SHOW_MEM:
			pshow_members(conn, sockfd, gID);
			break;
		case CRT_FOL:
			pcreate_folder(conn, sockfd, msg1, gfolID);
			break;
		case VIEW_FOL:
			//view folder
			//pshow_folders(conn, sockfd, msg1.payld[0]);
			//pshow_files(conn, sockfd, msg1.payld[0]);
			pview_folder(conn, sockfd, gID, msg1.payld[0]); 
			break;
		case VIEW_GRP:
			if(pview_group(conn, sockfd, msg1.payld[0]) == 1)
				return 1;
			break;
		case UP_FILE:
			pup_file(conn, sockfd, msg1, gfolID);
			break;
		case DOWN_FILE:
			pdown_file(conn,sockfd,msg1, gfolID);
			break;
		case DEL_FILE:
			pdelete_file(conn,sockfd,msg1, gID,gfolID);
			break;
		case DEL_FOL:
			pdelete_fol(conn,sockfd,msg1, gID);
			break;
		}
	}while(msg1.code!=BACK);
	return 1;
}

int pview_folder(MYSQL *conn,int sockfd, char *gID, char *folderID){
	//msg chua group name
	message msg1;
	//char sql[BUFF_SIZE], gfolID[5], gID[5];
	
	//xu ly hien thi cac folder va file trong 1 folder:
	pshow_folders(conn, sockfd, folderID);
	pshow_files(conn, sockfd, folderID);
	//nhan thong tin ve lua chon tiep theo cua client:
	do{
		if (recvData(sockfd, &msg1, sizeof(message), 0) <= 0)
			return 0;
		switch (msg1.code){
		/*
		case SHOW_MEM:
			pshow_members(conn, sockfd, gID);
			break;
			*/
		case CRT_FOL:
			pcreate_folder(conn, sockfd, msg1, folderID);
			break;
		case VIEW_FOL:
			//view folder
			//pshow_folders(conn, sockfd, msg1.payld[0]);
			//pshow_files(conn, sockfd, msg1.payld[0]);
			pview_folder(conn, sockfd, gID, msg1.payld[0]);
			break;
		case VIEW_GRP:
			pview_group(conn, sockfd, msg1.payld[0]);
			break;
		case UP_FILE:
			pup_file(conn, sockfd, msg1, folderID);
			break;
		case DOWN_FILE:
			pdown_file(conn,sockfd,msg1, folderID);
			break;
		case DEL_FILE:
			pdelete_file(conn,sockfd,msg1, gID,folderID);
			break;
		case DEL_FOL:
			pdelete_fol(conn,sockfd,msg1, gID);
			break;
		}
	}while(msg1.code!=BACK);
	return 1;
}

int pshow_folders(MYSQL *conn, int sockfd, char *folID){
	MYSQL_RES *res;
	MYSQL_ROW row;
	char sql[BUFF_SIZE];
	int nrow, i;
	message msg;

	sprintf(sql, "select fol_name, fd.folderID from folder fd, parent_folder pfd where fd.folderID=pfd.folderID and pfolderID=%s;", folID);
	if(excute_query(conn, sql) == 0) return 0;
	res=mysql_store_result(conn);
	nrow=mysql_num_rows(res);
	//gui thong tin ve so luong folder:
	if(sendData(sockfd, &nrow, sizeof(int), 0) < 0) return 0;
	//gui ket qua ve client:
	for(i=0;i<nrow;i++){
		row=mysql_fetch_row(res); //row[0]: folder name
		strcpy(msg.payld[0], row[0]);
		strcpy(msg.payld[1], row[1]);
		if(sendData(sockfd, &msg, sizeof(msg), 0) < 0) return 0;
	}	
	mysql_free_result(res);
	return 1;
}

int pshow_files(MYSQL *conn, int sockfd, char *folID){
	MYSQL_RES *res;
	MYSQL_ROW row;
	char sql[BUFF_SIZE];
	int nrow, i;
	message msg;

	sprintf(sql, "select fname, fileID from file where folderID=%s;", folID);
	if(excute_query(conn, sql) == 0) return 0; // loi truy van
	res=mysql_store_result(conn);
	nrow=mysql_num_rows(res);
	//gui thong tin ve so luong file:
	if(sendData(sockfd, &nrow, sizeof(int), 0) < 0) return 0;
	//gui ket qua ve client:
	for(i=0;i<nrow;i++){
		row=mysql_fetch_row(res); //row[0]: file name
		strcpy(msg.payld[0], row[0]);
		strcpy(msg.payld[1], row[1]);
		if(sendData(sockfd, &msg, sizeof(msg), 0) < 0) return 0;
	}		
	mysql_free_result(res);
	return 1;
}

int pshow_members(MYSQL *conn, int sockfd, char *gID){
	MYSQL_RES *res;
	MYSQL_ROW row;
	message msg;
	char sql[BUFF_SIZE];
	int nrow, i, role;

	sprintf(sql, "select u.userID, name, phone, email from user u, group_member gm where u.userID=gm.userID and role=2 and groupID=%s;", gID);
	printf("%s\n", sql);
	if(excute_query(conn, sql) == 0) return 0;
	res=mysql_store_result(conn);
	nrow=mysql_num_rows(res);

	role=check_role(conn, gID, uID);
	//gui thong tin ve role:
	if(sendData(sockfd, &role, sizeof(int), 0) < 0) return 0;
	//gui thong tin ve so luong member:
	if(sendData(sockfd, &nrow, sizeof(int), 0) < 0) return 0;
	//gui ket qua ve client:
	for(i=0;i<nrow;i++){
		row=mysql_fetch_row(res);
		strcpy(msg.payld[0], row[0]); //userID
		strcpy(msg.payld[1], row[1]); //name
		strcpy(msg.payld[2], row[2]); //phone
		strcpy(msg.payld[3], row[3]); //email
		if(sendData(sockfd, &msg, sizeof(msg), 0) < 0) return 0;
	}
	//nhan thong tin ve lua chon tiep theo cua client:
	do{
		if (recvData(sockfd, &msg, sizeof(message), 0) <= 0) return 0;
		switch (msg.code){
			case ADD_MEM:
				padd_member(conn, sockfd, msg, gID);
				break;
			case DEL_MEM:
				pdelete_member(conn, sockfd, msg.payld[0], gID);
				break;
			case SHOW_MEM:
				if(pshow_members(conn, sockfd, msg.payld[0]) == 1)
					return 1;
				break;
		}
	} while(msg.code != BACK);
	mysql_free_result(res);
	return 1;
}

int padd_member(MYSQL *conn, int sockfd, message msg, char *groupID){
	//msg: chua userID muon them vao nhom
	char **id, sql[BUFF_SIZE], tmp[30];
	int i, k=0, cnt;
	int nrow=10, ncol=4;	
	message msg1;

	//check quyen admin:
	if(check_role(conn, groupID, uID)!=1){
		strcpy(msg1.payld[0], "You are not admin!");
	}
	else{
	//cap phat bo nho dong:
	id=(char**)malloc(nrow*sizeof(char*));
	for (i = 0; i < nrow; i++){
		id[i] = (char *)malloc(ncol * sizeof(char));
	}
	cnt = str_split(msg.payld[0], ",", id); //so luong ID nhan duoc

	strcpy(sql,"insert into group_member() values ");
	for(i=0;i<cnt;i++){
		//check 1 user da la thanh vien nhom?
		if(check_mem_exist(conn, id[i], groupID) == 1) continue;
		if(check_userID_exist(conn, id[i]) == 0){
			sprintf(msg1.payld[0], "UserID %s does not exist!", id[i]);
			k=-1;
			break;
		}
		sprintf(tmp,"(%s,%s,2)", groupID, id[i]);
		if(i!=cnt-1) strcat(tmp, ",");
		strcat(sql, tmp);
		k++;
	}
	strcat(sql, ";");
	if(k>0){
		if(excute_query(conn, sql) == 0) return 0;	
		strcpy(msg1.payld[0], "Add member successful!");
	}
	else if(k==0) strcpy(msg1.payld[0], "No member added!");
	//giai phong bo nho da cap phat:
	for (i = 0; i < nrow; i++){
    	free(id[i]);
	}
	free(id);
	}
	//phan hoi cho client:
	if(sendData(sockfd, &msg1, sizeof(msg1), 0) < 0) return 0;
	return 1;
}

int pdelete_member(MYSQL *conn, int sockfd, char *memID, char *groupID){
	//msg: chua userID muon xoa khoi nhom
	char sql[BUFF_SIZE];
	message msg;

	if(check_role(conn, groupID, uID)!=1){
		strcpy(msg.payld[0], "You are not admin!");
	}
	else{
		sprintf(sql,"delete from group_member where groupID=%s and userID=%s;", groupID, memID);
		if(excute_query(conn, sql) == 0) return 0;
		strcpy(msg.payld[0], "Delete successful!");
	}	
	//phan hoi cho client:
	if(sendData(sockfd, &msg, sizeof(msg), 0) < 0) return 0;
	return 1;
}

int pcreate_folder(MYSQL *conn, int sockfd, message msg, char *pfolderID){
	//msg.payld[0]: folder name se tao moi
	MYSQL_RES *res;
	MYSQL_ROW row;
	char sql[BUFF_SIZE], fol_path[200], cur_time[30];
	//, buf[100];	
	message msg1;

	//check folder name co bi trung lap:
	if(check_folname_exist(conn, msg.payld[0], pfolderID) == 1){
		strcpy(msg1.payld[0], "This folder name already exists!");
	}
	else{
		// tim duong dan thu muc cha cua thu muc se tao:
		sprintf(sql, "select fol_path from folder where folderID=%s;", pfolderID);
		if (excute_query(conn, sql) == 0)
		return 0;
		res = mysql_store_result(conn);
		row = mysql_fetch_row(res);
		sprintf(fol_path, "%s/%s", row[0], msg.payld[0]);

		mkdir(fol_path, 0777); // tao folder moi
		get_curtime(cur_time); // lay thoi gian hien tai

		// them ban ghi vao table folder:
		sprintf(sql, "insert into folder(fol_name, fol_path, crtTime, ownerID) values ('%s','%s','%s',%s);", msg.payld[0], fol_path, cur_time, uID);
		if (excute_query(conn, sql) == 0) return 0;

		strcpy(sql, "select max(folderID) from folder;");
		if (excute_query(conn, sql) == 0) return 0;
		res = mysql_store_result(conn);
		row = mysql_fetch_row(res); // row[0]: ID cua folder moi tao

		// them ban ghi vao table parent_folder:
		sprintf(sql, "insert into parent_folder() values (%s,%s);", row[0], pfolderID);
		if (excute_query(conn, sql) == 0) return 0;
		strcpy(msg1.payld[0], "Create folder successful!");
	}
	//phan hoi cho client:
	if(sendData(sockfd, &msg1, sizeof(msg1), 0) < 0) return 0;
	return 1;
}

int pdown_file(MYSQL *conn, int sockfd, message msg, char *folID){
	MYSQL_RES *res;
	MYSQL_ROW row;
	FILE *f;
	char sql[BUFF_SIZE];
	char path_file[200];	
	sprintf(sql, "select fpath from file where folderID=%s and fname='%s' ;", folID,msg.payld[0]);
	if (excute_query(conn, sql) == 0)
	return 0;
	res = mysql_store_result(conn);
	row = mysql_fetch_row(res);
	//mo file

	//gui file
	strcpy(path_file,row[0]);
	printf("%s",path_file);
	f=fopen(path_file,"rb");
	long filelen;
    fseek(f, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(f);             // Get the current byte offset in the file       
        		rewind(f);    // pointer to start of file
        	 sendData(sockfd, &filelen, 20, 0); //sendData file size
        		
        		int sum = 0; //count size byte sendData
        		printf("\n OK, start transfer!\n");
        		while(1) {
        			int byteNum = BUFF_SIZE;
        		    if((sum + BUFF_SIZE) > filelen) {// if over file size
        		    	byteNum = filelen - sum; 
        		    }
        		    char* buffer = (char *) malloc((byteNum) * sizeof(char));
        		    fread(buffer, byteNum, 1, f); // read buffer with size 
        		    sum += byteNum; //increase byte sendData
        		    sendData(sockfd, buffer, byteNum, 0);
        		    
        		    free(buffer);
        		    if(sum >= filelen) {
        		    	break;
        		    }
        		}

        		printf("\n File Transfer Is Completed...\n");

        		fclose(f);

	return 1;

}

int pup_file(MYSQL *conn, int sockfd, message msg, char *folID){
	printf("fname:%s\n", msg.payld[0]);
	MYSQL_RES *res;
	MYSQL_ROW row;
	FILE *f2;
	int  bytes_received;
	char sql[BUFF_SIZE], path_file[200],cur_time[30];
	char *fileContent ;
	long filelen;	
	sprintf(sql, "select fol_path from folder where folderID=%s ;", folID);
	if (excute_query(conn, sql) == 0)
	return 0;
	res = mysql_store_result(conn);
	row = mysql_fetch_row(res);

	strcpy(path_file,row[0]);
	strcat(path_file,"/");
	get_curtime(cur_time);
	strcat(msg.payld[0],"_");
	strcat(msg.payld[0],cur_time);
	strcat(path_file,msg.payld[0]);
	printf("%s\n",path_file);
	sprintf(sql, "insert into file(fname, folderID, fpath, crtTime) values ('%s',%s,'%s','%s')",msg.payld[0],folID,path_file,cur_time);
	if (excute_query(conn, sql) == 0)
	return 0;

	bytes_received = recv(sockfd, &filelen, 20, 0);
			
			int sumByte = 0;
			
			//f2=fopen(file_name,"wb");
			f2=fopen(path_file,"wb");
			
			fileContent = (char*) malloc(BUFF_SIZE * sizeof(char));
			while(1) {
						bytes_received = recv(sockfd, fileContent, BUFF_SIZE, 0);
						if(bytes_received == 0) {
							printf("Error: Down File that bai \n");
							break;
						}
						sumByte += bytes_received;
						fwrite(fileContent, bytes_received, 1, f2);
						free(fileContent);
						fileContent = (char*) malloc(BUFF_SIZE * sizeof(char));
						if(sumByte >= filelen) {
							break;
						}
					}  // file content
		
			fclose(f2);
			free(fileContent);
			printf("\n File Transfer Is Completed...\n");


	return 1;
}

int pdelete_file(MYSQL *conn, int sockfd,message msg, char *gID, char *folID){
	//msg: chua userID muon them vao nhom
	MYSQL_RES *res;
	MYSQL_ROW row;
	char buf[50],sql[BUFF_SIZE],path_file[100];

		

	//check quyen admin:
	if(check_role(conn, gID, uID)!=1){
		strcpy(buf, "You are not admin!");
	}
	else{
		//remove.....
		sprintf(sql,"select fpath from file where fname='%s' && folderID = %s",msg.payld[0],folID);
		if (excute_query(conn, sql) == 0)
		return 0;
		res = mysql_store_result(conn);
		row = mysql_fetch_row(res);
		strcpy(path_file,row[0]);
		if (remove(path_file) == 0) {
        printf("The file is deleted successfully!");
    	} else {
        printf("The file is not deleted!");
    }

		strcpy(buf,"Deleted successfully");
		//xoa file khoi db
		sprintf(sql,"delete from file where fpath='%s';",path_file);
		if(excute_query(conn, sql) == 0) return 0;	
	}
	//phan hoi cho client:
	if(sendData(sockfd, buf, strlen(buf), 0) < 0) return 0;

	return 1;
}

int pdelete_fol(MYSQL *conn, int sockfd,message msg, char *gID){
	MYSQL_RES *res;
	MYSQL_ROW row;
	char buf[50],sql[BUFF_SIZE],path_fol[100];

		

	//check quyen admin:
	if(check_role(conn, gID, uID)!=1){
		strcpy(buf, "You are not admin!");
	}
	else{
		//remove.....
		sprintf(sql,"select fol_path from folder where fol_name='%s'",msg.payld[0]);
		if (excute_query(conn, sql) == 0)
		return 0;
		res = mysql_store_result(conn);
		row = mysql_fetch_row(res);
		strcpy(path_fol,row[0]);
		remove_directory(path_fol);

		strcpy(buf,"Deleted successfully!");
		//xoa file khoi db
		sprintf(sql,"delete from folder where fol_path='%s';",path_fol);
		if(excute_query(conn, sql) == 0) return 0;	
	}
	//phan hoi cho client:
	if(sendData(sockfd, buf, strlen(buf), 0) < 0) return 0;

	return 1;
}

int remove_directory(const char *path) {
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (d) {
      struct dirent *p;

      r = 0;
      while (!r && (p=readdir(d))) {
          int r2 = -1;
          char *buf;
          size_t len;

          /* Skip the names "." and ".." as we don't want to recurse on them. */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
             continue;

          len = path_len + strlen(p->d_name) + 2; 
          buf = malloc(len);

          if (buf) {
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);
             if (!stat(buf, &statbuf)) {
                if (S_ISDIR(statbuf.st_mode))
                   r2 = remove_directory(buf);
                else
                   r2 = unlink(buf);
             }
             free(buf);
          }
          r = r2;
      }
      closedir(d);
   }

   if (!r)
      r = rmdir(path);

   return r;
}

