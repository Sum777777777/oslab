#ifndef IPC_H
#define IPC_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define BUFSZ 256

// 共享内存数据结构
struct shared_data {
    int combination; // 1: 烟草+纸, 2: 纸+胶水, 3: 烟草+胶水
    int turn;
};

// 信号量联合体
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// 函数原型
int get_ipc_id(char *proc_file, key_t key);
char *set_shm(key_t shm_key, int shm_size, int shm_flag);
int set_sem(key_t sem_key, int sem_val, int sem_flag);
int down(int sem_id);
int up(int sem_id);

// 共享内存键值
extern key_t shm_key;
extern int shm_id;
extern struct shared_data *shm_ptr;

// 信号量键值和ID
extern key_t supplier_sem_key;
extern key_t done_sem_key;
extern key_t mutex_sem_key;
extern key_t smoker_sem_keys[3];

extern int supplier_sem;
extern int done_sem;
extern int mutex_sem;
extern int smoker_sems[3];

#endif
