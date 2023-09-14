#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "define.h"
#include "ui_define.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5550
#define BUFF_SIZE 1024
#define PATH "./Downloads/"

int client_sock;
void *route, *route_b, *route_view;

int sendData(int s, void *buf, int size, int flags);
int recvData(int s, void *buf, int size, int flags);
void get_fname(char *fpath, char *fname);

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

void get_fname(char *fpath, char *fname){
	int i;
	//, j, k=0;
	for(i=strlen(fpath)-1;i>=0;i--){
		if(fpath[i]=='/') break;
	}
	strncpy(fname,fpath+i+1,strlen(fpath));
	/*for(j=i+1;j<strlen(fpath);j++){
		fname[k]=fpath[j];
		k++;
	}*/
}

Window *fwin;

int main (int argc, char **argv){
    int status;
	fwin = malloc(sizeof(Window));
	//char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
		
	//Step 4: Communicate with server

  fwin->app = gtk_application_new ("com.gtk.myapp_1", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (fwin->app, "activate", G_CALLBACK (activate), fwin);
  status = g_application_run (G_APPLICATION (fwin->app), argc, argv);
  g_object_unref (fwin->app);
  free(fwin);

  
printf("Program closed!\n");
close(client_sock);
  return status;
}

void activate (GtkApplication* app, Window *win)
{
  GtkWidget *button,*button_1;
  GtkWidget *label_user;
  GtkWidget *label_pass;
  Window *win1;
  win1 = win;
  
  //create window
  win1->window = gtk_application_window_new (win1->app);
  gtk_window_set_title (GTK_WINDOW (win1->window), "Log In");
  gtk_window_set_default_size (GTK_WINDOW (win1->window), 320, 200);

  //create grid
  win1->grid = gtk_grid_new ();
  gtk_window_set_child (GTK_WINDOW (win1->window), win1->grid);
  
  //label
  label_user = gtk_label_new("Username  ");
  gtk_grid_attach(GTK_GRID(win1->grid), label_user, 0, 1, 1, 1);
  label_pass = gtk_label_new("Password  ");
  gtk_grid_attach(GTK_GRID(win1->grid), label_pass, 0, 2, 1, 1);
  
  //insert field
  win1->u_name = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(win1->u_name), "Username");
  gtk_grid_attach(GTK_GRID(win1->grid), win1->u_name, 1, 1, 1, 1);

  win1->pass = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(win1->pass), "Password");
  gtk_entry_set_visibility(GTK_ENTRY(win1->pass), 0);//hide password
  gtk_grid_attach(GTK_GRID(win1->grid), win1->pass, 1, 2, 1, 1);
  
  //button login
  button = gtk_button_new_with_label ("\nLog In\n");
  gtk_grid_attach(GTK_GRID(win1->grid), button, 1, 3, 1, 1);
  //g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), win1->window);
  g_signal_connect (button, "clicked", G_CALLBACK (check_login), win1);//G_CALLBACK (Login) goi ham login
  
  //button signup
  button_1 = gtk_button_new_with_label ("Sign Up");
  gtk_grid_attach(GTK_GRID(win1->grid), button_1, 1, 6, 1, 1);
  g_signal_connect_swapped(button_1, "clicked", G_CALLBACK(gtk_window_destroy), win1->window);
  g_signal_connect (button_1, "clicked", G_CALLBACK (signup), win1);
  
  gtk_widget_show (win1->window);
}

void check_login (GtkWidget *button, Window *win)
{
	Window *win1;
  	win1 = win;
  	
  	GtkWidget *label;
  	GtkWidget *grid;
  	GtkWidget *subwindow;
	message msg;

  	char username[30], password[30];
  	strcpy(username, gtk_editable_get_text(GTK_EDITABLE(win1->u_name)));
    //gtk_editable_get_text co kieu du lieu (char *)
  	
  	strcpy(password, gtk_editable_get_text(GTK_EDITABLE(win1->pass)));
  	//process:
	msg.code=LOGIN;
	strcpy(msg.payld[0],username);
	strcpy(msg.payld[1],password);
	if(send(client_sock, &msg, sizeof(msg), 0) < 0) return;

	if(recv(client_sock, &msg, sizeof(message), 0) <= 0) return;
	
	//dang nhap thanh cong:
  	if(msg.code == SUCC) {
    	gtk_window_destroy(GTK_WINDOW(win1->window));
    	HomePage(button, win1);
	}
   	else {
	//new window
		//gtk_widget_hide(win1->window);
		gtk_init ();
    	subwindow = gtk_window_new();
    	gtk_window_set_title (GTK_WINDOW (subwindow), "Login Error");
    
    	grid = gtk_grid_new ();
   		gtk_window_set_child (GTK_WINDOW (subwindow), grid);
    
    	label = gtk_label_new(msg.payld[0]);
    	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    
    	button = gtk_button_new_with_label ("Retry");
    	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
    	//g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_show), win1->window);
    	gtk_grid_attach(GTK_GRID(grid), button, 1, 3, 1, 1);
    
    	gtk_widget_show (subwindow);
	}
return;
}

void signup (GtkWidget *b, Window *win)
{
  GtkWidget *button,*button_1;
  GtkWidget *label_1,*label_2,*label_3,*label_4,*label_5,*label_6,*re_pass;
  Window *win1;
  win1 = win;
  
  //create window
  win1->window = gtk_application_window_new (win1->app);
  gtk_window_set_title (GTK_WINDOW (win1->window), "Sign Up");
  gtk_window_set_default_size (GTK_WINDOW (win1->window), 640, 400);

  //create grid
  win1->grid = gtk_grid_new ();
  gtk_window_set_child (GTK_WINDOW (win1->window), win1->grid);
  
  //label
  label_1 = gtk_label_new("Username:\n");
  gtk_grid_attach(GTK_GRID(win1->grid), label_1, 0, 1, 1, 1);
  label_2 = gtk_label_new("Password:\n");
  gtk_grid_attach(GTK_GRID(win1->grid), label_2, 0, 2, 1, 1);
  label_3 = gtk_label_new("Re-Password:\n");
  gtk_grid_attach(GTK_GRID(win1->grid), label_3, 0, 3, 1, 1);
  label_4 = gtk_label_new("Name:\n");
  gtk_grid_attach(GTK_GRID(win1->grid), label_4, 0, 4, 1, 1);
  label_5 = gtk_label_new("Email:\n");
  gtk_grid_attach(GTK_GRID(win1->grid), label_5, 0, 5, 1, 1);
  label_6 = gtk_label_new("Phone:\n");
  gtk_grid_attach(GTK_GRID(win1->grid), label_6, 0, 6, 1, 1);
  
  //insert field
  win1->u_name = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(win1->u_name), "Username");
  gtk_grid_attach(GTK_GRID(win1->grid), win1->u_name, 1, 1, 1, 1);

  win1->pass = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(win1->pass), "Password");
  gtk_entry_set_visibility(GTK_ENTRY(win1->pass), 0);//hide password
  gtk_grid_attach(GTK_GRID(win1->grid), win1->pass, 1, 2, 1, 1);
  
  win1->re_pass = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(win1->re_pass), "Re-Password");
  gtk_entry_set_visibility(GTK_ENTRY(win1->re_pass), 0);//hide password
  gtk_grid_attach(GTK_GRID(win1->grid), win1->re_pass, 1, 3, 1, 1);
  
  win1->name = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(win1->name), "Name");
  gtk_grid_attach(GTK_GRID(win1->grid), win1->name, 1, 4, 1, 1);
  
  win1->email = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(win1->email), "Email");
  gtk_grid_attach(GTK_GRID(win1->grid), win1->email, 1, 5, 1, 1);
  
  win1->phone = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(win1->phone), "Phone");
  gtk_grid_attach(GTK_GRID(win1->grid), win1->phone, 1, 6, 1, 1);
  
  //button signup
  button = gtk_button_new_with_label ("\nSign Up\n");
  gtk_grid_attach(GTK_GRID(win1->grid), button, 1, 7, 1, 1);
  g_signal_connect (button, "clicked", G_CALLBACK (check_signup), win1);
  
  //button login
  button_1 = gtk_button_new_with_label ("Log In");
  gtk_grid_attach(GTK_GRID(win1->grid), button_1, 1, 8, 1, 1);
  g_signal_connect_swapped(button_1, "clicked", G_CALLBACK(gtk_window_destroy), win1->window);
  g_signal_connect (button_1, "clicked", G_CALLBACK (activate), win1);
  
  gtk_widget_show (win1->window);
}

