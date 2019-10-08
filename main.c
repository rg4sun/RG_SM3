#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include<stdint.h>  // 关于不同平台int大小的问题

typedef struct _MsgInt {
	unsigned int* msgInt;//原始消息字符转化为int指针数组，每4个字符转化为一个int个数
	int intCount;//int数个数
}MsgInt;

typedef struct _ExtendMsgInt {
	unsigned int W[68];
	unsigned int W1[64];
}ExtendMsgInt;

// =====================================================================
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


// 初始向量
const unsigned int IV[8] = {
	0x7380166F,0x4914B2B9,0x172442D7,0xDA8A0600,
	0xA96F30BC,0x163138AA,0xE38DEE4D,0xB0FB0E4E
};

/*
 * 宏函数NOT_BIG_ENDIAN()
 * 用于测试运行环境是否为大端，小端返回true
 */
static const int endianTestNum = 1;
#define NOT_BIG_ENDIAN() ( *(char *)&endianTestNum == 1 )

#define FF_LOW(x,y,z) ( (x) ^ (y) ^ (z))
#define FF_HIGH(x,y,z) (((x) & (y)) | ( (x) & (z)) | ( (y) & (z)))

//#define FF(x,y,z,j)									\
//{													\
//	if(j<16){										\
//		((x) ^ (y) ^ (z));							\
//	}												\
//	else{											\
//		(((x)& (y)) | ((x) & (z)) | ((y) & (z)));	\
//	}												\
//}




#define GG_LOW(x,y,z) ( (x) ^ (y) ^ (z))
#define GG_HIGH(x,y,z) (((x) & (y)) | ( (~(x)) & (z)) )


//unsigned int ROTATE_LEFT(unsigned int word, unsigned int bits)
//{
//	word = ((word) << (bits) | (word) >> (32 - (bits)));
//	return word;
//}

//#define ROTATE_LEFT(uint32,shift) ( (uint32) = ( ( (uint32) << (shift) ) | ( (uint32) >> (32 - (shift)) ) ) )
// 上面写的会改变传入的参数的值，影响MsgExtend函数里W 的运算
#define ROTATE_LEFT(uint32,shift) ( ( ( (uint32) << (shift) ) | ( (uint32) >> (32 - (shift)) ) ) )

//#define  SHL(x,n) (((x) & 0xFFFFFFFF) << n)
//#define ROTL(x,n) (SHL((x),n) | ((x) >> (32 - n)))

#define P0(x) ((x) ^  ROTATE_LEFT((x),9) ^ ROTATE_LEFT((x),17))
#define P1(x) ((x) ^  ROTATE_LEFT((x),15) ^ ROTATE_LEFT((x),23))

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
		(uchr8)[(i)    ] = (unsigned char)((uint32) >> 24);		\
		(uchr8)[(i) + 1] = (unsigned char)((uint32) >> 16);		\
		(uchr8)[(i) + 2] = (unsigned char)((uint32) >> 8 );		\
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
	// 直接用memset从内存中拷贝值
	//zeroChar[zeroFill / 8] = '\0'; // 这个不需要加，直接从内存拷贝，不用确认是不是字符串，也无需加结束符
	// 实际上，zeroChar里面填充的全部0，在字符串识别来看都是结束符/0

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

	return filledMsgInt;
}

ExtendMsgInt MsgExtend(unsigned int msgInt16[])
{
	ExtendMsgInt etdMsgInt;

	for (int i = 0; i < 16; i++) {
		etdMsgInt.W[i] = msgInt16[i];
	}
	for (int j = 16; j < 68; j++) {
		unsigned int tmp;
		tmp = etdMsgInt.W[j - 16] ^ etdMsgInt.W[j - 9] ^ ROTATE_LEFT(etdMsgInt.W[j - 3], 15);
		// 好像找到W数组会莫名被修改的原因了，好像是ROTATE_LEFT会改变传入的参数
		etdMsgInt.W[j] = P1(tmp) ^ ROTATE_LEFT(etdMsgInt.W[j - 13], 7) ^ etdMsgInt.W[j - 6];
	}
	for (int j = 0; j < 64; j++) {
		etdMsgInt.W1[j] = etdMsgInt.W[j] ^ etdMsgInt.W[j + 4];
	}
	return etdMsgInt;
}

