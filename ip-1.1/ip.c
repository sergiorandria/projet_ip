#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <ncurses.h>
#include <time.h>
#include <math.h>

#include "ip.h"

static bool display_password;
static bool hidden_password; 
static bool default_netmask;

int size = 5;
int startx;
int starty;
int ip_class;
int netmask_class;

char netmask_temp[_IP_SIZE];   
int ip_temp[4];
char broadcast[_IP_SIZE];
 
char *choices[] = {
	 "1 - Se connecter",
	 "2 - Creer un compte",
	 "3 - Supprimer votre compte",
	 "4 - Parametres",
	 "5 - Quitter",
};

void get_ip(char ip[])
{
	echo();
	
	printw(_("\
  Entrer l'addresse IP\n\
  IP:"));
	refresh();
	scanw("%s", ip);
}

int check(char *ip)
{
	int n, value;
	
	n = 0;
	for(size_t i = 0; ip[i] != '\0'; ++i)
	{
		if(ip[i] == '.')	
			++n;
	}
	
	value = (n != 3) ?  NOT_VALID: VALID;
	return value;
}

char **split_ip_value(char *ip)
{
	int n = 0;
	char **ip_val = NULL;

	ip_val = (char **)malloc(4 * sizeof(char *));
	if (!ip_val){
		printw("Erreur d'allocation(split_ip_value())");
		refresh();
		endwin();
		exit(EXIT_FAILURE);
	}
	
	for(size_t i = 0; i < 4; ++i){
		ip_val[i] = (char *)malloc(10 * sizeof(char));
		if (!ip_val){
			printw("Erreur d'allocation(split_ip_value())");
			refresh();
			endwin();
			exit(EXIT_FAILURE);
			}
		}
	
	if (check(ip)){
		printw(_("Adresse ip non valide (Appuyer sur une touche pour quitter) ..."));
		refresh();
		getch();
		endwin();
		exit(EXIT_FAILURE);
	}
	
	char *token = strtok(ip, ".");
	
	strcpy(ip_val[n], token);
	while(token != NULL){
		strcpy(ip_val[n++], token);
		token = strtok(NULL, ".");
		}
	
	return ip_val;
}

int *check_ip_content(char **ip_val, int flag)
{
	int err = 0;
	int *_ip_val = NULL;
	
	_ip_val = (int *)malloc(4 * sizeof(int));
	if (!_ip_val){
		printw("Erreur d'allocation(check_ip_content())");
		refresh();
		endwin();
		exit(EXIT_FAILURE);
	}
	
	for(size_t i = 0; i < 4; ++i){
		if(numeric_value(ip_val[i]) && (flag == NOT_CIDR)){
			err = 1;
			break;
		}
	}
	
	if (!err){
		for(size_t i = 0; i < 4; ++i){ 
			_ip_val[i] = atoi(ip_val[i]);
		}
		
		return _ip_val;
	}
	else{
		printw(_("Adresse IP non valide (Appuyer sur une touche pour quitter)..."));
		refresh();
		getch();
		endwin();
		exit(EXIT_FAILURE);
	}
}

int numeric_value(char *s)
{
	int value = VALID;
	
	for(size_t i = 0; i < strlen(s); ++i){	
			if ((s[i] > '9') || (s[i] < '0') && ((s[i] != '\n') || (s[i] != '/'))){
				value = NOT_VALID;
				break;
			}
	}

	return value;
}