void check_signup (GtkWidget *button,Window *win)
{
  	GtkWidget *subwindow, *grid, *label, *button1;
	char re_pass[60];
	message msg;

	strcpy(msg.payld[0], gtk_editable_get_text(GTK_EDITABLE(win->u_name)));
	strcpy(msg.payld[1], gtk_editable_get_text(GTK_EDITABLE(win->pass)));
	strcpy(re_pass, gtk_editable_get_text(GTK_EDITABLE(win->re_pass)));
	strcpy(msg.payld[2], gtk_editable_get_text(GTK_EDITABLE(win->name)));
	strcpy(msg.payld[3], gtk_editable_get_text(GTK_EDITABLE(win->phone)));
	strcpy(msg.payld[4], gtk_editable_get_text(GTK_EDITABLE(win->email)));

	if (strcmp(msg.payld[1], re_pass) == 0){
		msg.code = SIGNUP;
		if (sendData(client_sock, &msg, sizeof(msg), 0) < 0)
			return;
		if (recvData(client_sock, &msg, sizeof(message), 0) <= 0)
			return;
	}
	else{
		strcpy(msg.payld[0], "Re-password failed!");
	}
	gtk_init();
	subwindow = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(subwindow), "Result");

	grid = gtk_grid_new();
	gtk_window_set_child(GTK_WINDOW(subwindow), grid);

	label = gtk_label_new(msg.payld[0]);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

	button1 = gtk_button_new_with_label("OK");
	gtk_grid_attach(GTK_GRID(grid), button1, 1, 3, 1, 1);
	g_signal_connect_swapped(button1, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	g_signal_connect_swapped(button1, "clicked", G_CALLBACK(gtk_window_destroy), win->window);
	g_signal_connect(button1, "clicked", G_CALLBACK(activate), win);
	
	gtk_widget_show(subwindow);
}

