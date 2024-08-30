#include <ncurses.h>
#include <locale.h>
#include <libintl.h>
#include <stdlib.h>
#include <string.h>

#include "ip.h"

int main()
{
	char ip[_IP_SIZE];
	char netmask[_IP_SIZE];
	char username[MAXUSERNAME];
	char password[MAXPASS];
	int *_ip_val = NULL;
	int *_netmask_val = NULL;
	char **ip_val = NULL;
	extern char broadcast[];
	
	initscr();
	cbreak();
	
	setlocale(LC_ALL, "");
	bindtextdomain(PROGRAME_NAME, "/usr/share/locale");
	textdomain(PROGRAME_NAME);
	
	getid(username, password);
	
	while(TRUE){
		get_ip(ip);
		_netmask_val = get_netmask(netmask);
		ip_val = split_ip_value(ip);
		_ip_val = check_ip_content(ip_val);
		write_broadcast(_ip_val, _netmask_val);
		check_ip_range_and_display(_ip_val, _netmask_val);
	}
	
	clrtoeol();
	refresh();
	endwin();
	return EXIT_SUCCESS;
}
