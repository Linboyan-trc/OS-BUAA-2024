1. 打印数组
	1. 设置数组指针，int一套long一套
		int 		*a_int;
		long int 	*a_long;
	2. 循环变量，数组规模
		int 		var_for;
		int 		arraySize;
	3. 得到数组规模, 三位数264
		arraySize = 0;
		if(*fmt == '#'){
			fmt++;
			while(IsDigit(*fmt)) {
				arraySize = arraySize*10 + Ctod(*fmt);
				fmt++;
			}
		}
	4. 增加情况
		case 'a':
		case 'A':
			4.1 long判断
				if (longFlag) {
					a_long = va_arg(ap, long int*);
				} else {
					a_int = va_arg(ap, int*);
				}
			4.2 打印 { + 剩下的自己看吧
length = PrintChar(buf, '{', 1, 0);
		OUTPUT(arg, buf, length);
		for (var_for = 0; var_for < arraySize; var_for++)
		{
			if (longFlag) { 
				num = a_long[var_for];
			} else { 
				num = a_int[var_for]; 
			}
			negFlag = 0;
			if (num < 0) {
				num = - num;
				negFlag = 1;
			}
			length = PrintNum(buf, num, 10, negFlag, width, ladjust, padc, 0);
			OUTPUT(arg, buf, length);
			if (var_for != arraySize - 1)
			{
				length = PrintChar(buf, ',', 1, 0);
				OUTPUT(arg, buf, length);
			}
		}
		length = PrintChar(buf, '}', 1, 0);
		OUTPUT(arg, buf, length);
		break;


代码:
/////////////////////////////////////////////////////////////////////////////////////////////////
	int *a_int;
	long int *a_long;
int var_for;
int arraySize;
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
	arraySize = 0;
	if (*fmt == '#'){
		fmt ++;
		while (IsDigit(*fmt)) {
			arraySize = arraySize * 10 + Ctod(*fmt);
			fmt ++;
		}
	}
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
	 case 'a':
	 case 'A':
		if (longFlag) {
			a_long = va_arg(ap, long int*);
		}
		else {
			a_int = va_arg(ap, int*);
		}
		length = PrintChar(buf, '{', 1, 0);
		OUTPUT(arg, buf, length);
		for (var_for = 0; var_for < arraySize; var_for++)
		{
			if (longFlag) { 
				num = a_long[var_for];
			} else { 
				num = a_int[var_for]; 
			}
			negFlag = 0;
			if (num < 0) {
				num = - num;
				negFlag = 1;
			}
			length = PrintNum(buf, num, 10, negFlag, width, ladjust, padc, 0);
			OUTPUT(arg, buf, length);
			if (var_for != arraySize - 1)
			{
				length = PrintChar(buf, ',', 1, 0);
				OUTPUT(arg, buf, length);
			}
		}
		length = PrintChar(buf, '}', 1, 0);
		OUTPUT(arg, buf, length);
		break;
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