void HomePage(GtkWidget *button, Window *win)
{
	Window *win1;
  	win1 = win;
  	
	//1_Tao nhom 2_Tham gia nhom 3_Nhom da tham gia 4_Nhom cua ban
  	GtkWidget *button_1, *button_2, *button_3, *button_4, *label;
	GtkWidget *subwindow;

	subwindow = gtk_application_window_new (win1->app);
  	gtk_window_set_title (GTK_WINDOW (subwindow), "Home");
  	gtk_window_set_default_size (GTK_WINDOW (subwindow), 640, 400);
  	
  	win1->grid = gtk_grid_new ();
  	gtk_window_set_child (GTK_WINDOW (subwindow), win1->grid);
  
  	//label
  	label = gtk_label_new("  Hello!\n\t\t\t\t\t\t\t");
  	gtk_grid_attach(GTK_GRID(win1->grid), label, 1, 0, 1, 1);
  	
  	//button
  	button_1 = gtk_button_new_with_label("\t\t\t\nCreate Group\n\n");
  	gtk_grid_attach(GTK_GRID(win1->grid), button_1, 4, 1, 1, 1);
  	g_signal_connect_swapped(button_1, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
  	g_signal_connect(button_1, "clicked", G_CALLBACK(CreateGroup), win1);
  	
  	button_2 = gtk_button_new_with_label("\t\t\t\nJoin a Group\n\n");
  	gtk_grid_attach(GTK_GRID(win1->grid), button_2, 4, 2, 1, 1);
  	g_signal_connect_swapped(button_2, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
  	g_signal_connect(button_2, "clicked", G_CALLBACK(JoinGroup), win1);
  	
  	button_3 = gtk_button_new_with_label("\t\t\t\nView All Group\n\n");
  	gtk_grid_attach(GTK_GRID(win1->grid), button_3, 4, 3, 1, 1);
  	g_signal_connect_swapped(button_3, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
  	g_signal_connect(button_3, "clicked", G_CALLBACK(show_groups), win1);
  	
  	button_4 = gtk_button_new_with_label("\t\t\t\nLog Out\n\n");
  	gtk_grid_attach(GTK_GRID(win1->grid), button_4, 4, 4, 1, 1);
  	g_signal_connect_swapped(button_4, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
  	g_signal_connect(button_4, "clicked", G_CALLBACK(Logout), win1);
  	
  	gtk_widget_show (subwindow);
}

void CreateGroup(GtkWidget *button, Window *win)
{
	Window *win1;
    win1 = win;
    
	GtkWidget *subgrid;
	GtkWidget *subwindow;
	GtkWidget *label, *label_1,*label_2,*button_1;
	//
	//Cua so nhap ten nhom, mo ta nhom
	//
	subwindow = gtk_application_window_new (win1->app);
    gtk_window_set_title (GTK_WINDOW (subwindow), "Create Group");
    gtk_window_set_default_size (GTK_WINDOW (subwindow), 640, 400);
    
	subgrid = gtk_grid_new ();
  	gtk_window_set_child (GTK_WINDOW (subwindow), subgrid);
  	
  	//label
  	label = gtk_label_new("Create Group!");
  	gtk_grid_attach(GTK_GRID(subgrid), label, 0, 0, 1, 1);
    label_1 = gtk_label_new("\nGroup Name:    ");
  	gtk_grid_attach(GTK_GRID(subgrid), label_1, 0, 2, 1, 1);
  	label_2 = gtk_label_new("\nDescription:    ");
  	gtk_grid_attach(GTK_GRID(subgrid), label_2, 0, 4, 1, 1);
  
 	//insert field
  	win1->gr_name = gtk_entry_new();
  	gtk_entry_set_placeholder_text(GTK_ENTRY(win1->gr_name), "Group Name\n");
  	gtk_grid_attach(GTK_GRID(subgrid), win1->gr_name, 1, 3, 1, 1);

  	win1->desc = gtk_entry_new();
  	gtk_entry_set_placeholder_text(GTK_ENTRY(win1->desc), "Description...\n\n\n");
 	gtk_grid_attach(GTK_GRID(subgrid), win1->desc, 1, 5, 1, 1);
 	
 	//button
	button = gtk_button_new_with_label ("Cancel");
	gtk_grid_attach(GTK_GRID(subgrid), button, 0, 8, 1, 1);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
    //g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_show), subwindow);
    g_signal_connect(button, "clicked", G_CALLBACK(HomePage), win1);
    
    button_1 = gtk_button_new_with_label ("Next");
 	gtk_grid_attach(GTK_GRID(subgrid), button_1, 1, 8, 1, 1);
 	//g_signal_connect_swapped(button_1, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
 	g_signal_connect(button_1, "clicked", G_CALLBACK(check_gname), win1);
    
	win1->window=subwindow;
    gtk_widget_show (subwindow);
}

void check_gname(GtkWidget *button, Window *win){
	Window *win1;
    win1 = win;

	GtkWidget *label;
  	GtkWidget *grid;
  	GtkWidget *subwindow;
	int bytes_sent, bytes_recv;
	message msg;

	route=&HomePage;
	route_b=&back2crt_grp;
	printf("check gname\n");
  	char gname[30], desc[100];
  	strcpy(gname, gtk_editable_get_text(GTK_EDITABLE(win1->gr_name)));
  	strcpy(desc, gtk_editable_get_text(GTK_EDITABLE(win1->desc)));

  	//process:
	msg.code=CRT_GRP;
	strcpy(msg.payld[0], gname);
	strcpy(msg.payld[1], desc);
	bytes_sent = sendData(client_sock, &msg, sizeof(msg), 0);
	if(bytes_sent < 0) return;
	
	bytes_recv = recvData(client_sock, &msg, sizeof(message), 0);
	if (bytes_recv <= 0) return;
	//if success:
  	if(msg.code == SUCC) {
    	gtk_window_destroy(GTK_WINDOW(win1->window)); //destroy crt grp
		//for add_member:
		add_member(button, win1);
	}
	//if group name is invalid
	else {
		//gtk_widget_hide(win1->window);
		gtk_init ();
    	subwindow = gtk_window_new();
    	gtk_window_set_title (GTK_WINDOW (subwindow), "Error");
    
    	grid = gtk_grid_new ();
   		gtk_window_set_child (GTK_WINDOW (subwindow), grid);
    
    	label = gtk_label_new(msg.payld[0]);
    	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    
    	button = gtk_button_new_with_label ("OK");
    	gtk_grid_attach(GTK_GRID(grid), button, 1, 3, 1, 1);
    	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
    	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), win1->window);
    	//g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_show), win1->window);
    	g_signal_connect(button, "clicked", G_CALLBACK(HomePage), win1);
    	
    	gtk_widget_show (subwindow);
	}
}

void JoinGroup(GtkWidget *button,Window *win)
{
	Window *win1;
    win1 = win;
    
	//gtk_widget_hide(win1->window);
	GtkWidget *subgrid,*subgrid_1;
	GtkWidget *subwindow;
	GtkWidget *label, *label_1, *button_1;
	
    subwindow = gtk_application_window_new (win1->app);
    gtk_window_set_title (GTK_WINDOW (subwindow), "Join a Group");
    gtk_window_set_default_size (GTK_WINDOW (subwindow), 640, 400);
    
	subgrid = gtk_grid_new ();
  	gtk_window_set_child (GTK_WINDOW (subwindow), subgrid);
  	
  	//label
  	label = gtk_label_new("Join a Group!");
  	gtk_grid_attach(GTK_GRID(subgrid), label, 0, 0, 1, 1);
    label_1 = gtk_label_new("\nEnter Group Code:    ");
  	gtk_grid_attach(GTK_GRID(subgrid), label_1, 0, 2, 1, 1);
  
 	//insert field
  	win1->infor = gtk_entry_new();
  	gtk_entry_set_placeholder_text(GTK_ENTRY(win1->infor), "Code...\n");
  	gtk_grid_attach(GTK_GRID(subgrid), win1->infor, 1, 3, 1, 1);
 	
 	//button
	button = gtk_button_new_with_label ("Cancel");
	gtk_grid_attach(GTK_GRID(subgrid), button, 0, 8, 1, 1);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
    g_signal_connect (button, "clicked", G_CALLBACK (HomePage), win1);
 	//g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_show), win1->window);
    
	//win1->window=subwindow;
    button_1 = gtk_button_new_with_label ("OK");
	g_signal_connect (button_1, "clicked", G_CALLBACK (check_join_group), win1);
 	gtk_grid_attach(GTK_GRID(subgrid), button_1, 1, 8, 1, 1);
    win1->window=subwindow;
	gtk_widget_show (subwindow);
}

void check_join_group(GtkWidget *button, Window *win){
	Window *win1;
  	win1 = win;
  	
  	GtkWidget *label;
  	GtkWidget *grid;
  	GtkWidget *subwindow;
	message msg;	
	char gcode[30], buf[100];
	
	//lay gia tri nhap vao tu giao dien:
  	strcpy(gcode, gtk_editable_get_text(GTK_EDITABLE(win1->infor)));

	msg.code = JOIN_GRP;
	strcpy(msg.payld[0], gcode);
	if(sendData(client_sock, &msg, sizeof(msg), 0) < 0) return;

	if(recvData(client_sock, &msg, sizeof(message), 0) <= 0) return;
	
	//hien thi cua so thong bao
	//gtk_widget_hide(win1->window);
	gtk_init();
	subwindow = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(subwindow), "Result");

	grid = gtk_grid_new();
	gtk_window_set_child(GTK_WINDOW(subwindow), grid);

	label = gtk_label_new(msg.payld[0]);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

	button = gtk_button_new_with_label("OK");
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), win1->window);
	g_signal_connect(button, "clicked", G_CALLBACK(HomePage), win1);
	gtk_grid_attach(GTK_GRID(grid), button, 1, 3, 1, 1);

	gtk_widget_show(subwindow);
}

