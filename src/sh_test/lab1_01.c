1. 编写print.c文件
        调用路径kern/printk.c
                        的 void printk( const *fmt , ...) {
                                va_list ap;
                                va_start( ap, fmt );
                                vprintfmt( outputk, NULL, fmt, ap );
                                va_end( ap );
                        在 lib/print.c中
                                在include系统头文件中
                                        由print.h文件
                                        print.h中声明了void vprintfmt(  fmt_callback_k out,
                                                                                                        void *data,
                                                                                                        const char *fmt,
                                                                                                        va_list ap );
                                在lib/print.c中
                                实现void vprintfmt(     fmt_callback_k out,
                                                                        void *data,
                                                                        const char *fmt,
                                                                        va_list ap ) { }
        编写print.c文件

        文件初始内容为对格式化输出的控制, 在lab1中完成了对%-0左右对齐等等的判断, 现在要新增一种格式情况%R
        %R用于把 1,2 输���成 (1,2)

        格式控制需要的参数有
        print_num(      不修改:         out函数, data环境, 
                                修改:   num要打印的数字, 
                                不修改: 10指定进制, 
                                修改:   neg_flag指定是否输出负数, 
                                不修改: width指定数字占位宽度, 
                                不修改: ladjust指定左对齐, 
                                不修改: padc指定填充字符, 
                                不修改: 0指定不转大写                   ）
        只需要指定num, 并根据正负性指定neg_flag
        需要调用函数:
                1. va_arg()
                        用于取变长参表中的参数
                        用于指定num数值
                        num = var_arg( 不修改:  ap,
                                                   修改:                int或int long 指明num的变量类型 )
                2. out()
                        用于直接打印, vprintfmt的打印都通过out实现
                        out( 不修改:    data,
                                 修改:  fmt 当前栈顶
                                 修改:  p-fmt 需要输出的长度 )
                3. print_num()
                        照抄
                        只需修改num, 正负性neg_flag

        // 1. 增加变量num1和num 2
        long num1, num2;
        // 2. 修改switch部分代码, 增加case 'R'的情况
        case 'R':
                // 1. 判断是��是long
                // 1. 然后指定num1和num2的数据和数据类型
                if (long_flag) {
                        num1 = va_arg( ap, int long );
                        num2 = va_arg( ap, int long );
                } else {
                        num1 = va_arg( ap, int );
                        num2 = va_arg( ap, int );
                }

                // 2. 打印"("
                // 2. out必须用字符串形式"(", 和*fmt保持一致
                out( data, "(", 1);

                // 3. 打印num1
                // 3. print_num需要考虑num和neg_flag
                // 3. 如果是负数, 要neg_flag取1, num取反
                if ( num1 < 0 ) {
                        neg_flag = 1;
                        num1 = - num1;
                }
                print_num( out, data, num1, 10, neg_flag, width, ladjust, padc, 0);

                // 4. 打印","
                out( data, ",", 1);

                // 5. 重置neg_flag
                在for(;;)中, 每打印一个新的%b,%d, 会在switch前对neg_flag=0
                打印%R的时候会连续打印两个数字, 要自己在case 'R'对neg_flag手动重置
                neg_flag = 0;

                // 6. 打印num2
                if ( num2 < 0 ) {
                        neg_flag = 1;
                        num2 = -num2;
                }
                print_num( out, data, num2, 10, neg_flag, width, ladjust, padc, 0);

                // 7. 打印")"
                out( data, ")", 1);
                break;



2. 编写string.c文件
        调用路径tests/lab1_sprint/init.c
                的void sprintf_1_check() {
                                char str[100];
                                sprintf(str, "%d\n", 12321);
                                printk("%s", str);
                                sprintf(str, "%c\n", 97);
                                printk("%s", str);
                        }
                在 lib/string.c中
                        在include系统头文件中
                                由string.h文件
                                string.h中声明了void sprintf(   char *buf,
                                                                                        const char *fmt,
                                                                                        ... );
                        在lib/string.c中
                        实现int sprintf( char *buf,
                                                        const char *fmt,
                                                        ... ) { }
                        sprintf函数要返回字符串buf的长度, 因此用int, 最后return strlen(buf)
        编写string.c文件

1. 检查 + 声明头文件
        在tests/lab1_sprint/init.c的sprintf_1_check()方法中使用了sprintf()方法
        检查include目录下是否用string.h
        检查string.h中是否有sprintf()方法
        题目要求sprintf方法要返回传入字符串长度, 所以函数返回值是int
        int sprintf( char *buf, const char *fmt, ... );
2. 编写string.c文件
        // 1. 编写函数 + 返回buf字符串长度
        int sprintf( char *buf, const char *fmt, ... ) {

                return strlen(buf);
        }

        // 2. 由于sprintf中有可变长参数列表..., 因此模仿printk( const *fmt, ... )写法
        // 2. 需要va_list, va_start, 调用vprintfmt, va_end
                int sprintf( char *buf, const char *fmt, ... ) {
                        va_list ap;
                va_start( ap, fmt );
                vprintfmt( stroutputk, buf, fmt, ap );
                va_end( ap );
                        return strlen(buf);
                }

        // 3. vprintfmt中的回调函数stroutputk需要再string.c中自己完成
        // 3. 因为vprintfmt中的回调函数outputk也是在kern/printk.c中自己完成的
        // 3. 也就是printk和sprintf都用了lib/print.c中的vprintfmt
        // 3. kern/printk.c需要自己完成回调函数outputk, 传递给vprintfmt
        // 3. lib/string.c需要自己完成回调函数stroutputk, 传递给vprintfmt
        // 3. 所以要引用print.c的头文件, 用于调用vprintfmt
        #include <print.h>

        // 4. 编写回调函数stroutputk()
        由于kern/printk.c中outputk��接在kern/printk.c中完成, 没有在include/printk.h中声明
        这里也直接在lib/string.c中完成stroutputk
        void stroutputk( )

        参考kern/printk.c中的outputk
        void outputk(void *data, const char *buf, size_t len) {
                for (int i = 0; i < len; i++) {
                        printcharc(buf[i]);
                }
        }

        void stroutputk( void *data, const char *buf, size_t len ) {
        }

        kern/printk.c的printk调用vprintfmt后, 靠回调函数out直接完成输出
        lib/string.c的sprintf调用vprintfmt后, 靠回调函数out改变环境data值
        再用过init.c中的printk( "%s", str )真正打印
        因此我们写的回调函数stroutputk不需要printcharc, 而是改变data的值
        void stroutputk( void *data, const char *buf, size_t len ) {
                int temp=0;
                for( int i = 0; i < len; i++, temp++ ) {
                        data[temp]= buf[i];
                }
                data[temp]='\0';
        }

        综上:
        1. 修改include/string.h
                int sprintf(    char *buf,
                                        const char *fmt,
                                        ... );
        2. 修改lib/string.c
                #include <print.h>

                int sprintf(    char *buf,
                                        const char *fmt,
                                        ... ) {
                va_list ap;
                va_start( ap, fmt );
                vprintfmt( stroutputk, buf, fmt, ap ); 
                                // 用于改变环境data, 而非直接输出
                                // 在printk的vprintfmt中data位用NULL
                                // 在sprintf的vprintfmr中data用buf
                                // 因为在init.c中sprintf( str, "%d\n", 12321); 
                                        str作了buf, 也就是, buf存储了数字12321转字符串之后的值
                va_end( ap );
                return stlen(buf);
        }
3. 修改lib/string.c
        完成stroutputk方法, 用于修改data位的buf
        void stroutputk( void *data, const char *input, size_t len) {
                // data待装载
                // input就是取出参数
                // len就是指定了要输出的长度
                int temp = 0;
                for( int i = 0; i < len ; i++, temp++ ) {
                        data[i] = input[i];
                }
                data[temp] = '\0';
        }
4. 修正
4.1 stroutputk内必须用char *str = (char *)data进行强制类型转换
4.2 必须使用全局静态变量static int str_idx = 0;
	至于strlen的问题会通过'\0'解决
4.3 stroutputk的三个参数分别是void *data, const char *buf, size_t len
const char *buf这里指向的是从参数列表里得到的东西(char *)va_arg(ap, char *)，也就是sprintf(str, "%s",112233);的112233部分
之前的const char *fmt指向的是*fmt，也就是printk("%d",2);的"%d"部分


# lab1代码

第一处lib/print.c的12行: 
        long num1, num2;
第二处lib/print.c的74行:
        case 'R':
                if (long_flag) {
                        num1 = va_arg(ap, long int);
                        num2 = va_arg(ap, long int);
                } else {
                        num1 = va_arg(ap, int);
                        num2 = va_arg(ap, int);
                }

                out(data, "(", 1);

                if (num1 < 0) {
                        neg_flag = 1;
                        num1 = -num1;
                }

                print_num(out, data, num1, 10, neg_flag, width, ladjust, padc, 0);

                out(data, ",", 1);

                neg_flag = 0;

                if (num2 < 0) {
                        neg_flag = 1;
                        num2 = -num2;
                }

                print_num(out, data, num2, 10, neg_flag, width, ladjust, padc, 0);

                out(data, ")", 1);

                break;

第一处include/string.h的15行:
        int sprintf(char *buf, const char *fmt, ...);
第二处lib/string.c的第2行:
        #include <print.h>
第三处lib/stirng.c的101行:
        static int str_idx = 0;
        void stroutputk(void *data, const char *buf, size_t len) {
                char *str = (char *)data;
                for (int i = 0; i < len; str_idx++, i++) {
                        str[str_idx] = buf[i];
                }
                str[str_idx] = '\0';
        }

        int sprintf(char *buf, const char *fmt, ...) {
                va_list ap;
                va_start(ap, fmt);
                str_idx = 0;
                vprintfmt(stroutputk, buf, fmt, ap);
                va_end(ap);
                return strlen(buf);
        }

