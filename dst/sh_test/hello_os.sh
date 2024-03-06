#!/bin/bash

# 1. 获得文件路径
src="$1"
des="$2"

# 2. 新建BBB文件
touch "$des"

# 3. 设置BBB文件内容
awk 'NR==8 || NR==32 || NR==128 || NR==512 || NR==1024 {print}' "$src" > "$des"
