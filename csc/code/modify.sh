# 1. 获得参数
srcFile="$1"
oldStr="$2"
newStr="$3"

# 2. 替换本文
sed -i "s/\b$oldStr\b/$newStr/g" "$srcFile"