void show_groups(GtkWidget *button,Window *win) //hien thi cac nhom tham gia
{
	//groups
	Window *wind[50], *win1;
	//wind=malloc(50*sizeof(Window));
    //win1 = win;    
	//gtk_widget_hide(win1->window);
	GtkWidget *subgrid;
	GtkWidget *subwindow;
	GtkWidget *button_1[50], *label;
	message msg;
	int i, j, n, x, y;
	
    subwindow = gtk_application_window_new (win->app);
    gtk_window_set_title (GTK_WINDOW (subwindow), "View All Group");
    gtk_window_set_default_size (GTK_WINDOW (subwindow), 640, 400);
    
	subgrid = gtk_grid_new ();
  	gtk_window_set_child (GTK_WINDOW (subwindow), subgrid);
  	
  	//label
  	label = gtk_label_new("All Group");
  	gtk_grid_attach(GTK_GRID(subgrid), label, 0, 0, 1, 1);
  	
	msg.code=SHW_GRPS;
	//gui yeu cau:
	if(sendData(client_sock, &msg, sizeof(msg), 0) < 0) return;
	//nhan thong tin ve so nhom:
	if(recvData(client_sock, &n, sizeof(int), 0) <= 0) return;

	//hien thi cac nhom:
	x=1;y=2;
	for(i=0;i<n;i++){
		wind[i]=malloc(sizeof(Window));
		memcpy(wind[i], win, sizeof(*win));
		if(i%5 == 0){
			x=1;
			y++;
		}
		if(recvData(client_sock, &msg, sizeof(message), 0) <= 0) return;	
		printf("%s.%s\n", msg.payld[0], msg.payld[1]);
		strcpy(wind[i]->grpID, msg.payld[0]); //gID
		strcpy(wind[i]->grpn, msg.payld[1]); //gname
		//hien thi:
		button_1[i] = gtk_button_new_with_label(msg.payld[1]);
		gtk_grid_attach(GTK_GRID(subgrid), button_1[i], x, y, 1, 1);
		g_signal_connect_swapped(button_1[i], "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
		g_signal_connect(button_1[i], "clicked", G_CALLBACK(view_group), wind[i]);
		x=x+2;
	}

	button = gtk_button_new_with_label ("Back");
	gtk_grid_attach(GTK_GRID(subgrid), button, 0, 1, 1, 1);	
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
    //g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_show), win->window);
    g_signal_connect(button, "clicked", G_CALLBACK(back2home), win);
    gtk_widget_show (subwindow);
}

void back2home(GtkWidget *button, Window *win){
	message msg;
	
	msg.code=BACK;
	if(sendData(client_sock, &msg, sizeof(message), 0) < 0) return;
	HomePage(button, win);
}

void Logout(GtkWidget *button,Window *win)
{
	message msg;
	
	msg.code=LOGOUT;
	if(sendData(client_sock, &msg, sizeof(message), 0) < 0) return;
	//gtk_window_destroy(GTK_WINDOW(win->window));
	activate(win->app, win);
}

void add_member(GtkWidget *button, Window *win)
{
	Window *win1;
    win1 = win;
    
	//gtk_widget_hide(win1->window);
	GtkWidget *subgrid,*subgrid_1;
	GtkWidget *subwindow, *subwindow_1;
	GtkWidget *label, *label_1,*label_2, *button_1;
	
    subwindow = gtk_application_window_new (win1->app);
    gtk_window_set_title (GTK_WINDOW (subwindow), "Add Member");
    gtk_window_set_default_size (GTK_WINDOW (subwindow), 640, 400);
    
	subgrid = gtk_grid_new ();
  	gtk_window_set_child (GTK_WINDOW (subwindow), subgrid);
  	
  	//label
    label = gtk_label_new("\nInput UserID to add:    ");
  	gtk_grid_attach(GTK_GRID(subgrid), label, 0, 2, 1, 1);
  
 	//insert field
  	win1->userID = gtk_entry_new();
  	gtk_entry_set_placeholder_text(GTK_ENTRY(win1->userID), "UserID\n");
  	gtk_grid_attach(GTK_GRID(subgrid), win1->userID, 1, 3, 1, 1);
 	
 	//button	
	button = gtk_button_new_with_label ("Back");
	gtk_grid_attach(GTK_GRID(subgrid), button, 0, 8, 1, 1);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
    g_signal_connect(button, "clicked", G_CALLBACK(route_b), win1);
    
	button_1 = gtk_button_new_with_label ("OK");
 	gtk_grid_attach(GTK_GRID(subgrid), button_1, 1, 8, 1, 1);
	//
	g_signal_connect(button_1, "clicked", G_CALLBACK(check_add_mem), win1);
	
	win1->window=subwindow;
	gtk_widget_show (subwindow);
}

void check_add_mem(GtkWidget *button,Window *win){
	//add_mem
	Window *win1;
  	win1 = win;
  	
  	GtkWidget *label;
  	GtkWidget *grid;
  	GtkWidget *subwindow;
	message msg;	
	char userID[30], buf[100];

	//lay gia tri nhap vao tu giao dien:
  	strcpy(userID, gtk_editable_get_text(GTK_EDITABLE(win1->userID)));
    
	if(strcmp(userID, "") == 0){ //in case: chua them thanh vien luc nay
		msg.code=SKIP;
		if(sendData(client_sock, &msg, sizeof(msg), 0) < 0) return;
	}
	else{
		msg.code = ADD_MEM;
		strcpy(msg.payld[0], userID);
		if(send(client_sock, &msg, sizeof(msg), 0) < 0) return;

		if(recv(client_sock, &msg, sizeof(message), 0) <= 0) return;

		//hien thi cua so thong bao
		//gtk_widget_hide(win1->window);
		gtk_init();
		subwindow = gtk_window_new();
		gtk_window_set_title(GTK_WINDOW(subwindow), "Result");

		grid = gtk_grid_new();
		gtk_window_set_child(GTK_WINDOW(subwindow), grid);

		label = gtk_label_new(msg.payld[0]);
		gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

		button = gtk_button_new_with_label("OK");
		g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
		g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), win1->window);
		g_signal_connect(button, "clicked", G_CALLBACK(route), win1);
		gtk_grid_attach(GTK_GRID(grid), button, 1, 3, 1, 1);

		gtk_widget_show(subwindow);
	}
}

void back2crt_grp(GtkWidget *button, Window *win){
	message msg;
	
	msg.code=BACK;
	if(sendData(client_sock, &msg, sizeof(message), 0) < 0) return;
	CreateGroup(button, win);
}

