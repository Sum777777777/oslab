#include "ipc.h"
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rate = argc > 1 ? atoi(argv[1]) : 1;

    // 初始化共享内存
    shm_key = 1234;
    shm_id = shmget(shm_key, sizeof(struct shared_data), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    shm_ptr = (struct shared_data *)shmat(shm_id, NULL, 0);
    shm_ptr->combination = 0;
    shm_ptr->turn = 0;

    // 初始化信号量
    supplier_sem_key = 5678;
    supplier_sem = set_sem(supplier_sem_key, 1, 0666 | IPC_CREAT);
    done_sem_key = 5679;
    done_sem = set_sem(done_sem_key, 0, 0666 | IPC_CREAT);
    mutex_sem_key = 5680;
    mutex_sem = set_sem(mutex_sem_key, 1, 0666 | IPC_CREAT);
    smoker_sem_keys[0] = 5681;
    smoker_sem_keys[1] = 5682;
    smoker_sem_keys[2] = 5683;
    for (int i = 0; i < 3; i++) {
        smoker_sems[i] = set_sem(smoker_sem_keys[i], 0, 0666 | IPC_CREAT);
    }

   // 修改主循环逻辑
while (1) {
    down(supplier_sem);  // 等待生产许可
    down(mutex_sem);     // 获取互斥锁

    // 生产逻辑
    shm_ptr->turn = (shm_ptr->turn + 1) % 3;
    int combination = shm_ptr->turn + 1;
    shm_ptr->combination = combination;
    printf("Supplier provides materials %d\n", combination);

    // 触发对应吸烟者
    switch (combination) {
        case 1: up(smoker_sems[0]); break; // 纸+胶水 → smoker0
        case 2: up(smoker_sems[2]); break; // 烟草+纸 → smoker2
        case 3: up(smoker_sems[1]); break; // 烟草+胶水 → smoker1
    }

    up(mutex_sem);      // 释放互斥锁
    down(done_sem);     // 等待消费完成
    up(supplier_sem);   // !!! 关键修复：重置生产许可 !!!
    sleep(rate);
}

    return 0;
}
