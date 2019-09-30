#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include<stdint.h>  // 关于不同平台int大小的问题

typedef struct _MsgInt {
	unsigned int* msgInt;//原始消息字符转化为int指针数组，每4个字符转化为一个int个数
	int intCount;//int数个数
}MsgInt;
unsigned int chars_unit32(unsigned int n, char b[], int i)
{
	(n) = ((unsigned int)(b)[(i)] << 24)
		| ((unsigned int)(b)[(i)+1] << 16)
		| ((unsigned int)(b)[(i)+2] << 8)
		| ((unsigned int)(b)[(i)+3]);
	return n;
}

//将1个32位的整数存储扩展位四个字符
void unit32_chars(unsigned int n, char b[], int i)
{
	b[i] = (char)((n) >> 24);
	b[i + 1] = (char)((n) >> 16);
	b[i + 2] = (char)((n) >> 8);
	b[i + 3] = (char)((n));

}

void bigEnd()
{
	char c;
	int a = 1;
	// 由于int为4Byte，逻辑顺序展开成16进制1=0x0000_0001，
	// 如果是大端则在内存中同逻辑顺序 00 00 00 01，
	// 而小端则存在内存中是 01 00 00 00 （按字节，即8bit为块，高低地址颠倒）
	int* p = &a; // 用p指针指向a的首地址
	c = *p; // c是char类型，1Byte，读取首地址开始1个字节
	if (c == a) // 如果 c的值刚好是1=a的话，说明颠倒了，则是小端
	{
		printf("本机为小端存储\n");
	}
	else
	{
		printf("本机为大端存储\n");
	}
}

//void test()
//{
//	unsigned char a[2] = { 0b01100001, 0b01100010 };
//	unsigned char b[2] = { 0b01100011, 0b01100010 };
//	//						   0b01100001, 0b01100010
//	unsigned char* c = (unsigned char*)malloc(2);
//
//	//*c = *a & *b;
//}

void test2()
{
	unsigned char* str = "abcdefg";
	unsigned int* a = (unsigned int*)malloc(sizeof(*str));
	unsigned int n = 0;
	char* s = "abcd";

	a = str;

	printf("%x\n", *a);
	printf("%s\n", a);

	unsigned int b = chars_unit32(n, s, 0);

	printf("%d\n", b);
}

void test3()
{
	unsigned char a = 'a';
	unsigned char b = 'b';
	unsigned char c = 'c';
	unsigned char d = 'd';

	unsigned int m = (unsigned int)"abcd";
	unsigned int aa = (unsigned int)a;
	unsigned int aaa = aa << 24;
	unsigned int bb = (unsigned int)b;
	unsigned int bbb = bb << 16;
	unsigned int dd = (unsigned int)d;

	unsigned int n = ((unsigned int)a << 24)
		| ((unsigned int)b << 16)
		| ((unsigned int)c << 8)
		| ((unsigned int)d);

	printf("%d\n", m);
	printf("%x\n", n);

	printf("%d\n", sizeof(unsigned long));
	printf("%d\n", sizeof(unsigned int));
	printf("%d\n", sizeof(unsigned __int32)); // 微软标准
	printf("%d\n", sizeof(unsigned __int64));
	//printf("%d\n", sizeof(int32_t));

}

void test4()
{
	unsigned long long a = 0x7;
	printf("%d\n", sizeof(unsigned long));
}

//---------------------------------------------------------------------------------------------------


/*
 * 宏函数NOT_BIG_ENDIAN()
 * 用于测试运行环境是否为大端，小端返回true
 */
static const int endianTestNum = 1;
#define NOT_BIG_ENDIAN() ( *(char *)&endianTestNum == 1 )


/*
 * 宏函数UCHAR_2_UINT(uchr8,uint32,i,notBigendian)
 * uchr8        -- unsigned char - 8bit
 * uint32       -- unsigned int  - 32bit
 * i            -- int
 * notBigendian -- int/bool
 * 将uchr8接收的字符数组,转换成大端表示的uint32（从底层看二进制按左高位右地位排列）
 * NOT_BIG_ENDIAN()检验环境，非大端时notBigendian为真，启用此宏函数
 */
#define UCHAR_2_UINT(uchr8,uint32,i,notBigendian)				\
{																\
	if(notBigendian){                                           \
		(uint32) = ((unsigned int) (uchr8)[(i)    ] << 24 )		\
				 | ((unsigned int) (uchr8)[(i) + 1] << 16 )		\
				 | ((unsigned int) (uchr8)[(i) + 2] << 8  )		\
				 | ((unsigned int) (uchr8)[(i) + 3]       );	\
	}															\
}

 /*
  * 宏函数UINT_2_UCHAR(uint32,uchr8,i,notBigendian)
  * uchr8        -- unsigned char - 8bit
  * uint32       -- unsigned int  - 32bit
  * i            -- int
  * notBigendian -- int/bool
  * 将大端表示的uint32,转换成uchr8的字符数组
  * NOT_BIG_ENDIAN()检验环境，非大端时notBigendian为真，启用此宏函数
  */
