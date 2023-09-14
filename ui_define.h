#include <gtk/gtk.h>
#include <gmodule.h>

#ifndef LOGIN_CASE_H
#define LOGIN_CASE_H
typedef struct Window {
	GtkApplication *app;
	
	GtkWidget *window, *window1;
	GtkWidget *grid;
	GtkWidget *box;
	GtkWidget *buttons[10];
	
	GtkWidget *userID;
	GtkWidget *u_name;
	GtkWidget *pass, *re_pass;
	GtkWidget *name;
	GtkWidget *email;
	GtkWidget *phone;
	GtkWidget *gr_name;
	GtkWidget *desc;
	GtkWidget *infor;
	GtkWidget *folname; //ten folder
	GtkWidget *fpath;

	void *go2, *back; //function(~giao dien) se chuyen toi
	char memID[4];
	char grpn[30], grpID[4]; 
	char folderID[4], fileID[4], fname[30], folname_str[30];
	int msg_code;
	
	int nfolder;// so luong folder
	char ifolder[10]; //infor folder
} Window;

void activate (GtkApplication* app, Window *win);
void check_login (GtkWidget *button,Window *win);

void signup(GtkWidget *button, Window *win);
void check_signup (GtkWidget *button,Window *win);

void HomePage(GtkWidget *button,Window *win);

void CreateGroup(GtkWidget *button,Window *win);
void check_gname(GtkWidget *button,Window *win);

void JoinGroup(GtkWidget *button,Window *win);
void check_join_group(GtkWidget *button,Window *win);

void show_groups(GtkWidget *button,Window *win);
void back2home(GtkWidget *button, Window *win);

void Logout(GtkWidget *button,Window *win);

void add_member(GtkWidget *button, Window *win);
void check_add_mem(GtkWidget *button,Window *win);
void back2crt_grp(GtkWidget *button, Window *win);

void view_group(GtkWidget *button,Window *win);
void back2show_grps(GtkWidget *button,Window *win);

void show_members(GtkWidget *button,Window *win);
void back2view_grp(GtkWidget *button, Window *win);
void delete_mem(GtkWidget *button, Window *win);

void Folder(GtkWidget *button,Window *win1);
void view_folder(GtkWidget *button,Window *win);
void back2pre_view(GtkWidget *button,Window *win);
void delete_folder(GtkWidget *button,Window *win);
void CreateFolder(GtkWidget *button,Window *win);
void check_crt_fol(GtkWidget *button,Window *win);

void File(GtkWidget *button,Window *win);
void upload_file(GtkWidget *button,Window *win);
void check_upld_file(GtkWidget *button,Window *win);
void download_file(GtkWidget *button,Window *win);
void delete_file(GtkWidget *button,Window *win);

#endif