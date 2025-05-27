#!/bin/bash

# 清理旧对象
ipcrm -a
rm -f supplier smoker

# 编译
gcc ipc.c supplier.c -o supplier
gcc ipc.c smoker.c -o smoker

# 设置权限
chmod +x supplier smoker

echo "编译完成，执行以下命令启动："
echo "终端1: ./supplier"
echo "终端2: ./smoker 0"
echo "终端3: ./smoker 1"
echo "终端4: ./smoker 2"