void view_group(GtkWidget *button,Window *win) //vao nhom
{
	route_view=&view_group;
	printf("view_grp, grp:%s\n", win->grpn);
	//into a group
	Window *wind[100];
    //win1 = win;
    //gtk_widget_hide(win1->window);
	GtkWidget *box;
	GtkWidget *subgrid;
	GtkWidget *subwindow;
	GtkWidget *label, *label_1;
	GtkWidget *button_1,*button_2,*button_3;//1_View member 2_Upload 3_Create Folder
	//GtkWidget *button1,*button2,*button3;//1File 2Folder
	message msg;

	// gui group name cho server:
	msg.code = VIEW_GRP;
	strcpy(msg.payld[0], win->grpn);
	if (sendData(client_sock, &msg, sizeof(msg), 0) < 0) return;

	subwindow = gtk_application_window_new (win->app);
    gtk_window_set_title (GTK_WINDOW (subwindow), win->grpn);
    gtk_window_set_default_size (GTK_WINDOW (subwindow), 640, 400);
    
    subgrid = gtk_grid_new ();
  	gtk_window_set_child (GTK_WINDOW (subwindow), subgrid);
    
	label = gtk_label_new("\n");
  	gtk_grid_attach(GTK_GRID(subgrid), label, 0, 1, 1, 1); 
	
	button_1 = gtk_button_new_with_label ("Show members");
	gtk_grid_attach(GTK_GRID(subgrid), button_1, 0, 3, 1, 1);	
	g_signal_connect_swapped(button_1, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	g_signal_connect(button_1, "clicked", G_CALLBACK(show_members), win);
	
	button_2 = gtk_button_new_with_label ("Upload file");
	gtk_grid_attach(GTK_GRID(subgrid), button_2, 0, 5, 1, 1);		
	//g_signal_connect_swapped(button_2, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	g_signal_connect(button_2, "clicked", G_CALLBACK(upload_file), win);
	
	button_3 = gtk_button_new_with_label ("Create folder");
	gtk_grid_attach(GTK_GRID(subgrid), button_3, 0, 7, 1, 1);	
	//g_signal_connect_swapped(button_3, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	g_signal_connect(button_3, "clicked", G_CALLBACK(CreateFolder), win);
	
	button = gtk_button_new_with_label ("Back");
	gtk_grid_attach(GTK_GRID(subgrid), button, 0, 9, 1, 1);	
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	g_signal_connect(button, "clicked", G_CALLBACK(back2show_grps), win); //back ViewGroup
	
	int i;
	//them border:
	for(i=3;i<=10;i++){
		gtk_grid_attach(GTK_GRID(subgrid), gtk_label_new("\t|\t"), 1, i, 1, 1);
	}
	int n, x,y,k;
	GtkWidget *button1[100]; //i
	x=2; y=1; 
	
	if(recvData(client_sock, &n, sizeof(int), 0) <= 0) return; //so folder co the hien thi
	printf("*Folders:\n");
	for(i=0;i<n;i++){
		wind[i]=malloc(sizeof(Window));
		memcpy(wind[i], win, sizeof(*win));
		
		if(recvData(client_sock, &msg, sizeof(message), 0) <= 0) return;
		//hien thi:
		if(i%5 == 0){
			x=2; y++;
		}
		button1[i] = gtk_button_new_with_label(msg.payld[0]);
		gtk_grid_attach(GTK_GRID(subgrid), button1[i], x, y, 1, 1);
		strcpy(wind[i]->folderID, msg.payld[1]);
		strcpy(wind[i]->folname_str, msg.payld[0]);
		//wind[i].msg_code = VIEW_FOL;
		g_signal_connect(button1[i], "clicked", G_CALLBACK(Folder), wind[i]);
		printf("   %s\n", msg.payld[0]);
		x=x+2;
	}	
	y++;
	//files:
	if(recvData(client_sock, &n, sizeof(int), 0) <= 0) return; //so folder co the hien thi
	printf("*Folders:\n");
	for(k=0;k<n;k++){
		if(recvData(client_sock, &msg, sizeof(message), 0) <= 0) return;
		//hien thi:
		if(k%5 == 0){
			x=2; y++;
		}
		i++;
		wind[i]=malloc(sizeof(Window));
		memcpy(wind[i], win, sizeof(*win));
		
		button1[i] = gtk_button_new_with_label(msg.payld[0]);
		gtk_grid_attach(GTK_GRID(subgrid), button1[i], x, y, 1, 1);
		strcpy(wind[i]->fname, msg.payld[0]);
		g_signal_connect(button1[i], "clicked", G_CALLBACK(File), wind[i]);
		printf("   %s\n", msg.payld[0]);
		x=x+2;
	}	
	
  	gtk_widget_show(subwindow);  	
}

void back2show_grps(GtkWidget *button,Window *win){
	message msg;
	msg.code = BACK;

	printf("back2shwgrp, %s\n", win->grpn);
	if (sendData(client_sock, &msg, sizeof(msg), 0) < 0) return;
	show_groups(button, win);
}

void show_members(GtkWidget *button,Window *win) 
{
	//members in a group
	Window *wind[50];
    
	GtkWidget *subgrid;
	GtkWidget *subwindow;
	GtkWidget *button_a, *button_1, *delete[50];
	message msg;
	int n,i,x,y, rowid, role;
	
	route=&show_members;
	route_b=&show_members;	

	printf("show_mems, group:%s\n", win->grpn);
	//gtk_widget_hide(win->window);
	//tao window
    subwindow = gtk_application_window_new (win->app);
    gtk_window_set_title (GTK_WINDOW (subwindow), "View Member");
    gtk_window_set_default_size (GTK_WINDOW (subwindow), 640, 400);
    
	subgrid = gtk_grid_new ();
  	gtk_window_set_child (GTK_WINDOW (subwindow), subgrid);
  	
	//label = gtk_label_new(msg.payld[0]);
	gtk_grid_attach(GTK_GRID(subgrid), gtk_label_new("UserID\t"), 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(subgrid), gtk_label_new("Name\t"), 1, 1, 2, 1);
	gtk_grid_attach(GTK_GRID(subgrid), gtk_label_new("Phone\t"), 3, 1, 2, 1);
	gtk_grid_attach(GTK_GRID(subgrid), gtk_label_new("Email\t"), 5, 1, 2, 1);
	
	msg.code=SHOW_MEM;
	strcpy(msg.payld[0], win->grpID); //win->grpID: groupID

	if(sendData(client_sock, &msg, sizeof(msg), 0) < 0) return;
	//nhan thong tin ve role:
	if(recvData(client_sock, &role, sizeof(int), 0) <= 0) return;
	//nhan thong tin ve so luong thanh vien:
	if(recvData(client_sock, &n, sizeof(int), 0) <= 0) return;
	printf("mem_num:%d\n", n);
	rowid=2;
	for(i=0;i<n;i++){
		wind[i]=malloc(sizeof(Window));
		memcpy(wind[i], win, sizeof(*win));

		if(recvData(client_sock, &msg, sizeof(message), 0) <= 0) return;
		//id, name, phone, email
		printf("%s, %s, %s, %s\n", msg.payld[0], msg.payld[1], msg.payld[2], msg.payld[3]);		
		
		gtk_grid_attach(GTK_GRID(subgrid), gtk_label_new(msg.payld[0]), 0, rowid, 1, 1);
		gtk_grid_attach(GTK_GRID(subgrid), gtk_label_new(msg.payld[1]), 1, rowid, 2, 1);
		gtk_grid_attach(GTK_GRID(subgrid), gtk_label_new(msg.payld[2]), 3, rowid, 2, 1);
		gtk_grid_attach(GTK_GRID(subgrid), gtk_label_new(msg.payld[3]), 6, rowid, 2, 1);

		if(role==1){
			strcpy(wind[i]->memID, msg.payld[0]);		
			delete[i] = gtk_button_new_with_label("Delete");
			gtk_grid_attach(GTK_GRID(subgrid), delete[i], 8, rowid, 1, 1);
			g_signal_connect(delete[i], "clicked", G_CALLBACK(delete_mem), wind[i]);
		}
		rowid++;
	}
	if(role==1){
  		button_a = gtk_button_new_with_label("Add Member");
	  	gtk_grid_attach(GTK_GRID(subgrid), button_a, 1, 0, 1, 1);
  		g_signal_connect_swapped(button_a, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
  		g_signal_connect(button_a, "clicked", G_CALLBACK(add_member), win);
	}
  	
	button = gtk_button_new_with_label ("Back");
	gtk_grid_attach(GTK_GRID(subgrid), button, 0, 0, 1, 1);
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
    //g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_show), win->window);
    g_signal_connect(button, "clicked", G_CALLBACK(back2view_grp), win); //back Group
    
	for(i=0; i<n; i++){
		wind[i]->window=subwindow;
	}
	win->window=subwindow;
    gtk_widget_show (subwindow);
}

void back2view_grp(GtkWidget *button, Window *win){
	message msg;	

	printf("back2vwgrp, group:%s\n", win->grpn);
	msg.code=BACK;
	if(sendData(client_sock, &msg, sizeof(message), 0) < 0) return;
	view_group(button, win);
}

void delete_mem(GtkWidget *button, Window *win)
{
	GtkWidget *subwindow, *grid, *label, *button1;
	message msg;

	printf("grpn:%s\n", win->grpn);
	msg.code = DEL_MEM;
	strcpy(msg.payld[0], win->memID);

	if(sendData(client_sock, &msg, sizeof(msg), 0) < 0) return;
	if(recvData(client_sock, &msg, sizeof(message), 0) <= 0) return;

	gtk_init();
	subwindow = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(subwindow), "Result");

	grid = gtk_grid_new();
	gtk_window_set_child(GTK_WINDOW(subwindow), grid);

	label = gtk_label_new(msg.payld[0]);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

	button1 = gtk_button_new_with_label("OK");
	gtk_grid_attach(GTK_GRID(grid), button1, 1, 3, 1, 1);
	g_signal_connect_swapped(button1, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	g_signal_connect_swapped(button1, "clicked", G_CALLBACK(gtk_window_destroy), win->window);
	g_signal_connect(button1, "clicked", G_CALLBACK(show_members), win);
	
	gtk_widget_show(subwindow);
}

void Folder(GtkWidget *button,Window *win)
{
	GtkWidget *window, *grid, *button_1;
	gtk_init();
	window = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(window), "Folder");
	//
	grid = gtk_grid_new();
	gtk_window_set_child(GTK_WINDOW(window), grid);

	button_1 = gtk_button_new_with_label("View Folder");
	gtk_grid_attach(GTK_GRID(grid), button_1, 1, 0, 1, 1);
	g_signal_connect_swapped(button_1, "clicked", G_CALLBACK(gtk_window_destroy), window);
	g_signal_connect(button_1, "clicked", G_CALLBACK(view_folder), win);

	button = gtk_button_new_with_label("Delete");
	gtk_grid_attach(GTK_GRID(grid), button, 1, 1, 1, 1);
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window);
	g_signal_connect(button, "clicked", G_CALLBACK(delete_folder), win);
	
	gtk_widget_show(window);
}

