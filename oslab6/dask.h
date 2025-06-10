#ifndef DISK_ARM_H
#define DISK_ARM_H

#include <iostream>
#include <iomanip>
#include <malloc.h>
#include <algorithm>
#include <climits>
#include <ctime>

class DiskArm {
public:
    DiskArm();
    ~DiskArm();
    void InitSpace(char* MethodName); //初始化寻道记录
    void Report(); //报告算法执行情况
    void Fcfs();  // 添加这行
    void Sstf();  // 添加这行
    void Scan();  // 添加这行
    void CScan(); // 添加这行
    void Look();
    void GenerateRandomRequests(int count, int maxCylinder);
private:
    int *Request; //磁盘请求道号
    int *Cylinder; //工作柱面道号
    int RequestNumber; //磁盘请求数
    int CurrentCylinder; //当前道号
    int SeekDirection; //磁头方向
    int SeekNumber; //移臂总数
    int SeekChang; //磁头调头数
};

#endif

