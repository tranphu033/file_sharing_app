#define MAXN 6
#define MAXS 100

typedef struct message_{
    int code;
    char payld[MAXN][MAXS];
    //char big_payld[1024];
}message;

enum msg_code{
    LOGIN=1, UN_INCORR, PWD_INCORR, LOGIN_SUCC,
    SIGNUP,
    SHW_GRPS,
    VIEW_GRP, SHOW_MEM, ADD_MEM, DEL_MEM,
    VIEW_FOL, SHW_FOL, SHW_F,
    UP_FILE, DOWN_FILE, DEL_FILE, DEL_FOL,
    CRT_GRP,
    JOIN_GRP,
    CRT_FOL,
    LOGOUT,
    NO_ACT, SKIP, SUCC, ERR, BACK
};
