#!/bin/bash
#First you can use grep (-n) to find the number of lines of string.
#Then you can use awk to separate the answer.

# 1. 获取源文件 需要查找的字符串 新生成的文件
srcFile="$1"
targetStr="$2"
newFile="$3"

# 2. 使用grep查找内容
grep -n "$targetStr" "$srcFile" | awk -F: '{print $1}'  > "$newFile"
