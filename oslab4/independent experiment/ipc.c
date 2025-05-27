#include "ipc.h"
#include <string.h>

key_t shm_key;
int shm_id;
struct shared_data *shm_ptr;

key_t supplier_sem_key;
key_t done_sem_key;
key_t mutex_sem_key;
key_t smoker_sem_keys[3];

int supplier_sem;
int done_sem;
int mutex_sem;
int smoker_sems[3];

int get_ipc_id(char *proc_file, key_t key) {
    FILE *pf;
    int i, j;
    char line[BUFSZ], colum[BUFSZ];
    if ((pf = fopen(proc_file, "r")) == NULL) {
        perror("Proc file not open");
        exit(EXIT_FAILURE);
    }
    fgets(line, BUFSZ, pf);
    while (!feof(pf)) {
        fgets(line, BUFSZ, pf);
        i = j = 0;
        while (line[i] == ' ') i++;
        while (line[i] != ' ') colum[j++] = line[i++];
        colum[j] = '\0';
        if (atoi(colum) != key) continue;
        j = 0;
        while (line[i] == ' ') i++;
        while (line[i] != ' ') colum[j++] = line[i++];
        colum[j] = '\0';
        fclose(pf);
        return atoi(colum);
    }
    fclose(pf);
    return -1;
}

char *set_shm(key_t key, int size, int flag) {
    int id;
    char *shm_buf;
    if ((id = get_ipc_id("/proc/sysvipc/shm", key)) < 0) {
        if ((id = shmget(key, size, flag)) < 0) {
            perror("shmget");
            exit(EXIT_FAILURE);
        }
    }
    if ((shm_buf = (char *)shmat(id, 0, 0)) < (char *)0) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    return shm_buf;
}

int set_sem(key_t key, int val, int flag) {
    int id;
    union semun sem_arg;
    if ((id = get_ipc_id("/proc/sysvipc/sem", key)) < 0) {
        if ((id = semget(key, 1, flag)) < 0) {
            perror("semget");
            exit(EXIT_FAILURE);
        }
        sem_arg.val = val;
        if (semctl(id, 0, SETVAL, sem_arg) < 0) {
            perror("semctl");
            exit(EXIT_FAILURE);
        }
    }
    return id;
}

int down(int sem_id) {
    struct sembuf buf = {0, -1, SEM_UNDO};
    return semop(sem_id, &buf, 1);
}

int up(int sem_id) {
    struct sembuf buf = {0, 1, SEM_UNDO};
    return semop(sem_id, &buf, 1);
}
