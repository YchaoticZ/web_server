#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DEBUG 0

#define STAT_200 " 200 OK\r\n"
#define STAT_404 " 404 Not Found\r\n"
#define STAT_501 " 501 Not Implemented\r\n"

#define F_GIF "Content-Type: image/gif\r\n"
#define F_HTML "Content-Type: text/html\r\n"
#define F_JPEG "Content-Type: image/jpeg\r\n"
#define F_JPG "Content-Type: image/jpg\r\n"
#define F_TXT "Content-Type: text/plain\r\n"

typedef enum {false, true} bool;
typedef enum {cgi, exe, gif, html, jpeg, jpg, plain} ext;

ext get_ext(char *file) {
    if (strstr(file, ".cgi") != NULL)
	return cgi;
    if (strstr(file, ".") == NULL)
	return exe;
    if (strstr(file, ".gif") != NULL)
	return gif;
    if (strstr(file, ".html") != NULL)
	return html;
    if (strstr(file, ".jpeg") != NULL)
	return jpeg;
    if (strstr(file, ".jpg") != NULL)
	return jpg;
    if (strstr(file, ".txt") != NULL)
	return plain;
}

bool has_args(char *request) {
    int i;
    for (i = 0; i < strlen(request); i++) {
	if (request[i] == '?')
	    return true;
    }
    return false;
}

int num_args(char *request) {
    int i, n_args = 0;
    char *start = (strstr(request, "?") + 1);
    for (i = 0; i < strlen(start); i++) {
	if (start[i] == '&')
	    n_args++;
    }
    return n_args;
}

