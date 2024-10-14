#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

struct file_info {
    char *name;
    time_t mod_time;
    off_t size;
    mode_t permissions;
};

int compare_mod_time(const void *a, const void *b) {
    struct file_info *file_a = (struct file_info *)a;
    struct file_info *file_b = (struct file_info *)b;
    return difftime(file_b->mod_time, file_a->mod_time);
}

void process_options(char *arg, int *l_flag, int *a_flag, int *t_flag) {
    for (int i = 1; i < strlen(arg); i++) {
        if (arg[i] == 'l') *l_flag = 1;
        else if (arg[i] == 'a') *a_flag = 1;
        else if (arg[i] == 't') *t_flag = 1;
    }
}

int main(int argc, char *argv[]) {
    struct dirent *de;
    DIR *dr;
    int l_flag = 0, a_flag = 0, t_flag = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            process_options(argv[i], &l_flag, &a_flag, &t_flag);
        }
    }

    dr = opendir(".");
    if (dr == NULL) {
        perror("opendir");
        return 1;
    }

    struct file_info *file_list = NULL;
    int capacity = 10, count = 0;
    file_list = malloc(capacity * sizeof(struct file_info));

    while ((de = readdir(dr)) != NULL) {
        if (!a_flag && de->d_name[0] == '.') continue;

        if (count >= capacity) {
            capacity *= 2;
            file_list = realloc(file_list, capacity * sizeof(struct file_info));
        }

        struct stat fileStat;
        if (stat(de->d_name, &fileStat) < 0) {
            perror("stat");
            continue;
        }

        file_list[count].name = strdup(de->d_name);
        file_list[count].mod_time = fileStat.st_mtime;
        file_list[count].size = fileStat.st_size;
        file_list[count].permissions = fileStat.st_mode & 0777;
        count++;
    }
    closedir(dr);

    if (t_flag) {
        qsort(file_list, count, sizeof(struct file_info), compare_mod_time);
    }

    for (int i = 0; i < count; i++) {
        if (l_flag) {
            printf("Size: %ld\tPermissions: %o\tLast Modified: %s\tName: %s\n", 
                   file_list[i].size, 
                   file_list[i].permissions, 
                   ctime(&file_list[i].mod_time), 
                   file_list[i].name);
        } else {
            printf("%s\n", file_list[i].name);
        }
        free(file_list[i].name);
    }
    free(file_list);

    return 0;
}

