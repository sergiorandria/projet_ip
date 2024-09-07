#include <ncurses.h>
#include <locale.h>
#include <libintl.h>
#include <stdlib.h>
#include <string.h>

#include "ip.h"

int main()
{
	int cidr;
	int *_netip;
	char ip[_IP_SIZE];
	char netmask[_IP_SIZE];
	char *netmask_cidr = NULL;
	char username[MAXUSERNAME];
	char password[MAXPASS];
	int *_ip_val = NULL;
	int *_netmask_val = NULL;
	char **ip_val = NULL;
	extern char broadcast[];
	
	initscr();
	cbreak();
	
	setlocale(LC_ALL, "");
	bindtextdomain(PROGRAM_NAME, "/usr/share/locale");
	textdomain(PROGRAM_NAME);
	
	getid(username, password);
	
	while(TRUE){
		get_ip(ip);
		ip_val = split_ip_value(ip);
		if(cidr_or_not(ip_val)){
			cidr = verify_cidr(ip_val[3]);
			_ip_val = check_ip_content(ip_val, CIDR);
			netmask_cidr = generate_netmask_from_cidr(cidr);
			_netmask_val = get_netmask(netmask_cidr, CIDR);
		}
		else {
			_ip_val = check_ip_content(ip_val, NOT_CIDR);
			_netmask_val = get_netmask(netmask, NOT_CIDR);
		}
		write_broadcast(_ip_val, _netmask_val);
		_netip = check_ip_range_and_display(_ip_val, _netmask_val);
		subneting(_netip, _netmask_val);
	}
	
	clrtoeol();
	refresh();
	endwin();
	return EXIT_SUCCESS;
}
