1. 编写print.c文件
	增加%$1T'和'%$2T'
	1. 增加变量
	typedef struct { int a, char b, cahr c, int d; } s1;
	typedef struct { int size, int c[10000]; } s2;
	
	2. 增加工具
	int type 	// 判断输出哪种结构体
	s1 *a_s1;		// 两种结构体指针
	s2 *a_s2;		// 两种结构体指针
	
	3. 增加情况
	// 1就是结构体s1, 2就是结构体s2
	case 'T':
		switch(type){
			case '1':{
				a_s1 = (s1*)va_arg(ap, s1*);
				break;
			}
			case '2':{
				a_s2 = (s2*)va_arg(ap, s2*);
				break;
			}
		}
	// 2. 如果是s1，逐个输出
		2.1 	int a = a_s1->a;
			char b = a_s1->b;
		2.2	neg_flag = 0;
			if (a<0) {
				neg_flag = 1;
				a= = -a;
			}
			print_num(data,...)
			
print_char(data,...)
// 3. 如果是s2，遍历输出
		代码请参考学长的


2021：
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct s1 {
	int a;
	char b;
	char c;
	int d;
};

struct s2 {
	int size;
	int c[c_size];
};
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef struct {
        int a;
        char b;
        char c;
        int d;
} s1;

typedef struct {
        int size;
        int c[10000];
} s2;
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int type;
s1* a_s1;
s2* a_s2;
int var_for; // for 循环里面的参数
int size;
int* arr_int; // type in struct s2
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
case 'T':
        if (type == 1) {
                a_s1 = (s1*)va_arg(ap, s1*); // 保险起见，做了强制转换
        } else {
                a_s2 = (s2*)va_arg(ap, s2*);
        }
        // output the '{'
        length = PrintChar(buf, '{', 1, 0);
        OUTPUT(arg, buf, length);
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
if (type == 1) {
        // output int a, char b, char c, int d;
                int a = a_s1->a;
                char b = a_s1->b;
                char c = a_s1->c;
                int d = a_s1->d;
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// print int a;
negFlag = 0;
if (a < 0) {
        a = -a;
        negFlag = 1;
}
length = PrintNum(buf, a, 10, negFlag, width, ladjust, padc, 0);
OUTPUT(arg, buf, length);

// print ','
length = PrintChar(buf, ',', 1, 0);
OUTPUT(arg, buf, length);

// print char b
length = PrintChar(buf, b, width, ladjust);
OUTPUT(arg, buf, length);

// print ','
length = PrintChar(buf, ',', 1, 0);
OUTPUT(arg, buf, length);

// print char c
length = PrintChar(buf, c, width, ladjust);
OUTPUT(arg, buf, length);

// print ','
length = PrintChar(buf, ',', 1, 0);
OUTPUT(arg, buf, length);

// print int d
negFlag = 0;
if (d < 0) {
        d = -d;
        negFlag = 1;
}
length = PrintNum(buf, d, 10, negFlag, width, ladjust, padc, 0);
OUTPUT(arg, buf, length);
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
else {
    size = a_s2->size; // 拿到size
    arr_int = a_s2->c; // 拿到数组
    for (var_for = 0; var_for < size; var_for++) { // var_for 已经预先定义好了，为0
            int x = arr_int[var_for];
            negFlag = 0;
            if (x < 0) {
                    x = -x;
                    negFlag = 1;
            }
            length = PrintNum(buf, x, 10, negFlag, width, ladjust, padc, 0);
            OUTPUT(arg, buf, length);

            if (var_for != size-1) {
                    length = PrintChar(buf, ',', 1, 0);
                    OUTPUT(arg, buf, length);
            }

    }
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// print '}'
length = PrintChar(buf, '}', 1, 0);
OUTPUT(arg, buf, length);
break;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef struct {
        int a;
        char b;
        char c;
        int d;
} s1;

typedef struct {
        int size;
        int c[100];
} s2;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void mips_init()
{
        printf("init.c:\tmips_init() is called\n");

        s2 s = {3, {1,2,3}};
        printf("%$2T", &s);
        printf("\n");