void view_folder(GtkWidget *button,Window *win)
{
	route_view=&view_folder;
	Window *win1, *wind[50];
    win1 = win;
    
    //gtk_widget_hide(win1->window);
	GtkWidget *box;
	GtkWidget *subgrid;
	GtkWidget *subwindow;
	GtkWidget *label, *label_1;
	GtkWidget *button_1,*button_2,*button_3;//1_View member 2_Upload 3_Create Folder
	//GtkWidget *button1,*button2,*button3;//1File 2Folder
	message msg;

	// gui group name cho server:
	msg.code = VIEW_FOL;
	strcpy(msg.payld[0], win->folderID);
	if (sendData(client_sock, &msg, sizeof(msg), 0) < 0) return;

	subwindow = gtk_application_window_new (win->app);
    gtk_window_set_title (GTK_WINDOW (subwindow), win->grpn);
    gtk_window_set_default_size (GTK_WINDOW (subwindow), 640, 400);
    
    subgrid = gtk_grid_new ();
  	gtk_window_set_child (GTK_WINDOW (subwindow), subgrid);
    
	label = gtk_label_new("\n");
  	gtk_grid_attach(GTK_GRID(subgrid), label, 0, 1, 1, 1); 
	
	/*button_1 = gtk_button_new_with_label ("Show members");
	gtk_grid_attach(GTK_GRID(subgrid), button_1, 0, 3, 1, 1);	
	g_signal_connect_swapped(button_1, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	g_signal_connect(button_1, "clicked", G_CALLBACK(show_members), win);
	*/
	button_2 = gtk_button_new_with_label ("Upload file");
	gtk_grid_attach(GTK_GRID(subgrid), button_2, 0, 5, 1, 1);		
	//g_signal_connect_swapped(button_2, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	g_signal_connect(button_2, "clicked", G_CALLBACK(upload_file), win);
	
	button_3 = gtk_button_new_with_label ("Create folder");
	gtk_grid_attach(GTK_GRID(subgrid), button_3, 0, 7, 1, 1);	
	//g_signal_connect_swapped(button_3, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	g_signal_connect(button_3, "clicked", G_CALLBACK(CreateFolder), win);
	
	button = gtk_button_new_with_label ("Back");
	gtk_grid_attach(GTK_GRID(subgrid), button, 0, 9, 1, 1);	
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	g_signal_connect(button, "clicked", G_CALLBACK(back2pre_view), win); //back ViewGroup
	
	int i;
	//them border:
	for(i=3;i<=10;i++){
		gtk_grid_attach(GTK_GRID(subgrid), gtk_label_new("\t|\t"), 1, i, 1, 1);
	}
	int n, x,y,k;
	GtkWidget *button1[100]; //i
	x=2; y=1; 
	
	if(recvData(client_sock, &n, sizeof(int), 0) <= 0) return; //so folder co the hien thi
	printf("*Folders:\n");
	for(i=0;i<n;i++){
		wind[i]=malloc(sizeof(Window));
		memcpy(wind[i], win, sizeof(*win));
		
		if(recvData(client_sock, &msg, sizeof(message), 0) <= 0) return;
		//hien thi:
		if(i%5 == 0){
			x=2; y++;
		}
		button1[i] = gtk_button_new_with_label(msg.payld[0]);
		gtk_grid_attach(GTK_GRID(subgrid), button1[i], x, y, 1, 1);
		strcpy(wind[i]->folderID, msg.payld[1]);
		strcpy(wind[i]->folname_str, msg.payld[0]);
		//wind[i].msg_code = VIEW_FOL;
		g_signal_connect(button1[i], "clicked", G_CALLBACK(Folder), wind[i]);
		printf("   %s\n", msg.payld[0]);
		x=x+2;
	}	
	y++;
	//files:
	if(recvData(client_sock, &n, sizeof(int), 0) <= 0) return; //so folder co the hien thi
	printf("*Folders:\n");
	for(k=0;k<n;k++){
		if(recvData(client_sock, &msg, sizeof(message), 0) <= 0) return;
		//hien thi:
		if(k%5 == 0){
			x=2; y++;
		}
		i++;
		wind[i]=malloc(sizeof(Window));
		memcpy(wind[i], win, sizeof(*win));
		
		button1[i] = gtk_button_new_with_label(msg.payld[0]);
		gtk_grid_attach(GTK_GRID(subgrid), button1[i], x, y, 1, 1);
		strcpy(wind[i]->fname, msg.payld[0]);
		g_signal_connect(button1[i], "clicked", G_CALLBACK(File), wind[i]);
		printf("   %s\n", msg.payld[0]);
		x=x+2;
	}	
	
  	gtk_widget_show(subwindow);  	
}

