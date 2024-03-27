#include <print.h>

/* forward declaration */
static void print_char(fmt_callback_t, void *, char, int, int);
static void print_str(fmt_callback_t, void *, const char *, int, int);
static void print_num(fmt_callback_t, void *, unsigned long, int, int, int, int, char, int);

void vprintfmt(fmt_callback_t out, void *data, const char *fmt, va_list ap) {
	char c;
	const char *s;
	long num;

	int width;
	int long_flag; // output is long (rather than int)
	int neg_flag;  // output is negative
	int ladjust;   // output is left-aligned
	char padc;     // padding char

	for (;;) {
		/* scan for the next '%' */
		/* Exercise 1.4: Your code here. (1/8) */
		// 1. 找到fmt所指字符串中的第一个%符号，并且字符串不能结束
		const char *p = fmt;
		while (*p != '%' && *p != '\0') {
			p++;
		}

		/* flush the string found so far */
		/* Exercise 1.4: Your code here. (2/8) */
		// 2. 对于非%部分的字符串，原样打印输出
		// 2. out的提供的回调函数，用于原样输出，需要传入环境值data，字符串起始位置fmt，字符串长度p-fmt
		// 2. 打印完毕后，fmt退栈，移到当前p位置，作为新的栈顶
		out(data, fmt, p - fmt);
		fmt = p;

		/* check "are we hitting the end?" */
		/* Exercise 1.4: Your code here. (3/8) */
		// 3. 查看字符串是否结束
		// 3. 如果结束，就break跳出for(;;)的恒循环
		if (*fmt == '\0') {
			break;
		}

		/* we found a '%' */
		/* Exercise 1.4: Your code here. (4/8) */
		// 4. 进行到这一步，说明当前fmt所指字符为%
		// 4. 移动fmt往前一步
		fmt++;

		/* check format flag */
		/* Exercise 1.4: Your code here. (5/8) */
		// 5.1 判断%[-0]后是否为-，如果为-就要左对齐，设置ladjust为1
		// 5.1 否则就是右对齐，保持ladjust=0
		// 5.2 判断%[-0]后是否为0，如果有的话，右对齐要用'0'，不然就用' '作为右对齐填充
		// 5.2 设置填充字符用' '还是'0'用变量padc记录
		ladjust = 0;
		padc = ' ';
		if (*fmt == '-') {
			ladjust = 1;
			fmt++;
		} else if (*fmt == '0') {
			padc = '0';
			fmt++;
		}

		/* get width */
		/* Exercise 1.4: Your code here. (6/8) */
		// 6. 读取是否指定了宽度
		width = 0;
		while ('0' <= *fmt && *fmt<= '9' && *fmt != '\0') {
			width *= 10; 			// 1. 进位，自己自乘10
			width += *fmt - '0';  	// 2. 加上新的个位
			fmt++;
		}

		/* check for long */
		/* Exercise 1.4: Your code here. (7/8) */
		// 7. 检查是否是输出long，如果是那么long_flag置1
		long_flag = 0;
		if (*fmt == 'l') {
			long_flag = 1;
			fmt++;
		}

		neg_flag = 0;
		switch (*fmt) {
		case 'b':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 2, 0, width, ladjust, padc, 0);
			break;

		case 'd':
		case 'D':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}

			/*
			 * Refer to other parts (case 'b', case 'o', etc.) and func 'print_num' to
			 * complete this part. Think the differences between case 'd' and the
			 * others. (hint: 'neg_flag').
			 */
			/* Exercise 1.4: Your code here. (8/8) */
			// 8. case到这个情况就是要以%d的格式输出一个整数
			// 8. 栈中存了printk("%d%c%ld,a,b,c)
			// 8. num是在栈中取到的参数，可能是a，b或c；这里匹配到了%d，所以此时num就是a
			// 8. 对于一个整数，首先要判断正负
			// 8. print_num函数只能接受非负数，所以如果num是负数，还要取反一下
			// 8. print_num需要的参数: out函数, data环境, 要打印的num
			// 8. print_num需要的参数: 10进制, 是否为负数neg_flag
			// 8. print_num需要的参数: 宽度width, 是否左对齐ladjust, 占位字符padc, 是否要转大写这里都是数字不涉及转大写的问题所以直接给0
			if (num < 0) {
				neg_flag = 1;
				num = -num;
			}

			print_num(out,data,num,10,neg_flag,width,ladjust,padc,0);

			break;

		case 'o':
		case 'O':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 8, 0, width, ladjust, padc, 0);
			break;

		case 'u':
		case 'U':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 10, 0, width, ladjust, padc, 0);
			break;

		case 'x':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 16, 0, width, ladjust, padc, 0);
			break;

		case 'X':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 16, 0, width, ladjust, padc, 1);
			break;

		case 'c':
			c = (char)va_arg(ap, int);
			print_char(out, data, c, width, ladjust);
			break;

		case 's':
			s = (char *)va_arg(ap, char *);
			print_str(out, data, s, width, ladjust);
			break;

		case '\0':
			fmt--;
			break;

		default:
			/* output this char as it is */
			out(data, fmt, 1);
		}
		fmt++;
	}
}


