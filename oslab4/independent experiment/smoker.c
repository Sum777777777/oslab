#include "ipc.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <smoker_id 0-2>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int smoker_id = atoi(argv[1]);
    int rate = argc > 2 ? atoi(argv[2]) : 1; // 默认1秒
    
    // 初始化共享内存
    shm_key = 1234;
    shm_id = shmget(shm_key, sizeof(struct shared_data), 0666);
    if (shm_id == -1) {
        perror("smoker: shmget failed");
        exit(EXIT_FAILURE);
    }
    shm_ptr = (struct shared_data *)shmat(shm_id, NULL, 0);
    
    // 获取信号量
    supplier_sem_key = 5678;
    done_sem_key = 5679;
    mutex_sem_key = 5680;
    smoker_sem_keys[0] = 5681;
    smoker_sem_keys[1] = 5682;
    smoker_sem_keys[2] = 5683;
    
    supplier_sem = set_sem(supplier_sem_key, 0, 0666);
    done_sem = set_sem(done_sem_key, 0, 0666);
    mutex_sem = set_sem(mutex_sem_key, 1, 0666); // 互斥信号量初始为1
    smoker_sems[0] = set_sem(smoker_sem_keys[0], 0, 0666);
    smoker_sems[1] = set_sem(smoker_sem_keys[1], 0, 0666);
    smoker_sems[2] = set_sem(smoker_sem_keys[2], 0, 0666);
    
    // 根据吸烟者ID等待对应材料组合
    while (1) {
        // 等待供应者提供的信号量
        printf("Smoker %d is waiting...\n", smoker_id);
        down(smoker_sems[smoker_id]);  // 阻塞等待自己的信号量
        
        // 获取互斥锁访问共享内存
        down(mutex_sem);
        
        // 验证是否是正确的组合
        int combination = shm_ptr->combination;
        int valid = 0;
        switch (smoker_id) {
            case 0:  // 拥有烟草，需要纸+胶水（组合1）
                valid = (combination == 1);
                break;
            case 1:  // 拥有纸，需要烟草+胶水（组合3）
                valid = (combination == 3);
                break;
            case 2:  // 拥有胶水，需要烟草+纸（组合2）
                valid = (combination == 2);
                break;
        }
        
        if (valid) {
            // 制作并吸烟
            printf("Smoker %d is making a cigarette with combination %d\n", 
                  smoker_id, combination);
            sleep(rate);  // 模拟制作过程
            printf("Smoker %d finished smoking\n", smoker_id);
            
            // 通知供应者已完成
            up(done_sem);
        } else {
            // 组合不匹配，可能是虚假唤醒，重新等待
            up(smoker_sems[smoker_id]);  // 恢复信号量
        }
        
        up(mutex_sem);  // 释放互斥锁
    }
    
    shmdt(shm_ptr);
    return 0;
}
