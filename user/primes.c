#include <kernel/types.h>
#include <user/user.h>


void child_pro(int p[]) {
    int x,y;
    // 用于与子进程通信的管道
    int child_p[2];
    // 从父进程读取，所以关闭与父进程连接管道的写入端
    close(p[1]);
    // 从父进程读取成功之后的逻辑
    if (read(p[0],&x,sizeof(int))) {
        // 打印读取成功后的素数
        // 上一层中传递的第一个值一定是素数，并且依据这个值筛选素数
        fprintf(1,"prime %d\n",x);
        // 创建与子进程通信的管道
        pipe(child_p);
        // 再次使用fork，当自身为父进程时
        if (fork() != 0) {
            // 关闭与子进程通信的读取端
            close(child_p[0]);
            // 从父进程持续读取，读取的数字不为第一个数字的倍数时，持续向子进程写入
            while (read(p[0], &y, sizeof(int))) {
                if (y %x != 0) {
                    write(child_p[1], &y, sizeof(int));
                }
            }
            // 关闭从父进程的读取
            close(p[0]);
            // 关闭向子进程的写入
            close(child_p[1]);
            // 等待子进程完成
            wait(0);
        }
        // 创建子进程
        else {
            child_pro(child_p);
            }
    }
                   
    exit(0);
}



int main(){
    int p[2];
    pipe(p);
    if (fork()!=0) {
        close(p[0]);// 关闭第一个管道的读取
        for(int i = 2;i<=35;i++){
            write(p[1],&i,sizeof(int));
        }// 主进程，将2到35写入第一个管道
        close(p[1]);// 关闭第一个管道的写入
        wait(0);// 等待子进程完成
        exit(0);
    }
    else {
        child_pro(p);// 执行子进程
    }
    exit(0);
}