int *check_ip_range_and_display(int *ip, int *netmask)
{
	int err = 0;
	int *_netip = NULL;
	char *netip;
	long double host;
	
	_netip = malloc(4 * sizeof(int));
	netip = compute_netip(ip,netmask);
	sscanf(netip, "%d.%d.%d.%d\n", &_netip[0], &_netip[1], &_netip[2], &_netip[3]);
		
	host = get_the_host_number(netmask);
	for(size_t i = 0; i < 4; ++i)
	{
		if (ip[i] > 255 || ip[i] < 0){
			printw(_("Adresse ip non valide(Appuyer sur une touche pour quitter) ...")); 
			refresh();
			err = 1;
			break;
			}
	}
	
	if (!err){
		if (ip[0] <= 127 && ip[0] >= 0)
		{
			attron(A_BOLD);
			printw(_("La classe de l'addresse IP est A\n"));
			printw(_("Addresse du reseau: %s\n"), netip);
			printw(_("Netmask: %s\n"), netmask_temp);
			printw(_("Broadcast: %s\n"), broadcast);
			printw(_("Nombres d'hote: %Lg\n\n"), host);
			attroff(A_BOLD);
		}
		else if (ip[0] <= 191 && ip[0] >= 128)
		{	
			attron(A_BOLD);
			printw(_("La classe de l'addresse IP est B\n"));
			printw(_("Addresse du reseau: %s\n"), netip);
			printw(_("Netmask: %s\n"), netmask_temp);
			printw(_("Broadcast: %s\n"), broadcast);
			printw(_("Nombres d'hote: %Lg\n\n"), host);
			attroff(A_BOLD);
		}
		else if (ip[0] <= 223 && ip[0] >= 192)
		{	
			attron(A_BOLD);
			printw(_("La classe de l'addresse IP est C\n"));
			printw(_("Addresse du reseau: %s\n"), netip);
			printw(_("Netmask: %s\n"), netmask_temp);
			printw(_("Broadcast: %s\n"), broadcast);
			printw(_("Nombre d'hote: %Lg\n\n"), host);
			attroff(A_BOLD);
		}
		else if (ip[0] <= 239 && ip[0] >= 224)
		{
			attron(A_BOLD);
			printw(_("La classe de l'addresse IP est D\n"));
			printw(_("Nombre d'hote: %Lg\n\n"), host);
			attroff(A_BOLD);
		}
		else if (ip[0] <= 255 && ip[0] >= 240)
		{	
			attron(A_BOLD);
			printw(_("La classe l'addresse IP est E\n"));
			printw(_("Nombre d'hote: %Lg\n\n"), host);
			attroff(A_BOLD);
		}
		
		return _netip;
	}
	
	noecho();
	refresh();
	getch();
}

void getid(char *username, char *password)
{
	int switch_value;
	
	mvprintw(1, 0, "\
		Ce petit programme permet de classifier un addresse ip donnee par l'utilisateur.\n\
		\t Classe A: 0.0.0.0 - 127.255.255.255\n\
		\t Classe B: 128.0.0.0 - 191.255.255.255\n\
		\t Classe C: 192.0.0.0 - 223.255.255.255\n\
		\t Classe D: 224.0.0.0 - 239.255.255.255\n\
		\t Classe E: 240.0.0.0 - 255.255.255.255\n\n\
				Appuyer sur une touche ...\n\
		");

	switch_value = print_selection();
	refresh();
fetch_data:
	printw(">");
	
	refresh();
	echo();
	switch(switch_value)
	{	
			case 1:
			{	
				int flag, same;
				size_t max_nbytes = MAXPASS + MAXUSERNAME + 1;
				int login_attempt = 0;
				char *string_concatenate_f = NULL;
				char *string_concatenate = NULL;
				FILE *fd = NULL;
				
				fd = fopen("data.csv", "r+");
				if (!fd) {
					printw(_("Erreur lors de l'ouverture de la base de donnee"));
					refresh();
					endwin();
					exit(EXIT_FAILURE);
					}
				
				clear();
			login:
				printw("username: ");
				refresh();
				scanw("%s", username);
				printw("password: ");
				refresh();
				get_password(password, 0);
				
				string_concatenate = (char *)malloc((strlen(password) + strlen(username) + 1) * sizeof(char));
				if (!string_concatenate){
					printw(_("Erreur de connexion"));
					refresh();
					endwin();
					exit(EXIT_FAILURE);
					}
				
				memset(string_concatenate,0,strlen(password) + strlen(username));
				string_concatenate_f = (char *)malloc((MAXPASS + MAXUSERNAME + 1) * sizeof(char));
				if (!string_concatenate_f){
					printw(_("Erreur de connexion"));
					refresh();
					endwin();
					exit(EXIT_FAILURE);
					}
				
				memset(string_concatenate_f, 0, MAXPASS + MAXUSERNAME + 1);
				
				flag = FALSE;
				concatenate_strings(string_concatenate, ':', username, password);
				while(fgets(string_concatenate_f, MAXPASS + MAXUSERNAME + 1, fd) != NULL)
				{
					if (__strlen(string_concatenate) == (__strlen(string_concatenate_f) - 1))
					{	
						if (strings_are_same(string_concatenate, string_concatenate_f)){
							flag = TRUE;
							break;
							}
					}
				}
				
				if (!flag && (login_attempt < 2)){
					login_attempt++;
					printw("Nom d'utilisateur ou mot de passe incorrect\n");
					refresh();
					rewind(fd);
					sleep(2);
					clear();
					goto login;
					}
				else if (!flag && login_attempt == 2){
					printw(_("Nombre d'essai depasse\nexit.\n"));
					refresh();
					fclose(fd);
					endwin();
					exit(EXIT_FAILURE);
				}
				else {
					printw(_("Authentication reussie\n\n"));
					refresh();
				}
				fclose(fd);
			}
			break;
			case 2:
			{	
				FILE *fd;
				char password_temp[MAXPASS];
				
				fd = fopen("data.csv", "a+");
				if (!fd){
					printw(_("Erreur lors de l'ouverture de la base de donnee"));
					refresh();
					endwin();
					exit(EXIT_FAILURE);
					}
				
				clear();
			get_username:
				printw(_("Username: "));
				refresh();
				scanw("%s", username);
				if (verify_username(username)){
					printw("Nom d'utilisateur invalide\n");
					refresh();
					goto get_username;
					}
			if_password_doesnt_match:	
				printw(_("Password: "));
				refresh();
				scanw("%s", password);
				printw(_("Retype password: "));
				refresh();
				scanw("%s", password_temp);
				
				if (strcmp(password, password_temp)) {
					printw(_("Les mots de passe ne correspondent pas\n"));
					refresh();
					goto if_password_doesnt_match;
				}
				
				fprintf(fd, "%s:%s\n", username, password);
				fclose(fd);
			}
			break;
			case 3:
				endwin();
				exit(EXIT_SUCCESS);
			break;
			case 4:
				endwin();
				exit(EXIT_SUCCESS);
			break;
			case 5:
				endwin();
				exit(EXIT_SUCCESS);
			break;
			default:
			{
				printw(_(" Veuillez entrer un choix valide \n\t"));
				refresh();
				sleep(TIME_ELAPSED);
				goto fetch_data;
			}
	}
}

