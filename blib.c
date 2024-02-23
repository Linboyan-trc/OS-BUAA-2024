#include <blib.h>

size_t strlen(const char *s) {
    const char *p = s;
    while (*p != '\0') {
        p++;
    }
    return p - s;
}

char *strcpy(char *dst, const char *src) {
    char *res = dst;
    while ((*dst++ = *src++))
        ;
    return res;
}

char *strncpy(char *dst, const char *src, size_t n) {
    char *res = dst;
    while (*src && n--) {
        *dst++ = *src++;
    }
    *dst = '\0';
    return res;
}

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n--) {
        if (*s1 != *s2) {
            return *s1 - *s2;
        }
        if (*s1 == 0) {
            break;
        }
        s1++;
        s2++;
    }
    return 0;
}

char *strcat(char *dst, const char *src) {
    char *p = dst;
    // 移动到目标字符串的末尾
    while (*p != '\0') {
        p++;
    }
    // 将源字符串逐个字符追加到目标字符串末尾
    while (*src != '\0') {
        *p = *src;
        p++;
        src++;
    }
    // 添加字符串结束符
    *p = '\0';
    return dst;
}

char *strncat(char *dst, const char *src, size_t n){
    char *p = dst;
    // 移动到目标字符串的末尾
    while (*p != '\0') {
        p++;
    }
    // 将源字符串的最多 n 个字符逐个追加到目标字符串末尾
    while (*src != '\0' && n > 0) {
        *p = *src;
        p++;
        src++;
        n--;
    }
    // 添加字符串结束符
    *p = '\0';
    return dst;
}

char *strchr(const char *str, int character){
    while (*str != '\0') {
        if (*str == (char)character) {
            return (char *)str;
        }
        str++;
    }

    return NULL;
}

char* strsep(char** stringp, const char* delim){
    if (*stringp == NULL) {
        return NULL;  // 如果字符串指针为 NULL，直接返回 NULL
    }

    char* start = *stringp;
    char* end = *stringp;

    // 在字符串中查找分隔符的位置
    while (*end != '\0' && strchr(delim, *end) == NULL) {
        end++;
    }

    // 如果找到分隔符，则用 '\0' 替换它，并更新 *stringp 指向下一个位置
    if (*end != '\0') {
        *end = '\0';
        *stringp = end + 1;
    } else {
        *stringp = NULL;  // 如果未找到分隔符，将 *stringp 设置为 NULL
    }

    return start;
}


void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

void *memcpy(void *out, const void *in, size_t n) {
    char *csrc = (char *)in;
    char *cdst = (char *)out;
    for (size_t i = 0; i < n; i++) {
        cdst[i] = csrc[i];
    }
    return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *p1 = s1, *p2 = s2;
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++, p2++;
    }
    return 0;
}
