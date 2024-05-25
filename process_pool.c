#include <stdio.h>

#include <unistd.h>
#include <stdlib.h>

#include <string.h>
#include <sys/wait.h>

#include <mqueue.h>


typedef struct {
    int worker_num; // 进程数
    char mq_key[20]; // 消息队列的key
    char subProcessFile[50]; // 子进程的文件路径
    
} ps_pool;

void recv_client(ps_pool *pool);

ps_pool pool;


int main() {
    sprintf(pool.mq_key, "%s", "/thread_pool");
    pool.worker_num = 2; // 进程的数目
    sprintf(pool.subProcessFile, "%s", "../thread-pool/th_pool");

    int pid = fork();
    if (pid == 0) {
        // 子进程
        for (int i = 0; i < pool.worker_num; i++) {
            pid = fork();
            if (pid == 0) { // 子进程 
                char para[30]; // 传递的参数
                sprintf(para, "%s-%d", pool.mq_key, i);
                execlp(pool.subProcessFile, para, NULL);
                
                // sleep(2);
                printf("子进程,结束 %d\n", i);

            } else {
                // exit(0);
            }
        }
    } else {
        // 父进程
        recv_client(&pool);

        printf("父进程,结束");
    }

}

// 接收,客户端的发送
void recv_client(ps_pool *pool) {
    int i = 0;
    // 初始化消息队列
    int mqId = mq_open(pool->mq_key, O_CREAT | O_RDWR, 0777, NULL);
    if (mqId == -1) {
        perror("mq_open ");
        return ;
    }
    char recvData[10240] = {'\0'};

    int subNum = 0;

    while (1) {
        printf("I wait msg\n");
        int ret = mq_receive(mqId, recvData, sizeof(recvData), 0);
        printf("I recv %s\n", recvData);
        if (ret == -1) {
            perror("mq_receive ");
            return ;
        }

        
        char subMq_key[30];
        sprintf(subMq_key, "%s-%d", pool->mq_key, subNum);
        
        // 发给,任务进程
        int subMqId = mq_open(subMq_key, O_CREAT | O_RDWR, 0777, NULL);
        if (subMqId == -1) {
            perror("mq_open ");
            return ;
        }

        ret = mq_send(subMqId, recvData, strlen(recvData), 0);
        if (ret == -1) {
            perror("send fail");  // 发送失败
        } else {
            printf("发送成功\n");
        }
        mq_close(subMqId);
        subNum++;

    }





}