void concatenate_strings(char *string, char char_value, char *first_string, char *second_string)
{
	int i;
	char *charptr;
	
	charptr = string;
	for(i = 0; first_string[i] != '\0'; ++i)
		*charptr++ = first_string[i];
		
	*charptr++ = char_value;
	for(i = 0; second_string[i] != '\0'; ++i)
		*charptr++ = second_string[i];
		
	*charptr = '\0';
}

int len_of_two_string(char *first_string, char *second_string)
{
	int retval = TRUE;
	
	if (__strlen(first_string) != __strlen(second_string))
		retval = FALSE;
		
	return retval;
}	

int strings_are_same(char *first_string, char *second_string)
{
	int ret_val;
	
	ret_val = TRUE;
	for(size_t i = 0; first_string[i] != '\0'; ++i)
	{
		if (first_string[i] != second_string[i]){
			ret_val = FALSE;
			break;
		}
	}

	return ret_val;
}

int verify_username(char *username)
{
	int ret_val;
	
	ret_val = VALID;
	if(username == NULL)
	{
		username = (char *)malloc(MAXUSERNAME * sizeof(char));
		if (!username){
			endwin();
			exit(EXIT_FAILURE);
		}
	}
	
	for(size_t i = 0; username[i] != '\0'; ++i)
	{
		if (username[i] == ' '){
			ret_val = NOT_VALID;
			break;
			}
	}
	
	return ret_val;
}

void get_password(char *password, int getpass_flag)
{
	int c, i = 0;

	noecho();
	
	if(password == NULL)
	{
		password = (char *)malloc(MAXPASS * sizeof(char));
		if (!password){
			endwin();
			exit(EXIT_FAILURE);
		}
	}
	
	while((c = getch()) != '\n' && c != EOF && i < MAXPASS){
		password[i++] = c;
	}
	
	password[i] = '\0';
	addch('\n');
	echo();
}

int will_segfault(void)
{	
	int have_sigsegv = FALSE;
	struct sigaction sa;
	
	sa.sa_handler = (__sighandler_t)handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
		
	if (sigaction(SIGSEGV, &sa, NULL)){
		have_sigsegv = TRUE;
	}

	return have_sigsegv;
}

static void handler(int signum)
{	
	printw("Size of string[] out of range\n");
	endwin();
	exit(EXIT_FAILURE);
}