void back2pre_view(GtkWidget *button,Window *win){
	message msg;

	msg.code=BACK;
	if(sendData(client_sock, &msg, sizeof(msg), 0) < 0) return;
}

void delete_folder(GtkWidget *button,Window *win){
	GtkWidget *subwindow, *grid, *label, *button1;
	message msg;
	char buf[50];
	int bytes_recv;

	msg.code=DEL_FOL;
	strcpy(msg.payld[0], win->folname_str);
	if(sendData(client_sock, &msg, sizeof(msg), 0) < 0) return;

	bytes_recv= recvData(client_sock, buf, sizeof(buf), 0);
	if(bytes_recv <= 0) return;
	buf[bytes_recv]='\0';
	
	gtk_init();
	subwindow = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(subwindow), "Result");

	grid = gtk_grid_new();
	gtk_window_set_child(GTK_WINDOW(subwindow), grid);

	label = gtk_label_new(buf);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

	button1 = gtk_button_new_with_label("OK");
	gtk_grid_attach(GTK_GRID(grid), button1, 1, 3, 1, 1);
	g_signal_connect_swapped(button1, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	//g_signal_connect_swapped(button1, "clicked", G_CALLBACK(gtk_window_destroy), win->window);
	//g_signal_connect(button1, "clicked", G_CALLBACK(back2view_grp), win);
	
	gtk_widget_show(subwindow);
}

void CreateFolder(GtkWidget *button,Window *win)
{
	//view a group
	Window *win1;
    win1 = win;
    
	GtkWidget *subgrid,*subgrid_1;
	GtkWidget *subwindow,*subwindow_1;
	GtkWidget *label, *label_1,*label_2, *button_1,*button_2;
	
    subwindow = gtk_application_window_new (win1->app);
    gtk_window_set_title (GTK_WINDOW (subwindow), "Create Folder");
    gtk_window_set_default_size (GTK_WINDOW (subwindow), 640, 400);
    
	subgrid = gtk_grid_new ();
  	gtk_window_set_child (GTK_WINDOW (subwindow), subgrid);
  	
  	//label
  	label = gtk_label_new("Create Folder!");
  	gtk_grid_attach(GTK_GRID(subgrid), label, 0, 0, 1, 1);
    label_1 = gtk_label_new("\nEnter Folder Name:    ");
  	gtk_grid_attach(GTK_GRID(subgrid), label_1, 0, 2, 1, 1);
  
 	//insert field
  	win1->folname = gtk_entry_new();
  	gtk_grid_attach(GTK_GRID(subgrid), win1->folname, 1, 3, 1, 1);
 	
 	//button
	button = gtk_button_new_with_label ("Cancel");
	gtk_grid_attach(GTK_GRID(subgrid), button, 0, 8, 1, 1);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
    //g_signal_connect(button, "clicked", G_CALLBACK(back2view_grp), win1); //back Group
    
    button_1 = gtk_button_new_with_label ("Create");
 	gtk_grid_attach(GTK_GRID(subgrid), button_1, 1, 8, 1, 1);
	//g_signal_connect_swapped(button_1, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
    g_signal_connect(button_1, "clicked", G_CALLBACK(check_crt_fol), win1);
    
	win1->window=subwindow;
    gtk_widget_show (subwindow);
}

void check_crt_fol(GtkWidget *button,Window *win){
	//view a group
	Window *win1;
  	win1 = win;
  	
  	GtkWidget *label;
  	GtkWidget *grid;
  	GtkWidget *subwindow;
	message msg;	
	char folname[30], buf[100];
	int brecv;

	//lay gia tri nhap vao tu giao dien:
	strcpy(folname, gtk_editable_get_text(GTK_EDITABLE(win1->folname)));

	msg.code = CRT_FOL;
	strcpy(msg.payld[0], folname);
	if (send(client_sock, &msg, sizeof(msg), 0) < 0)
		return;

	if (brecv=recv(client_sock, &msg, sizeof(message), 0) <= 0)
		return;
	
	// hien thi cua so thong bao
	// gtk_widget_hide(win1->window);
	gtk_init();
	subwindow = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(subwindow), "Result");

	grid = gtk_grid_new();
	gtk_window_set_child(GTK_WINDOW(subwindow), grid);

	label = gtk_label_new(msg.payld[0]);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

	button = gtk_button_new_with_label("OK");
	gtk_grid_attach(GTK_GRID(grid), button, 1, 3, 1, 1);
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), win1->window);
	//g_signal_connect(button, "clicked", G_CALLBACK(route_view), win1);
	
	gtk_widget_show(subwindow);
}

void File(GtkWidget *button,Window *win)
{
	GtkWidget *window, *grid; 
	GtkWidget *button1, *button2;

	gtk_init();
	window = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(window), "Options");
	//
	grid = gtk_grid_new();
	gtk_window_set_child(GTK_WINDOW(window), grid);

	button1 = gtk_button_new_with_label("Download");
	gtk_grid_attach(GTK_GRID(grid), button1, 0, 1, 1, 1);
	g_signal_connect_swapped(button1, "clicked", G_CALLBACK(gtk_window_destroy), window);
	g_signal_connect(button1, "clicked", G_CALLBACK(download_file), win);
		
	button2 = gtk_button_new_with_label("Delete");
	gtk_grid_attach(GTK_GRID(grid), button2, 0, 2, 1, 1);
	g_signal_connect_swapped(button2, "clicked", G_CALLBACK(gtk_window_destroy), window);
	g_signal_connect(button2, "clicked", G_CALLBACK(delete_file), win);
	
	gtk_widget_show(window);
}

