#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* getfilename(char *path)
{
  char *p;
    
  //Find first character after last slash.
    // 从后向前遍历，找到并返回‘/’后的序列
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  return p;
}
void find(char *path, char* target)
{
  char buf[512], *p;
  int fd;
    // 目录项结构体
  struct dirent de;
    // 文件统计信息
  struct stat st;
  // 当打开失败的时候,报错
  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
    //fstat()用来将参数fildes所指的文件状态,复制到参数buf所指的结构中(struct stat)执行成功则返回0,失败返回-1,错误代码存于errno
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
    //判断打开的类型
  switch(st.type){
  // 打开的是文件
  case T_FILE:
          // 打开的文件与目标匹配，输出
    if (strcmp(getfilename(path), target) == 0) {
      printf("%s\n", path);
    }
    break;
          // 打开的是路径
  case T_DIR:
          // 判断是否溢出
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
          // 把path拷贝到buf里
    strcpy(buf, path);
          // 指向buf的末尾
    p = buf+strlen(buf);
    *p++ = '/';
          // 每次读取一个目录
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        // 文件夹下无文件
      if(de.inum == 0) {
          // 进行下一次读取
        continue;
      }
      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) { // 跳过子目录的.和..
        continue;
      }
        // 将de.name中的内容移动到p指针所指向的位置，拼接路径字符串
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      find(buf, target);
    }
    break;
  }
  close(fd);
}
int main(int argc, char *argv[])
{
  // 两个参数,起始路径和目标文件
  if(argc < 3){
    fprintf(2, "find: need 2 args\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