void parse(int accept_fd) {
    char *content = (char *) malloc(sizeof(char) * 512);
    if (read(accept_fd, content, 512) < 0) {
	perror("read error");
	exit(1);
    }

    char *part_end = strstr(content, "\n");
    *(part_end - 1) = 0;

#if DEBUG
    /* the client doesn't write anything unless this is the first thing written */
    write(accept_fd, content, strlen(content));
    write(accept_fd, "\r\n", 2);
#endif

    char *type = (char *) malloc(sizeof(char) * 16); // "GET"
    char *request = (char *) malloc(sizeof(char) * 256); // "/request"
    char *version = (char *) malloc(sizeof(char) * 16); // "HTTP/x.x"
    strncpy(type, content, 16);
    strcpy(request, strstr(content, "/"));
    strcpy(version, strstr(content, "HTTP"));
    part_end = strstr(type, "/");
    *(part_end - 1) = 0;
    part_end = strstr(request, "HTTP");
    *(part_end - 1) = 0;

    write(accept_fd, version, strlen(version)); // write the "HTTP/x.x"

    /* http://ip_addr:port/request?name1=val1&name2=val2&name3=val3&name4=val4& */
    char *temp = (char *) malloc(sizeof(char) * 256);
    char *arg_1 = (char *) malloc(sizeof(char) * 256);
    char *arg_2 = (char *) malloc(sizeof(char) * 256);
    char *arg_3 = (char *) malloc(sizeof(char) * 256);
    char *arg_4 = (char *) malloc(sizeof(char) * 256);
    char *arg_5 = (char *) malloc(sizeof(char) * 256);
    char *arg_6 = (char *) malloc(sizeof(char) * 256);

    bool req_arg = has_args(request);
    if (req_arg) {
	int i, n_args = num_args(request);
	char *amp, *eq, *arg_str = (char *) malloc(sizeof(char) * 256);
	strcpy(arg_str, (strstr(request, "?") + 1));
	*strstr(request, "?") = 0;

	for (i = 0; i < n_args; i++) {
	    if (i == 0) {
		amp = strstr(arg_str, "&");
		*amp = 0;
		strcpy(temp, arg_str);
		eq = strstr(temp, "=");
		strncpy(arg_1, eq + 1, strlen(eq + 1));
		arg_str = amp + 1;
	    } else if (i == 1) {
		amp = strstr(arg_str, "&");
		*amp = 0;
		strcpy(temp, arg_str);
		eq = strstr(temp, "=");
		strncpy(arg_2, eq + 1, strlen(eq + 1));
		arg_str = amp + 1;
	    } else if (i == 2) {
		amp = strstr(arg_str, "&");
		*amp = 0;
		strcpy(temp, arg_str);
		eq = strstr(temp, "=");
		strncpy(arg_3, eq + 1, strlen(eq + 1));
		arg_str = amp + 1;
	    } else if (i == 3) {
		amp = strstr(arg_str, "&");
		*amp = 0;
		strcpy(temp, arg_str);
		eq = strstr(temp, "=");
		strncpy(arg_4, eq + 1, strlen(eq + 1));
		arg_str = amp + 1;
	    } else if (i == 4) {
		amp = strstr(arg_str, "&");
		*amp = 0;
		strcpy(temp, arg_str);
		eq = strstr(temp, "=");
		strncpy(arg_5, eq + 1, strlen(eq + 1));
		arg_str = amp + 1;
	    } else if (i == 5) {
		amp = strstr(arg_str, "&");
		*amp = 0;
		strcpy(temp, arg_str);
		eq = strstr(temp, "=");
		strncpy(arg_6, eq + 1, strlen(eq + 1));
		arg_str = amp + 1;
	    }
	}
    }

    struct stat f_stat;
    int stat_fd;
    char *cwd = (char *) malloc(sizeof(char) * 256);
    char *f_name = (char *) malloc(sizeof(char) * 256);
    if ((cwd = getcwd(cwd, 256)) == NULL) {
	perror("getcwd error");
	close(accept_fd);
	exit(1);
    }
    strcpy(f_name, cwd);
    strcat(f_name, request);

    if (strcmp(type, "GET") != 0) { // 501
	write(accept_fd, STAT_501, strlen(STAT_501));

	f_name = "./html/501.html";
	if ((stat_fd = stat(f_name, &f_stat)) < 0) {
	    perror("501.html stat error");
	    close(accept_fd);
	    exit(1);
	}

	FILE *f_open;
	char *f_cont = (char *) malloc(sizeof(char) * 1048476);
	size_t f_size = f_stat.st_size;
	char *con_len = (char *) malloc(sizeof(char) * 64);

	sprintf(con_len, "Content-Length: %d\r\n", (int) f_size);
	write(accept_fd, con_len, strlen(con_len));
	write(accept_fd, "\r\n", 2);

	if ((f_open = fopen(f_name, "r")) == NULL) {
	    perror("501.html fopen error");
	    close(accept_fd);
	    exit(1);
	}

	fread(f_cont, sizeof(char), f_size, f_open);
	write(accept_fd, f_cont, f_size);
	close(accept_fd);
	exit(0);
    }

    if ((stat_fd = stat(f_name, &f_stat)) < 0) { // 404
	write(accept_fd, STAT_404, strlen(STAT_404));

	f_name = "./html/404.html";
	if ((stat_fd = stat(f_name, &f_stat)) < 0) {
	    perror("404.html stat error");
	    close(accept_fd);
	    exit(1);
	}

	FILE *f_open;
	char *f_cont = (char *) malloc(sizeof(char) * 1048476);
	size_t f_size = f_stat.st_size;
	char *con_len = (char *) malloc(sizeof(char) * 64);

	sprintf(con_len, "Content-Length: %d\r\n", (int) f_size);
	write(accept_fd, con_len, strlen(con_len));
	write(accept_fd, "\r\n", 2);

	if ((f_open = fopen(f_name, "r")) == NULL) {
	    perror("404.html fopen error");
	    close(accept_fd);
	    exit(1);
	}

	fread(f_cont, sizeof(char), f_size, f_open);
	write(accept_fd, f_cont, f_size);
	close(accept_fd);
	exit(0);
    }

    if S_ISDIR(f_stat.st_mode) { // 200
	write(accept_fd, STAT_200, strlen(STAT_200));
	write(accept_fd, F_TXT, strlen(F_TXT));
	write(accept_fd, "\r\n", 2);

	int red;
	if ((red = dup2(accept_fd, STDOUT_FILENO)) < 0) {
	    perror("dup error");
	    close(accept_fd);
	    exit(1);
	}
	if ((red = dup2(accept_fd, STDERR_FILENO)) < 0) {
	    perror("dup error");
	    close(accept_fd);
	    exit(1);
	}
	if (execlp("ls", "ls", f_name, NULL) < 0) {
	    perror("exec error");
	    close(accept_fd);
	    exit(1);
	}

	close(accept_fd);
	exit(0);
    } else if S_ISREG(f_stat.st_mode) { // 200
	write(accept_fd, STAT_200, strlen(STAT_200));
	ext f_ext = get_ext(f_name);

	if (f_ext == cgi) {
	    int red;
	    if ((red = dup2(accept_fd, STDOUT_FILENO)) < 0) {
		perror("dup error");
		close(accept_fd);
		exit(1);
	    }
	    if ((red = dup2(accept_fd, STDERR_FILENO)) < 0) {
		perror("dup error");
		close(accept_fd);
		exit(1);
	    }
	    if (execlp(f_name, f_name, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, NULL) < 0) {
		perror("exec family error");
		close(accept_fd);
		exit(1);
	    }

	    close(accept_fd);
	    exit(0);
	} else if (f_ext == exe) {
	    int red;
	    if ((red = dup2(accept_fd, STDOUT_FILENO)) < 0) {
		perror("dup error");
		close(accept_fd);
		exit(1);
	    }
	    if ((red = dup2(accept_fd, STDERR_FILENO)) < 0) {
		perror("dup error");
		close(accept_fd);
		exit(1);
	    }
	    if (execlp(f_name, f_name, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, NULL) < 0) {
		perror("exec family error");
		close(accept_fd);
		exit(1);
	    }

	    close(accept_fd);
	    exit(0);
	} else if (f_ext == gif) {
	    write(accept_fd, F_GIF, strlen(F_GIF));

	    FILE *f_open;
	    char *f_cont = (char *) malloc(sizeof(char) * 10485760);
	    size_t f_size = f_stat.st_size;
	    char *con_len = (char *) malloc(sizeof(char) * 64);

	    sprintf(con_len, "Content-Length: %d\r\n", (int) f_size);
	    write(accept_fd, con_len, strlen(con_len));
	    write(accept_fd, "\r\n", 2);

	    if ((f_open = fopen(f_name, "r")) == NULL) {
		perror("fopen error");
		close(accept_fd);
		exit(1);
	    }

	    fread(f_cont, sizeof(char), f_size, f_open);
	    write(accept_fd, f_cont, f_size);
	    close(accept_fd);
	    exit(0);
	} else if (f_ext == html) {
	    write(accept_fd, F_HTML, strlen(F_HTML));

	    FILE *f_open;
	    char *f_cont = (char *) malloc(sizeof(char) * 1048576);
	    size_t f_size = f_stat.st_size;
	    char *con_len = (char *) malloc(sizeof(char) * 64);

	    sprintf(con_len, "Content-Length: %d\r\n", (int) f_size);
	    write(accept_fd, con_len, strlen(con_len));
	    write(accept_fd, "\r\n", 2);

	    if ((f_open = fopen(f_name, "r")) == NULL) {
		perror("fopen error");
		close(accept_fd);
		exit(1);
	    }

	    fread(f_cont, sizeof(char), f_size, f_open);
	    write(accept_fd, f_cont, f_size);
	    close(accept_fd);
	    exit(0);
	} else if (f_ext == jpeg) {
	    write(accept_fd, F_JPEG, strlen(F_JPEG));

	    FILE *f_open;
	    char *f_cont = (char *) malloc(sizeof(char) * 10485760);
	    size_t f_size = f_stat.st_size;
	    char *con_len = (char *) malloc(sizeof(char) * 64);

	    sprintf(con_len, "Content-Length: %d\r\n", (int) f_size);
	    write(accept_fd, con_len, strlen(con_len));
	    write(accept_fd, "\r\n", 2);

	    if ((f_open = fopen(f_name, "r")) == NULL) {
		perror("fopen error");
		close(accept_fd);
		exit(1);
	    }

	    fread(f_cont, sizeof(char), f_size, f_open);
	    write(accept_fd, f_cont, f_size);
	    close(accept_fd);
	    exit(0);
	} else if (f_ext == jpg) {
	    write(accept_fd, F_JPG, strlen(F_JPG));

	    FILE *f_open;
	    char *f_cont = (char *) malloc(sizeof(char) * 10485760);
	    size_t f_size = f_stat.st_size;
	    char *con_len = (char *) malloc(sizeof(char) * 64);

	    sprintf(con_len, "Content-Length: %d\r\n", (int) f_size);
	    write(accept_fd, con_len, strlen(con_len));
	    write(accept_fd, "\r\n", 2);

	    if ((f_open = fopen(f_name, "r")) == NULL) {
		perror("fopen error");
		close(accept_fd);
		exit(1);
	    }

	    fread(f_cont, sizeof(char), f_size, f_open);
	    write(accept_fd, f_cont, f_size);
	    close(accept_fd);
	    exit(0);
	} else if (f_ext == plain) {
	    write(accept_fd, F_TXT, strlen(F_TXT));

	    FILE *f_open;
	    char *f_cont = (char *) malloc(sizeof(char) * 1048576);
	    size_t f_size = f_stat.st_size;
	    char *con_len = (char *) malloc(sizeof(char) * 64);

	    sprintf(con_len, "Content-Length: %d\r\n", (int) f_size);
	    write(accept_fd, con_len, strlen(con_len));
	    write(accept_fd, "\r\n", 2);

	    if ((f_open = fopen(f_name, "r")) == NULL) {
		perror("fopen error");
		close(accept_fd);
		exit(1);
	    }

	    fread(f_cont, sizeof(char), f_size, f_open);
	    write(accept_fd, f_cont, f_size);
	    close(accept_fd);
	    exit(0);
	}
    }

    close(accept_fd);
    exit(0);
}

int main(int argc, char *argv[]) {
    int port, socket_fd, accept_fd;
    struct sockaddr_in server, client;

    if (argc > 2) {
	perror("too many arguments");
	exit(1);
    } else if (argc < 2) {
	perror("too few arguments");
	exit(1);
    } else {
	sscanf(argv[1], "%d", &port);
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
	perror("socket error");
	exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *) &server, sizeof(server)) < 0) {
	perror("bind error");
	exit(1);
    }

    listen(socket_fd, 8);

    pid_t pid;
    socklen_t client_size = sizeof(client);
    while ((accept_fd = accept(socket_fd, (struct sockaddr *) &client, (socklen_t *) &client_size)) > 0) {
	if ((pid = fork()) < 0) {
	    perror("fork error");
	    exit(1);
	}

	if (pid == 0) {
	    close(socket_fd);
	    parse(accept_fd);
	    exit(0);
	}
	close(accept_fd);
    }

    return 0;
}