#define UINT_2_UCHAR(uint32,uchr8,i,notBigendian)				\
{																\
	if(notBigendian){                                           \
		(uchr8)[(i)    ] = (unsigned char)((uint32) >> 24)		\
		(uchr8)[(i) + 1] = (unsigned char)((uint32) >> 16)		\
		(uchr8)[(i) + 2] = (unsigned char)((uint32) >> 8 )		\
		(uchr8)[(i) + 3] = (unsigned char)((uint32)      );		\
	}															\
}

MsgInt MsgFill512(unsigned char* msg, int notBigendian)
{
	// int msgLength = sizeof(*msg) * 8; 指针数组长度不能用sizeof算
	//由于指针的特殊性，永远记录的首地址，而不记录所指向的数据的内存大小，因此指针数组无法求出长度
	//unsigned char msgArry[] = *msg; // 因此将值传入数组，数组可以计算长度...这样不合法
	//int msgLength = sizeof(msgArry) / sizeof(msgArry[0]) * 8; //。。。我好像忘了strlen了。，。
	MsgInt filledMsgInt;
	unsigned long long msgLength = strlen((char*)msg);// 这里必须有个强制转换，strlen不支持unsigned char*
	unsigned long long msgbitLength = msgLength * 8; // 求原始消息的比特长度
	int zeroFill = 448 - (msgbitLength + 8) % 512; // +8是补了0x80=0b1000_0000
	unsigned char* zeroChar = (unsigned char*)malloc(zeroFill / 8);

	memset(zeroChar, 0, zeroFill / 8);
	// 不能用strlen((char*)zeroChar),zeroChar全填0，而字符串结束标志就是0，所以strlen((char*)zeroChar)=0
	//zeroChar[zeroFill / 8] = '\0';

	int totalChrLength = msgLength + 1 + zeroFill / 8 + 8;

	filledMsgInt.msgInt = (unsigned int*)malloc(totalChrLength / 4);
	filledMsgInt.intCount = totalChrLength / 4;//totalChrLength是字符个数8bit/个，msgInt为32bit/个

	unsigned char* msgFill = (unsigned char*)malloc(totalChrLength);// 1表示0x80的长度，一个字节
	memcpy(msgFill, msg, msgLength);
	unsigned char one = 0x80;
	memcpy(msgFill + msgLength, &one, 1);
	memcpy(msgFill + msgLength + 1, zeroChar, zeroFill / 8);
	//unsigned char* msgLenChr = (unsigned char*)msgLength;
	//memcpy(msgFill + msgLength + 1  + zeroFill / 8, msgLenChr, 8); // 这里填充有问题

	unsigned char msgLenChr[8];
	if (notBigendian) { // 小端系统，long long 都是在内存中颠倒存储的，所以需要转换
		for (int i = 0; i < 8; i++) {
			msgLenChr[i] = msgbitLength >> (56 - 8 * i);
		}
		memcpy(msgFill + msgLength + 1 + zeroFill / 8, msgLenChr, 8);
	}
	else { // 如果是大端系统，直接拷贝msgbitLength内存内容即可
		memcpy(msgFill + msgLength + 1 + zeroFill / 8, &msgbitLength, 8);
	}

	/*printf("%d\n", msgbitLength);
	printf("%d\n", zeroFill);
	printf("%d\n", zeroFill / 8);
	printf("%s\n", zeroChar);
	printf("%d\n", strlen(zeroChar));*/

	for (int i = 0; i < filledMsgInt.intCount; i++) {
		unsigned char msgSlice[4] = { *(msgFill + i * 4),*(msgFill + i * 4 + 1),*(msgFill + i * 4 + 2),*(msgFill + i * 4 + 3) };
		unsigned int a = (unsigned int*)msgSlice;
		UCHAR_2_UINT(msgSlice, filledMsgInt.msgInt[i], 0, notBigendian);
	}
	for (int i = 0; i < filledMsgInt.intCount; i++) {
		printf("%08x", filledMsgInt.msgInt[i]);
	}
	return filledMsgInt;
}

int main()
{
	int bigendFlag = NOT_BIG_ENDIAN();

	unsigned char* chr = "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd";
	unsigned int n;

	//unsigned char ch[] = *chr;


	MsgFill512(chr, bigendFlag);

	UCHAR_2_UINT(chr, n, 0, bigendFlag);

	//printf("%x\n%s\n", n,n);


	return 0;
}
