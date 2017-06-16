#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

void error(char * msg)
{
    perror(msg);
    exit(-1);
}
void add_file_type(struct stat *  mystat)
{
    S_ISDIR(mystat->st_mode) ? printf("d") : printf("-"); 
}
void add_owner_perm(struct stat * mystat)
{
    mystat->st_mode & S_IRUSR ? printf("r") : printf("-");
    mystat->st_mode & S_IWUSR ? printf("w") : printf("-");
    mystat->st_mode & S_IXUSR ? printf("x") : printf("-");   
}
void add_group_perm(struct stat * mystat)
{
    mystat->st_mode & S_IRGRP ? printf("r") : printf("-");
    mystat->st_mode & S_IWGRP ? printf("w") : printf("-");
    mystat->st_mode & S_IXGRP ? printf("x") : printf("-"); 
}
void add_other_perm(struct stat * mystat)
{
    mystat->st_mode & S_IROTH ? printf("r") : printf("-");
    mystat->st_mode & S_IWOTH ? printf("w") : printf("-");
    mystat->st_mode & S_IXOTH ? printf("x") : printf("-");
}
void add_space()
{
    printf(" ");
}
void add_mode_str(struct stat * mystat)
{
    add_file_type(mystat);
    add_owner_perm(mystat);
    add_group_perm(mystat);
    add_other_perm(mystat);
    add_space();
}
void add_num_links(struct stat * mystat)
{
    int n = mystat->st_nlink;
    printf("%d", n);
    add_space();
}
void add_size(struct stat * mystat)
{
    int n = mystat->st_size;
    printf("%d", n);
    add_space();
}
void add_name(struct dirent * myFile)
{
    printf("%s", myFile->d_name);
}
void add_time(struct stat * mystat)
{
    time_t time = mystat->st_mtime;
    char buf[40];
    strftime(buf, 39, "%b %d %H:%M ", localtime(&time));
    printf("%s", buf); 
}
void add_owner()
{
    struct passwd * p;
    p = getpwuid(getuid());
    printf("%s ", p->pw_name);
}
void add_group()
{
    struct group * g;
    g = getgrgid(getuid());
    printf("%s ", g->gr_name);
}
void add_mode_links_own_grp(struct stat * mystat)
{
    add_mode_str(mystat);
    add_num_links(mystat);
    add_owner();
    add_group();
}
void add_size_time_name(struct dirent ** myFile, struct stat * mystat)
{
    add_size(mystat);
    add_time(mystat);
    add_name(*myFile);
    printf("\n");
}
void make_full_path(char * path, char * dirname, char * filename)
{
    strcat(path, dirname);
    if(path[strlen(path)-1] != '/')
	strcat(path, "/");
    strcat(path, filename);
}
void make_indent(int indent)
{
    for(int i = 0; i < indent; ++i)
	printf("\t");
}
void make_path(char * filepath, char * path, struct dirent * myFile)
{
    strcpy(filepath, "");
    make_full_path(filepath, path, myFile->d_name);
}
void list_files(char * path, int indent)
{
    DIR * dirp;
    struct dirent * myFile;
    struct stat mystat;
    char filepath[256] = "";
    if((dirp = opendir(path)) == NULL )
	error("opendir");
    while((myFile = readdir(dirp)) != NULL )
    {
        if(strcmp(myFile->d_name, ".") == 0 || strcmp(myFile->d_name, "..") == 0 || strncmp(myFile->d_name, ".", 1) == 0)
	    continue;
	make_indent(indent);
	make_path(filepath, path, myFile);
	if( stat(filepath, &mystat) == -1  )
	    error("stat");
	add_mode_links_own_grp(&mystat);
	add_size_time_name(&myFile, &mystat);
	if(S_ISDIR(mystat.st_mode))
	    list_files(filepath, ++indent);
    }
}
int main(int argc, char * argv[])
{
    int indent = 0;
    if(argc == 1)
	list_files(".", indent);
    else
    {
        for(int i = 1; i < argc; ++i)
        {
	    list_files(argv[i], indent);
        }
    } 
    return 0;
}