void upload_file(GtkWidget *button,Window *win)
{
	GtkWidget *subgrid;
	GtkWidget *subwindow;
	GtkWidget *label, *label_1, *gr_name, *button_1;
	
    subwindow = gtk_application_window_new (win->app);
    gtk_window_set_title (GTK_WINDOW (subwindow), "Upload File");
    gtk_window_set_default_size (GTK_WINDOW (subwindow), 640, 400);
    
	subgrid = gtk_grid_new ();
  	gtk_window_set_child (GTK_WINDOW (subwindow), subgrid);
  	
  	//label
  	label = gtk_label_new("Upload File");
  	gtk_grid_attach(GTK_GRID(subgrid), label, 0, 0, 1, 1);
    label_1 = gtk_label_new("\nEnter File Path:    ");
  	gtk_grid_attach(GTK_GRID(subgrid), label_1, 0, 2, 1, 1);
  
 	//insert field
  	win->fpath = gtk_entry_new();
  	gtk_grid_attach(GTK_GRID(subgrid), win->fpath, 1, 3, 1, 1);
 	
 	//button
	button = gtk_button_new_with_label ("Cancel");
	gtk_grid_attach(GTK_GRID(subgrid), button, 0, 8, 1, 1);
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
    //g_signal_connect(button, "clicked", G_CALLBACK(back2view_grp), win); //back Group
    
    button_1 = gtk_button_new_with_label ("Upload");
 	gtk_grid_attach(GTK_GRID(subgrid), button_1, 1, 8, 1, 1);
    //g_signal_connect_swapped(button_1, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
    g_signal_connect(button_1, "clicked", G_CALLBACK(check_upld_file), win); //back Group
    
	win->window=subwindow;
    gtk_widget_show (subwindow);
}

void check_upld_file(GtkWidget *button,Window *win){
  	GtkWidget *label;
  	GtkWidget *grid;
  	GtkWidget *subwindow;
	message msg;	
	char fpath[100], fname[30], buf[100];
	FILE *f1;

	//lay gia tri nhap vao tu giao dien:
	strcpy(fpath, gtk_editable_get_text(GTK_EDITABLE(win->fpath)));
	printf("path:'%s'\n", fpath);
	
	//gui file
	f1=fopen(fpath,"rb");
	if(f1!=NULL){ 
		get_fname(fpath,fname);
		msg.code=UP_FILE;
		strcpy(msg.payld[0], fname);
		if(sendData(client_sock, &msg, sizeof(msg), 0) < 0) return;

	long filelen;
    fseek(f1, 0, SEEK_END);          // Jump to the end of the file
    filelen = ftell(f1);             // Get the current byte offset in the file       
    rewind(f1);    // pointer to start of file
    sendData(client_sock, &filelen, 20, 0); //send file size
    
    int sum = 0;
    while(1) {
        int byteNum = BUFF_SIZE;
        if((sum + BUFF_SIZE) > filelen) {// if over file size
            byteNum = filelen - sum; 
   	    }
        char* buffer = (char *) malloc((byteNum) * sizeof(char));
        fread(buffer, byteNum, 1, f1); // read buffer with size 
        sum += byteNum; //increase byte send
         sendData(client_sock, buffer, byteNum, 0);
        
            free(buffer);
            if(sum >= filelen) {
                break;
            }
   	}
   	fclose(f1);
	strcpy(buf, "File Transfer Is Completed!");
	}
	else strcpy(buf, "Can't open this file!");
	 
	// hien thi cua so thong bao
	// gtk_widget_hide(win->window);
	gtk_init();
	subwindow = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(subwindow), "Result");

	grid = gtk_grid_new();
	gtk_window_set_child(GTK_WINDOW(subwindow), grid);

	label = gtk_label_new(buf);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

	button = gtk_button_new_with_label("OK");
	gtk_grid_attach(GTK_GRID(grid), button, 1, 3, 1, 1);
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), win->window);
	//g_signal_connect(button, "clicked", G_CALLBACK(route_view), win);
	
	gtk_widget_show(subwindow);
}

void download_file(GtkWidget *button,Window *win){
	GtkWidget *subwindow, *grid, *label, *button1;
	message msg;
	char path_file[100];
	FILE *f2;
	long filelen;
	int bytes_received;
	char * fileContent;

	strcpy(path_file,PATH);
	
	msg.code=DOWN_FILE;
	strcpy(msg.payld[0], win->fname);
	if(sendData(client_sock, &msg, sizeof(msg), 0) < 0) return;

	strcat(path_file, win->fname);
	// printf("%s",path_file);
	recvData(client_sock, &filelen, 20, 0);

	int sumByte = 0;

	f2 = fopen(path_file, "wb");

	fileContent = (char *)malloc(BUFF_SIZE * sizeof(char));
	while (1){
		bytes_received = recvData(client_sock, fileContent, BUFF_SIZE, 0);
		if (bytes_received == 0){
			printf("ko nhan dc gi\n");
			break;
		}
		sumByte += bytes_received;
		fwrite(fileContent, bytes_received, 1, f2);
		free(fileContent);
		fileContent = (char *)malloc(BUFF_SIZE * sizeof(char));
		if (sumByte >= filelen)	{
			break;
		}
	} // file content
	fclose(f2);
	free(fileContent);
	//hien thi ket qua:
	gtk_init();
	subwindow = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(subwindow), "Result");

	grid = gtk_grid_new();
	gtk_window_set_child(GTK_WINDOW(subwindow), grid);

	label = gtk_label_new("File Transfer Is Completed!");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

	button1 = gtk_button_new_with_label("OK");
	gtk_grid_attach(GTK_GRID(grid), button1, 1, 3, 1, 1);
	g_signal_connect_swapped(button1, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	//g_signal_connect_swapped(button1, "clicked", G_CALLBACK(gtk_window_destroy), win->window);
	//g_signal_connect(button1, "clicked", G_CALLBACK(show_members), win);
	
	gtk_widget_show(subwindow);
}

void delete_file(GtkWidget *button,Window *win){
	GtkWidget *subwindow, *grid, *label, *button1;
	message msg;
	char buf[50];
	int bytes_recv;

	msg.code=DEL_FILE;
	strcpy(msg.payld[0], win->fname);
	if(sendData(client_sock, &msg, sizeof(msg), 0) < 0) return;

	bytes_recv= recvData(client_sock, buf, sizeof(buf), 0);
	if(bytes_recv <= 0) return;
	buf[bytes_recv]='\0';
	
	gtk_init();
	subwindow = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(subwindow), "Result");

	grid = gtk_grid_new();
	gtk_window_set_child(GTK_WINDOW(subwindow), grid);

	label = gtk_label_new(buf);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

	button1 = gtk_button_new_with_label("OK");
	gtk_grid_attach(GTK_GRID(grid), button1, 1, 3, 1, 1);
	g_signal_connect_swapped(button1, "clicked", G_CALLBACK(gtk_window_destroy), subwindow);
	//g_signal_connect_swapped(button1, "clicked", G_CALLBACK(gtk_window_destroy), win->window);
	//g_signal_connect(button1, "clicked", G_CALLBACK(back2view_grp), win);
	
	gtk_widget_show(subwindow);
}
