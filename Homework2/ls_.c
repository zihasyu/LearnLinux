#include <stdio.h>
// #include <stdlib.h>
// #include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

// 打印文件的元数据信息，包括文件类型、权限和大小。
void print_file_info(struct dirent *de, struct stat st)
{
    printf((S_ISDIR(st.st_mode)) ? "d" : "-");  // 检查文件类型 是否是目录
    printf((st.st_mode & S_IRUSR) ? "r" : "-"); // 检查文件权限 S_IRUSR：用户读权限
    printf((st.st_mode & S_IWUSR) ? "w" : "-"); // S_IWUSR：用户写权限
    printf((st.st_mode & S_IXUSR) ? "x" : "-"); // S_IXUSR：用户执行权限
    printf((st.st_mode & S_IRGRP) ? "r" : "-"); // S_IRGRP：组读权限
    printf((st.st_mode & S_IWGRP) ? "w" : "-"); // S_IWGRP：组写权限
    printf((st.st_mode & S_IXGRP) ? "x" : "-"); // S_IXGRP：组执行权限
    printf((st.st_mode & S_IROTH) ? "r" : "-"); // S_IROTH：其他读权限
    printf((st.st_mode & S_IWOTH) ? "w" : "-"); // S_IWOTH：其他写权限
    printf((st.st_mode & S_IXOTH) ? "x" : "-"); // S_IXOTH：其他执行权限
    printf(" %ld ", st.st_size);                // 文件大小
    printf(" %s\n", de->d_name);                // 文件名
}

int main()
{
    DIR *dr = opendir("."); // 打开当前目录
    if (dr == NULL)
    {
        printf("Could not open current directory"); // 错误处理
        return 0;
    }

    struct dirent *de; // 用于存储目录中的文件
    while ((de = readdir(dr)) != NULL)
    {
        struct stat st; // 用于存储文件的元数据信息
        if (de->d_name[0] == '.' || (de->d_name[0] == '.' && de->d_name[1] == '.'))
        {
            continue;
        }
        if (stat(de->d_name, &st) == 0)
        {
            print_file_info(de, st);
        }
    }

    closedir(dr); // 关闭目录
    return 0;
}