int __strlen(char *string)
{
	int i;
	
	i = 0;
	while(string[i] != '\0')
		++i;
	
	return i;
}

void print_menu(WINDOW *menu_win, int highlight)
{
	int x, y, i;
	
	start_color();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	mvchgat(0, 0, -1, A_BLINK, 1, NULL);
	refresh();
	noecho();
	
	x = 2;
	y = 2;
	box(menu_win, 0, 0);
	for(i = 0; i < size; ++i)
	{
		if(highlight == i + 1)
		{
			wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		++y;
	}
	wrefresh(menu_win);
}

int print_selection(void)
{
	WINDOW *menu_win;
	int highlight = 1;
	int choice = 0;
	int c;
	
	startx = (90 - WIDTH)/2;
	starty = (30 - HEIGHT)/2;
	
	menu_win = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(menu_win, TRUE);
	mvprintw(0, 0, "Utiliser les touches directionnelles, Appuyer sur entrer pour selectionner");
	refresh();
	while(1)
	{
		c = wgetch(menu_win);
		switch(c)
		{
			case KEY_UP:
				if (highlight == 1)
					highlight = size;
				else
					highlight--;
				break;
			case KEY_DOWN:
				if (highlight == size)
					highlight = 1;
				else
					highlight++;
				break;
			case 10:
				choice = highlight;
				break;
			default:
				break;
		}
		print_menu(menu_win, highlight);
		if (choice != 0)
			break;
	}
	
	refresh();
	return choice;
}

void print_t(time_t time, char *string)
{
	int i;
	
	i = 0;
	while(string[i] != '\0'){
		addch(string[i++]);
		sleep(time);
	}
}
	
int *get_netmask(char *netmask, int flag)
{
	char **netmask_val = NULL;
	int *_netmask_val = NULL;
	
	netmask_val = (char **)malloc(4 * sizeof(char));
	if (!netmask_val){
		endwin();
		exit(EXIT_FAILURE);
	}
	
	for(size_t i = 0; i < 4; ++i)
	{
		netmask_val[i] = (char *)malloc(8 * sizeof(char));
		if (!netmask_val[i]){
			endwin();
			exit(EXIT_FAILURE);
			}
	}
	
	if (flag == NOT_CIDR){
		memset(netmask, 0, _IP_SIZE);
	get_netmask:
		echo();
		printw(_("  Netmask: "));
		scanw("%s", netmask);
	
		strcpy(netmask_temp, netmask);
		if (check(netmask)){
			printw(_("Netmask non valide (Appuyer sur une touche pour quitter) ...\n"));
			refresh();
			getch();
			goto get_netmask;
		}
	
		if (!numeric_value(netmask) && netmask != NULL){
			printw(_("Netmask non valide (Appuyer sur une touche pour quitter) ...\n"));
			refresh();
			getch();
			noecho();
			goto get_netmask;
		}
	
		netmask_val = split_netmask_value(netmask);
		_netmask_val = check_ip_content(netmask_val, NOT_CIDR);
		if (!check_netmask_generally(_netmask_val)){
			printw(_("Netmask non valide (Appuyer sur une touche pour quitter) ...\n"));
			refresh();
			getch();
			goto get_netmask;
		}
	}
	else {
		strcpy(netmask_temp, netmask);
		netmask_val = split_netmask_value(netmask);
		_netmask_val = check_ip_content(netmask_val, CIDR);
	}
	
	return _netmask_val;
}

int check_netmask_content(int netmask)
{
	int i, n, retval;
	
	n = 0;
	retval = TRUE;
	for(i = 7; i >= 0; --i)
	{
		n += (int)pow(2, i); 
		if (n > netmask){
			retval = FALSE;
			break;
		}
		else if (n == netmask){
			break;
		}
	}

	return retval;
}
	
int check_netmask_generally(int *netmask)
{
	int i, retval, flag;
	
	flag = FALSE;
	retval = TRUE;
	for(i = 0; i < 4; ++i)
	{
		if (!flag && !check_netmask_content(netmask[i]) && netmask[i] != 0){
			retval = FALSE;
			break;
		}
		
		if (netmask[i] == 0)
			flag = TRUE;
		
		else if (flag == TRUE)
			retval = FALSE;
	}
	
	return retval;
}

int *generate_broadcast(int *n_ip, int *netmask)
{
	int i;
	int *broadcast = NULL;
	
	broadcast = (int *)malloc(4 * sizeof(int));
	if (!broadcast)
	{
		endwin();
		exit(EXIT_FAILURE);
	}
	
	for(i = 0; i < 4; ++i){
		broadcast[i] = n_ip[i] | netmask[i];
	}
	
	return broadcast;
}
	
int *reverse_netmask(int *netmask)
{
	int i;
	int *result = NULL;
	
	result = (int *)malloc(4 * sizeof(int));
	if (!result)
		exit(EXIT_FAILURE);
	
	for(i = 0; i < 4; ++i)
		result[i] = 255 - netmask[i];

	return result;
}

void save_ip_content(int *ip)
{
	int i;
	
	for(i = 0; i < 4; ++i)
		ip_temp[i] = ip[i];
}

void write_broadcast(int *_ip_val, int *_netmask_val)
{
	int *_broadcast = NULL;
	extern char broadcast[];
		
	_broadcast = generate_broadcast(_ip_val, reverse_netmask(_netmask_val));	
	memset(broadcast, 0, _IP_SIZE);
	sprintf(broadcast, "%d.%d.%d.%d", _broadcast[0],_broadcast[1],_broadcast[2],_broadcast[3]);
}

char *compute_netip(int *_ip_val, int *_netmask_val)
{
	int i, *stock = NULL;
	char *netip = NULL;
	
	stock = (int *)malloc(4 * sizeof(int));
	if (!stock){
		endwin();
		exit(EXIT_FAILURE);
	}
	
	netip = (char *)malloc(15 * sizeof(char));
	if (!netip){
		endwin();
		exit(EXIT_FAILURE);
	}
	
	for(i = 0; i < 4; ++i){
		stock[i] = _ip_val[i] & _netmask_val[i];
	}
	
	sprintf(netip, "%d.%d.%d.%d", stock[0],stock[1],stock[2],stock[3]);
	return netip;
}

long double get_the_host_number(int *_netmask_val)
{
	int i, n, n_iter, state;
	long double host;
	
	host = 0;
	state = FALSE;
	n_iter = 0;
	for(i = 0; i < 4; ++i)
	{
		if (_netmask_val[i] == 255)
				++n_iter;
		else if(_netmask_val[i] != 0)
		{
			n = _netmask_val[i];
			while((n % 2) != 1)
			{
				n = n >> 1;
				++host;  
			}
			state = TRUE;
		}
	}
	
	i = (state == TRUE) ? 3 - n_iter: 4 - n_iter;
	host = pow(2, host + 8 * i) - 2;
	return host;
}

int cidr_or_not(char **ip)
{
	int i, retval = 0;
	
	for(i = 0; i < strlen(ip[3]); ++i)
	{
		if (ip[3][i] == '/')
		{
			retval = 1;
			break;
		}
	}
	
	return retval;
}

int verify_cidr(char *ip)
{
	int n, flag = FALSE;
	int cidr_int_val;
	char *cidr = NULL;
	char *last_ip_val;
	
	cidr = (char *)malloc(7 * sizeof(char));
	if (!cidr){
		printw("Corruption de memoire");
		getch();
		endwin();
		exit(EXIT_FAILURE);
	}
	
	last_ip_val = (char *)malloc(3 * sizeof(char));
	if (!last_ip_val){
		printw("Corruption de memoire");
		getch();
		endwin();
		exit(EXIT_FAILURE);
	}
	
	n = 0;
	for(size_t i = 0; i < strlen(ip); ++i)
	{
		if (ip[i] == '/')
			flag = TRUE;
		else if (flag == TRUE)
			cidr[n++] = ip[i];
	
		if (flag == FALSE)
			last_ip_val[i] = ip[i];
	}
	
	cidr[n] = '\0';
	
	strcpy(ip, last_ip_val); 
	if(numeric_value(cidr)){
		printw("Invalide CIDR");
		getch();
		endwin();
		exit(EXIT_FAILURE);
	}
	
	cidr_int_val = atoi(cidr);
	if (cidr_int_val < 0 || cidr_int_val > 32){
		printw("Invalide CIDR");
		getch();
		endwin();
		exit(EXIT_FAILURE);
	}

	return cidr_int_val;
}

char *generate_netmask_from_cidr(int cidr)
{
	int div, mod, result; 
	char *netmask = NULL;
	
	netmask = malloc(15 * sizeof(char));
	if (netmask == NULL){
		printw("Netmask Invalide");
		getch();
		endwin();
		exit(EXIT_FAILURE);
	}
	
	mod = cidr % 8;
	result = 0;
	for(size_t i = 0; i < mod; ++i)
		result += pow(2, 7 - i);
	
	div = cidr/8;
	switch(div)
	{
		case 0:
			sprintf(netmask, "%d.0.0.0", result);
		break;
		case 1:
			sprintf(netmask, "255.%d.0.0", result);
		break;
		case 2:
			sprintf(netmask, "255.255.%d.0", result);
		break;
		case 3:
			sprintf(netmask, "255.255.255.%d", result);
		break;
		case 4:
			sprintf(netmask, "255.255.255.255");
		break;
	}
	
	return netmask;
}

void subneting(int *netip, int *netmask)
{
	int i, n, subnet, bit, flag = FALSE, index = 0;
	int *subnetmask;
	int *subnetip;
	int *subnet_first_val;
	int *subnet_last_val;
	
	printw(_("Nombre de subnetting: "));
	scanw("%d", &subnet);
	
	if (subnet <= 0){
		exit(EXIT_FAILURE);
	}
	
	subnetmask = malloc(4 * sizeof(int));
	subnet_first_val = malloc(4 * sizeof(int));
	subnet_last_val = malloc(4 * sizeof(int));
	subnetip = malloc(4 * sizeof(int));
	
	for(bit = 0; pow(2, bit) < subnet; ++bit)
		;

	for(i = 0; i < 4; ++i)
	{
		subnetmask[i] = netmask[i];
		if (flag == TRUE)
		{
			n = (n + bit) % 8;
			for(size_t j = 0; j <= n; ++j)
				subnetmask[i] += (int)pow(2, 7 - j);
			break;
		}
		
		if (subnetmask[i] != 0 && subnetmask[i] != 255 && !flag)
		{
			n = get_bit(subnetmask[i]);
			if (n + bit > 8)
			{
				subnetmask[i] = 255;
				flag = TRUE;
			}
			else
			{
				for(size_t j = 1; j <= bit; ++j)
					subnetmask[i] += (int)pow(2, 8 - (n + j));
				break;
			}
		}

		if (subnetmask[i] == 0)
		{
			for(size_t j = 1; j <= bit; ++j)
				subnetmask[i] += (int)pow(2, 7 - j + 1); 
			
			break; 
		}
	}
	
	attron(A_BOLD);
	printw(" Netmask du sous-reseau: %d.%d.%d.%d\n", subnetmask[0], subnetmask[1], subnetmask[2], subnetmask[3]);
	attroff(A_BOLD);
	for(i = 3; i >= 0; --i)
	{
		if (subnetmask[i] != 0)
		{
				n = subnetmask[i];
				index = i;
				break;
		}
	}

	for(i = 0; i < 4; ++i)
		subnetip[i] = netip[i];
	
	n = 256 - n;
	printw("%d,%d\n", index, n);
	while(subnetip[index] < 255)
	{
		printw("%d.%d.%d.%d\t", subnetip[0], subnetip[1], subnetip[2], subnetip[3]);
		for(i = 0; i < 3; ++i)
			subnet_first_val[i] = subnetip[i];
		
		subnet_first_val[i] = subnetip[i] + 1;
		
		printw("%d.%d.%d.%d - ", subnet_first_val[0], subnet_first_val[1], subnet_first_val[2], subnet_first_val[3]);
		for(i = 0; i < index; ++i)
			subnet_last_val[i] = subnet_first_val[i];
			
		subnet_last_val[index] = subnet_first_val[index] + n - 1;
		
		for(i = index + 1; i < 3; ++i)
			subnet_last_val[i] = 255;
		
		subnet_last_val[3] = 254;
		printw("%d.%d.%d.%d\n", subnet_last_val[0], subnet_last_val[1], subnet_last_val[2], subnet_last_val[3]);
		subnetip[index] += n;
	}
	
}

int get_bit(int n)
{
	int i, tmp;
	
	i = 0;
	tmp = n;
	while(tmp > 0 && i <= 7)
	{
		tmp -= (int)pow(2, 7 - i);
		++i;
	}
	
	return i;
}

