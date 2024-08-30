#include <time.h>

#define NOT_VALID 1
#define VALID 0
#define TIME_ELAPSED 2
#define MAXPASS 100
#define MAXUSERNAME 100
#define WIDTH 30
#define HEIGHT 10
#define _IP_SIZE 15

#define TRUE 1
#define FALSE 0

#define split_netmask_value(STRING) split_ip_value(STRING)

#define _(STRING) gettext(STRING)
#define PROGRAME_NAME "ip"

enum PASS_FLAGS {
	HIDDEN_PASSWORD,
	SHOW_PASSWORD,
	SHOW_PASSWORD_LEN,
	SHOW_PASSWORD_ASTX,
};

enum CLASS { A = 1, B, C, D, E };

int check(char *ip);
int __strlen(char *string);
int numeric_value(char *s);
char **split_ip_value(char *ip);
int *check_ip_content(char **ip_val);
void get_ip(char ip[]);
void check_ip_range_and_display(int *ip, int *netmask);
void get_password(char *password, int getpass_flag);
void getid(char *username, char *password);
void concatenate_strings(char *string, char char_value, char *first_string, char *second_string);
int strings_are_same(char *first_string, char *second_string);
int verify_username(char *username);
static void handler(int signum);
int will_segfault(void);
int len_of_two_string(char *first_string, char *second_string);
void print_menu(WINDOW *menu_win, int highlight);
int print_selection(void);
void print_t(time_t time, char *string);
int *get_netmask(char *netmask);
int check_netmask_content(int netmask);
int check_netmask_generally(int *netmask);
int *generate_broadcast(int *n_ip, int *netmask);
int *reverse_netmask(int *netmask);
void save_ip_content(int *ip);
void write_broadcast(int *_ip_val, int *_netmask_val);
char *compute_netip(int *_ip_val, int *_netmask_val);
long double get_the_host_number(int *_netmask_val);

