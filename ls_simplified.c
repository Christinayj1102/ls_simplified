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
};

// 파일 정보를 출력하는 함수
void print_file_info(char *filename) {
    struct stat fileStat;
    if (stat(filename, &fileStat) < 0) {
        perror("stat");
        return;
    }
    printf("Size: %ld\tPermissions: %o\tLast Modified: %s\tName: %s\n", 
           fileStat.st_size, 
           fileStat.st_mode & 0777, 
           ctime(&fileStat.st_mtime), 
           filename);
}

// 파일 목록을 최신순으로 정렬하는 함수
int compare_mod_time(const void *a, const void *b) {
    struct file_info *file_a = (struct file_info *)a;
    struct file_info *file_b = (struct file_info *)b;
    return difftime(file_b->mod_time, file_a->mod_time);  // 최신순 정렬
}

// 옵션 문자열에서 플래그 설정
void process_options(char *arg, int *l_flag, int *a_flag, int *t_flag) {
    for (int i = 1; i < strlen(arg); i++) {
        if (arg[i] == 'l') {
            *l_flag = 1;
        } else if (arg[i] == 'a') {
            *a_flag = 1;
        } else if (arg[i] == 't') {
            *t_flag = 1;
        }
    }
}

int main(int argc, char *argv[]) {
    struct dirent *de;  // 디렉토리 엔트리
    DIR *dr;
    int l_flag = 0, a_flag = 0, t_flag = 0;

    // 옵션 처리
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            process_options(argv[i], &l_flag, &a_flag, &t_flag);  // 옵션 문자열 처리
        }
    }

    // 현재 디렉토리 열기
    dr = opendir(".");
    if (dr == NULL) {
        printf("Could not open current directory");
        return 1;
    }

    struct file_info file_list[256]; // 파일 목록 저장
    int count = 0;

    // 파일 읽기
    while ((de = readdir(dr)) != NULL) {
        // 숨김 파일 처리
        if (!a_flag && de->d_name[0] == '.') {
            continue;
        }

        struct stat fileStat;
        if (stat(de->d_name, &fileStat) < 0) {
            perror("stat");
            continue;
        }

        file_list[count].name = strdup(de->d_name); // 파일 이름 복사
        file_list[count].mod_time = fileStat.st_mtime; // 수정 시간 저장
        count++;
    }
    closedir(dr);

    // 최신순 정렬
    if (t_flag) {
        qsort(file_list, count, sizeof(struct file_info), compare_mod_time);
    }

    // 파일 출력
    for (int i = 0; i < count; i++) {
        if (l_flag) {
            print_file_info(file_list[i].name);
        } else {
            printf("%s\n", file_list[i].name);
        }
        free(file_list[i].name);
    }

    return 0;
}