unsigned int* CF(unsigned int Vi[], unsigned int msgInt16[], unsigned int W[], unsigned int W1[])
{
	unsigned int regA2H[8]; // A~H 8个寄存器
	unsigned int SS1, SS2, TT1, TT2;

	for (int i = 0; i < 8; i++) {
		regA2H[i] = Vi[i];
	}
	for (int j = 0; j < 64; j++) {
		unsigned int T = 0x79cc4519; // 文档中的常量Tj，此处用T
		if (j >= 16) {
			T = 0x7a879d8a;
		}
		SS1 = ROTATE_LEFT(ROTATE_LEFT(regA2H[0], 12) + regA2H[4] + ROTATE_LEFT(T, j), 7);
		SS2 = SS1 ^ ROTATE_LEFT(regA2H[0], 12);
		if (j < 16) {
			TT1 = FF_LOW(regA2H[0], regA2H[1], regA2H[2]) + regA2H[3] + SS2 + W1[j];
			TT2 = GG_LOW(regA2H[4], regA2H[5], regA2H[6]) + regA2H[7] + SS1 + W[j];
		}
		else {
			TT1 = FF_HIGH(regA2H[0], regA2H[1], regA2H[2]) + regA2H[3] + SS2 + W1[j];
			TT2 = GG_HIGH(regA2H[4], regA2H[5], regA2H[6]) + regA2H[7] + SS1 + W[j];
		}
		regA2H[3] = regA2H[2];
		regA2H[2] = ROTATE_LEFT(regA2H[1], 9);
		regA2H[1] = regA2H[0];
		regA2H[0] = TT1;
		regA2H[7] = regA2H[6];
		regA2H[6] = ROTATE_LEFT(regA2H[5], 19);
		regA2H[5] = regA2H[4];
		regA2H[4] = P0(TT2);
	}
	for (int i = 0; i < 8; i++) { // 计算 ABCDEFH ^ Vi
		regA2H[i] ^= Vi[i];
	}
	return regA2H;
}

unsigned int* SM3Hash(unsigned char* msgText, int notBigendian) {
	MsgInt filledMsgInt = MsgFill512(msgText, notBigendian);
	// 对填充好的消息按512bit进行分组，即每16个int一组
	int groupAmount = filledMsgInt.intCount / 16; 
	//unsigned int* V = IV;

	unsigned int V[8];
	for (int i = 0; i < 8; i++) {
		V[i] = IV[i];
	}
	for (int i = 0; i < groupAmount; i++) {
		unsigned int* bi = 16 * i + filledMsgInt.msgInt;
		ExtendMsgInt etdMsgInt = MsgExtend(bi);
		unsigned int* temp = CF(V, bi, etdMsgInt.W, etdMsgInt.W1); // 每一轮压缩更新V
		for (int i = 0; i < 8; i++) {
			V[i] = temp[i];
		}
	}
	return V;
	char sm3HashValue[32];
	for (int i = 0; i < 8; i++) {
		UINT_2_UCHAR(V[i], sm3HashValue, 4 * i, notBigendian);
	}
}


void test()
{
	int bigendFlag = NOT_BIG_ENDIAN();

	unsigned char* chr = "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd";

	MsgInt msgInt;
	ExtendMsgInt etdMsgInt;
	//unsigned char ch[] = *chr;


	msgInt = MsgFill512(chr, bigendFlag);

	//消息填充输出测试
	puts("------- 消息填充输出测试 -------");
	for (int i = 0; i < msgInt.intCount; i++) {
		printf("%08x ", msgInt.msgInt[i]);
	}

	etdMsgInt = MsgExtend(msgInt.msgInt);

	//消息扩展输出测试
	puts("\n------- 消息扩展输出测试 -------");
	printf("W0---W67:\n");
	for (int i = 0; i < 68; i++) {
		printf("%08x ", etdMsgInt.W[i]);
	}
	printf("\n\nW1_0----W1_63:\n");
	for (int i = 0; i < 64; i++) {
		printf("%08x ", etdMsgInt.W1[i]);
	}
}


int main()
{
	int bigendFlag = NOT_BIG_ENDIAN();

	unsigned char* chr = "你好么";
	unsigned int* hashInt = SM3Hash(chr, bigendFlag);

	unsigned int result[8];

	for (int i = 0; i < 8; i++) {
		result[i] = hashInt[i];
	}

	//printf("%08x ", hashInt[0]);
	//printf("%08x ", hashInt[1]);
	//printf("%08x ", hashInt[2]);
	for (int i = 0; i < 8; i++) {
		printf("%08x ", result[i]);
	}
	printf("\n");

	//test();

	return 0;
}