int vscanfmt(scan_callback_t in, void *data, const char *fmt, va_list ap) {
	int *ip;
	char *cp;
	char ch;
	int base, num, neg, ret = 0;

	while (*fmt) {
		if (*fmt == '%') {
			ret++;
			fmt++; // ?? '%'
			do {
				in(data, &ch, 1);
			} while (ch == ' ' || ch == '\t' || ch == '\n'); // ?????
			// ??,?? ch ??????????
			switch (*fmt) {
			case 'd': // ???
				// Lab 1-Extra: Your code here. (2/5)
				// 1. get place
				ip = va_arg(ap, int *);
				// 2. neg
				//in(data,&ch,1);
				if (ch == '-') {
					neg = 1;
					in(data,&ch,1);
				}
				// 3. pre zero
				if (ch == '0') {
					while (ch=='0') {
						in(data,&ch,1);
					}
				}
				// 4. base
				base = 0;
				while ('0' <= ch && ch <= '9' && ch != '\0') {
					base *= 10;
					base += ch - '0';
					in(data,&ch,1);
				}
				// 5. store in ip
				if (neg){
					*ip = -base;
				} else {
					*ip = base;
				}
				break;
			case 'x': // ????
				// Lab 1-Extra: Your code here. (3/5)
				// 1. get place
				ip = va_arg(ap, int *);
				// 2. neg
				//in(data,&ch,1);
				if (ch == '-') {
					neg = 1;
					in(data,&ch,1);
				}
				// 3. pre zero
				if (ch == '0') {
					while (ch=='0') {
						in(data,&ch,1);
					}
				}
				// 4. base
				base = 0;
				while ((('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'f') )&& ch != '\0') {
					base *= 16;
					if ('0' <= ch && ch <= '9') {
						base += ch - '0';
					} else {
						base += ch - 'a' + 10;
					}
					in(data,&ch,1);
				}
				// 5. store in ip
				if (neg){
					ip[0] = -base;
				} else {
					ip[0] = base;
				}
				break;
			case 'c':
				// Lab 1-Extra: Your code here. (4/5)
				// 1. get place
				cp = va_arg(ap, char *);
				// 2. store
				//in(data,cp,1);
				*cp = ch;
				break;
			case 's':
				// Lab 1-Extra: Your code here. (5/5)
				// 1. get argument to store
				num = 0;
				cp = va_arg(ap, char *);
				// 2. get char not \t \n ' ' add to cp
				//in(data,&ch,1);
				while(ch != '\t' && ch != '\n' && ch != ' '){
					cp[num] = ch;
					num++;
					in(data,&ch,1);
				}
				// 3. \0
				cp[num] = '\0';
				break;
			}
			fmt++;
		}
	}
	return ret;
}

/* --------------- local help functions --------------------- */
void print_char(fmt_callback_t out, void *data, char c, int length, int ladjust) {
	int i;

	if (length < 1) {
		length = 1;
	}
	const char space = ' ';
	if (ladjust) {
		out(data, &c, 1);
		for (i = 1; i < length; i++) {
			out(data, &space, 1);
		}
	} else {
		for (i = 0; i < length - 1; i++) {
			out(data, &space, 1);
		}
		out(data, &c, 1);
	}
}

void print_str(fmt_callback_t out, void *data, const char *s, int length, int ladjust) {
	int i;
	int len = 0;
	const char *s1 = s;
	while (*s1++) {
		len++;
	}
	if (length < len) {
		length = len;
	}

	if (ladjust) {
		out(data, s, len);
		for (i = len; i < length; i++) {
			out(data, " ", 1);
		}
	} else {
		for (i = 0; i < length - len; i++) {
			out(data, " ", 1);
		}
		out(data, s, len);
	}
}

void print_num(fmt_callback_t out, void *data, unsigned long u, int base, int neg_flag, int length,
	       int ladjust, char padc, int upcase) {
	/* algorithm :
	 *  1. prints the number from left to right in reverse form.
	 *  2. fill the remaining spaces with padc if length is longer than
	 *     the actual length
	 *     TRICKY : if left adjusted, no "0" padding.
	 *		    if negtive, insert  "0" padding between "0" and number.
	 *  3. if (!ladjust) we reverse the whole string including paddings
	 *  4. otherwise we only reverse the actual string representing the num.
	 */

	int actualLength = 0;
	char buf[length + 70];
	char *p = buf;
	int i;

	do {
		int tmp = u % base;
		if (tmp <= 9) {
			*p++ = '0' + tmp;
		} else if (upcase) {
			*p++ = 'A' + tmp - 10;
		} else {
			*p++ = 'a' + tmp - 10;
		}
		u /= base;
	} while (u != 0);

	if (neg_flag) {
		*p++ = '-';
	}

	/* figure out actual length and adjust the maximum length */
	actualLength = p - buf;
	if (length < actualLength) {
		length = actualLength;
	}

	/* add padding */
	if (ladjust) {
		padc = ' ';
	}
	if (neg_flag && !ladjust && (padc == '0')) {
		for (i = actualLength - 1; i < length - 1; i++) {
			buf[i] = padc;
		}
		buf[length - 1] = '-';
	} else {
		for (i = actualLength; i < length; i++) {
			buf[i] = padc;
		}
	}

	/* prepare to reverse the string */
	int begin = 0;
	int end;
	if (ladjust) {
		end = actualLength - 1;
	} else {
		end = length - 1;
	}

	/* adjust the string pointer */
	while (end > begin) {
		char tmp = buf[begin];
		buf[begin] = buf[end];
		buf[end] = tmp;
		begin++;
		end--;
	}

	out(data, buf, length);
}
