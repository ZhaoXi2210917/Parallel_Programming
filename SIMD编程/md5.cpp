#include "PCFG.h"//解决ABI造成的编译问题
#include "md5.h"
#include <iomanip>
#include <assert.h>
#include <chrono>
#include <vector>
#define MD5_IMPL          // 让 md5.h 暴露那些宏
using namespace std;
using namespace chrono;














/**
 * StringProcess: 将单个输入字符串转换成MD5计算所需的消息数组
 * @param input 输入
 * @param[out] n_byte 用于给调用者传递额外的返回值，即最终Byte数组的长度
 * @return Byte消息数组
 */
Byte *StringProcess(string input, int *n_byte)
{
	// 将输入的字符串转换为Byte为单位的数组
	Byte *blocks = (Byte *)input.c_str();
	int length = input.length();

	// 计算原始消息长度（以比特为单位）
	int bitLength = length * 8;

	// paddingBits: 原始消息需要的padding长度（以bit为单位）
	// 对于给定的消息，将其补齐至length%512==448为止
	// 需要注意的是，即便给定的消息满足length%512==448，也需要再pad 512bits
	int paddingBits = bitLength % 512;
	if (paddingBits > 448)
	{
		paddingBits += 512 - (paddingBits - 448);
	}
	else if (paddingBits < 448)
	{
		paddingBits = 448 - paddingBits;
	}
	else if (paddingBits == 448)
	{
		paddingBits = 512;
	}

	// 原始消息需要的padding长度（以Byte为单位）
	int paddingBytes = paddingBits / 8;
	// 创建最终的字节数组
	// length + paddingBytes + 8:
	// 1. length为原始消息的长度（bits）
	// 2. paddingBytes为原始消息需要的padding长度（Bytes）
	// 3. 在pad到length%512==448之后，需要额外附加64bits的原始消息长度，即8个bytes
	int paddedLength = length + paddingBytes + 8;
	Byte *paddedMessage = new Byte[paddedLength];

	// 复制原始消息
	memcpy(paddedMessage, blocks, length);

	// 添加填充字节。填充时，第一位为1，后面的所有位均为0。
	// 所以第一个byte是0x80
	paddedMessage[length] = 0x80;							 // 添加一个0x80字节
	memset(paddedMessage + length + 1, 0, paddingBytes - 1); // 填充0字节

	// 添加消息长度（64比特，小端格式）
	for (int i = 0; i < 8; ++i)
	{
		// 特别注意此处应当将bitLength转换为uint64_t
		// 这里的length是原始消息的长度
		paddedMessage[length + paddingBytes + i] = ((uint64_t)length * 8 >> (i * 8)) & 0xFF;
	}

	// 验证长度是否满足要求。此时长度应当是512bit的倍数
	int residual = 8 * paddedLength % 512;
	// assert(residual == 0);

	// 在填充+添加长度之后，消息被分为n_blocks个512bit的部分
	*n_byte = paddedLength;
	return paddedMessage;
}


/**
 * MD5Hash: 将单个输入字符串转换成MD5
 * @param input 输入
 * @param[out] state 用于给调用者传递额外的返回值，即最终的缓冲区，也就是MD5的结果
 * @return Byte消息数组
 */
void MD5Hash(string input, bit32 *state)
{

	Byte *paddedMessage;
	int *messageLength = new int[1];
	for (int i = 0; i < 1; i += 1)
	{
		paddedMessage = StringProcess(input, &messageLength[i]);//StringProcess是预处理操作
		// cout<<messageLength[i]<<endl;
		assert(messageLength[i] == messageLength[0]);
	}
	int n_blocks = messageLength[0] / 64;

	// bit32* state= new bit32[4];
	state[0] = 0x67452301;
	state[1] = 0xefcdab89;
	state[2] = 0x98badcfe;
	state[3] = 0x10325476;

	// 逐block地更新state
	for (int i = 0; i < n_blocks; i += 1)
	{
		bit32 x[16];

		// 下面的处理，在理解上较为复杂
		for (int i1 = 0; i1 < 16; ++i1)
		{
			x[i1] = (paddedMessage[4 * i1 + i * 64]) |
					(paddedMessage[4 * i1 + 1 + i * 64] << 8) |
					(paddedMessage[4 * i1 + 2 + i * 64] << 16) |
					(paddedMessage[4 * i1 + 3 + i * 64] << 24);
		}

		bit32 a = state[0], b = state[1], c = state[2], d = state[3];

		auto start = system_clock::now();
		/* Round 1 */
		FF(a, b, c, d, x[0], s11, 0xd76aa478);
		FF(d, a, b, c, x[1], s12, 0xe8c7b756);
		FF(c, d, a, b, x[2], s13, 0x242070db);
		FF(b, c, d, a, x[3], s14, 0xc1bdceee);
		FF(a, b, c, d, x[4], s11, 0xf57c0faf);
		FF(d, a, b, c, x[5], s12, 0x4787c62a);
		FF(c, d, a, b, x[6], s13, 0xa8304613);
		FF(b, c, d, a, x[7], s14, 0xfd469501);
		FF(a, b, c, d, x[8], s11, 0x698098d8);
		FF(d, a, b, c, x[9], s12, 0x8b44f7af);
		FF(c, d, a, b, x[10], s13, 0xffff5bb1);
		FF(b, c, d, a, x[11], s14, 0x895cd7be);
		FF(a, b, c, d, x[12], s11, 0x6b901122);
		FF(d, a, b, c, x[13], s12, 0xfd987193);
		FF(c, d, a, b, x[14], s13, 0xa679438e);
		FF(b, c, d, a, x[15], s14, 0x49b40821);

		/* Round 2 */
		GG(a, b, c, d, x[1], s21, 0xf61e2562);
		GG(d, a, b, c, x[6], s22, 0xc040b340);
		GG(c, d, a, b, x[11], s23, 0x265e5a51);
		GG(b, c, d, a, x[0], s24, 0xe9b6c7aa);
		GG(a, b, c, d, x[5], s21, 0xd62f105d);
		GG(d, a, b, c, x[10], s22, 0x2441453);
		GG(c, d, a, b, x[15], s23, 0xd8a1e681);
		GG(b, c, d, a, x[4], s24, 0xe7d3fbc8);
		GG(a, b, c, d, x[9], s21, 0x21e1cde6);
		GG(d, a, b, c, x[14], s22, 0xc33707d6);
		GG(c, d, a, b, x[3], s23, 0xf4d50d87);
		GG(b, c, d, a, x[8], s24, 0x455a14ed);
		GG(a, b, c, d, x[13], s21, 0xa9e3e905);
		GG(d, a, b, c, x[2], s22, 0xfcefa3f8);
		GG(c, d, a, b, x[7], s23, 0x676f02d9);
		GG(b, c, d, a, x[12], s24, 0x8d2a4c8a);

		/* Round 3 */
		HH(a, b, c, d, x[5], s31, 0xfffa3942);
		HH(d, a, b, c, x[8], s32, 0x8771f681);
		HH(c, d, a, b, x[11], s33, 0x6d9d6122);
		HH(b, c, d, a, x[14], s34, 0xfde5380c);
		HH(a, b, c, d, x[1], s31, 0xa4beea44);
		HH(d, a, b, c, x[4], s32, 0x4bdecfa9);
		HH(c, d, a, b, x[7], s33, 0xf6bb4b60);
		HH(b, c, d, a, x[10], s34, 0xbebfbc70);
		HH(a, b, c, d, x[13], s31, 0x289b7ec6);
		HH(d, a, b, c, x[0], s32, 0xeaa127fa);
		HH(c, d, a, b, x[3], s33, 0xd4ef3085);
		HH(b, c, d, a, x[6], s34, 0x4881d05);
		HH(a, b, c, d, x[9], s31, 0xd9d4d039);
		HH(d, a, b, c, x[12], s32, 0xe6db99e5);
		HH(c, d, a, b, x[15], s33, 0x1fa27cf8);
		HH(b, c, d, a, x[2], s34, 0xc4ac5665);

		/* Round 4 */
		II(a, b, c, d, x[0], s41, 0xf4292244);
		II(d, a, b, c, x[7], s42, 0x432aff97);
		II(c, d, a, b, x[14], s43, 0xab9423a7);
		II(b, c, d, a, x[5], s44, 0xfc93a039);
		II(a, b, c, d, x[12], s41, 0x655b59c3);
		II(d, a, b, c, x[3], s42, 0x8f0ccc92);
		II(c, d, a, b, x[10], s43, 0xffeff47d);
		II(b, c, d, a, x[1], s44, 0x85845dd1);
		II(a, b, c, d, x[8], s41, 0x6fa87e4f);
		II(d, a, b, c, x[15], s42, 0xfe2ce6e0);
		II(c, d, a, b, x[6], s43, 0xa3014314);
		II(b, c, d, a, x[13], s44, 0x4e0811a1);
		II(a, b, c, d, x[4], s41, 0xf7537e82);
		II(d, a, b, c, x[11], s42, 0xbd3af235);
		II(c, d, a, b, x[2], s43, 0x2ad7d2bb);
		II(b, c, d, a, x[9], s44, 0xeb86d391);

		state[0] += a;
		state[1] += b;
		state[2] += c;
		state[3] += d;
	}

	// 下面的处理，在理解上较为复杂
	for (int i = 0; i < 4; i++)
	{
		uint32_t value = state[i];
		state[i] = ((value & 0xff) << 24) |		 // 将最低字节移到最高位
				   ((value & 0xff00) << 8) |	 // 将次低字节左移
				   ((value & 0xff0000) >> 8) |	 // 将次高字节右移
				   ((value & 0xff000000) >> 24); // 将最高字节移到最低位
	}

	// 输出最终的hash结果
	// for (int i1 = 0; i1 < 4; i1 += 1)
	// {
	// 	cout << std::setw(8) << std::setfill('0') << hex << state[i1];
	// }
	// cout << endl;

	// 释放动态分配的内存
	// 实现SIMD并行算法的时候，也请记得及时回收内存！
	delete[] paddedMessage;
	delete[] messageLength;
}


void MD5Hash_SIMD(const std::vector<std::string>& inputs,uint32_t digests[4][4])
{
/************ 1. 逐条口令调用 StringProcess 得到填充后字节流 ************/
struct MsgBuf {
Byte*   ptr   = nullptr;   // padding 后的整块数据
size_t  nByte = 0;         // 字节数，一定是 64 的倍数
size_t  nBlk  = 0;         // 512-bit 块数
} buf[4];

size_t maxBlocks = 0;
size_t lanes = inputs.size();


for (int i = 0; i < 4; ++i) {
	if (i < lanes) 
	{
		buf[i].ptr  = StringProcess(inputs[i], reinterpret_cast<int*>(&buf[i].nByte));
		buf[i].nBlk = buf[i].nByte / 64;
	}
else 
{                       
	        buf[i].ptr = nullptr;
	        buf[i].nBlk = 0;  
	}
maxBlocks   = std::max(maxBlocks, buf[i].nBlk);
}

/************ 2. 向量化状态寄存器初始化 ************/
vec32 A = vdupq_n_u32(0x67452301);
vec32 B = vdupq_n_u32(0xefcdab89);
vec32 C = vdupq_n_u32(0x98badcfe);
vec32 D = vdupq_n_u32(0x10325476);

/************ 3. 按块并行压缩 ************/
for (size_t blk = 0; blk < maxBlocks; ++blk)
{
/* 3.1 载入并转置 16×32-bit 消息字 */
vec32 W[16];
for (int word = 0; word < 16; ++word)
{
uint32_t lane[4] = {0,0,0,0};
for (int laneId = 0; laneId < 4; ++laneId) {
  if (blk < buf[laneId].nBlk) {                // 该条口令还有这一块
	  size_t off = blk*64 + word*4;
	  Byte*  p   = buf[laneId].ptr + off;      // 指向 4 字节小端
	  lane[laneId] =  p[0] | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);
  }
}
W[word] = vld1q_u32(lane);   // 把 4 个 lane 装入向量
}

/* 3.2 保存原值 */
vec32 AA=A, BB=B, CC=C, DD=D;

/* 3.3 64 步 – 与标量版完全同序，只把 FF→FF_V 等 */
/* ---------- Round 1 ---------- */
FF_V(A,B,C,D,W[ 0],  7,vdupq_n_u32(0xd76aa478));
FF_V(D,A,B,C,W[ 1], 12,vdupq_n_u32(0xe8c7b756));
FF_V(C,D,A,B,W[ 2], 17,vdupq_n_u32(0x242070db));
FF_V(B,C,D,A,W[ 3], 22,vdupq_n_u32(0xc1bdceee));
FF_V(A,B,C,D,W[ 4],  7,vdupq_n_u32(0xf57c0faf));
FF_V(D,A,B,C,W[ 5], 12,vdupq_n_u32(0x4787c62a));
FF_V(C,D,A,B,W[ 6], 17,vdupq_n_u32(0xa8304613));
FF_V(B,C,D,A,W[ 7], 22,vdupq_n_u32(0xfd469501));
FF_V(A,B,C,D,W[ 8],  7,vdupq_n_u32(0x698098d8));
FF_V(D,A,B,C,W[ 9], 12,vdupq_n_u32(0x8b44f7af));
FF_V(C,D,A,B,W[10], 17,vdupq_n_u32(0xffff5bb1));
FF_V(B,C,D,A,W[11], 22,vdupq_n_u32(0x895cd7be));
FF_V(A,B,C,D,W[12],  7,vdupq_n_u32(0x6b901122));
FF_V(D,A,B,C,W[13], 12,vdupq_n_u32(0xfd987193));
FF_V(C,D,A,B,W[14], 17,vdupq_n_u32(0xa679438e));
FF_V(B,C,D,A,W[15], 22,vdupq_n_u32(0x49b40821));

/* ---------- Round 2 ---------- */
GG_V(A,B,C,D,W[ 1],  5,vdupq_n_u32(0xf61e2562));
GG_V(D,A,B,C,W[ 6],  9,vdupq_n_u32(0xc040b340));
GG_V(C,D,A,B,W[11], 14,vdupq_n_u32(0x265e5a51));
GG_V(B,C,D,A,W[ 0], 20,vdupq_n_u32(0xe9b6c7aa));
GG_V(A,B,C,D,W[ 5],  5,vdupq_n_u32(0xd62f105d));
GG_V(D,A,B,C,W[10],  9,vdupq_n_u32(0x02441453));
GG_V(C,D,A,B,W[15], 14,vdupq_n_u32(0xd8a1e681));
GG_V(B,C,D,A,W[ 4], 20,vdupq_n_u32(0xe7d3fbc8));
GG_V(A,B,C,D,W[ 9],  5,vdupq_n_u32(0x21e1cde6));
GG_V(D,A,B,C,W[14],  9,vdupq_n_u32(0xc33707d6));
GG_V(C,D,A,B,W[ 3], 14,vdupq_n_u32(0xf4d50d87));
GG_V(B,C,D,A,W[ 8], 20,vdupq_n_u32(0x455a14ed));
GG_V(A,B,C,D,W[13],  5,vdupq_n_u32(0xa9e3e905));
GG_V(D,A,B,C,W[ 2],  9,vdupq_n_u32(0xfcefa3f8));
GG_V(C,D,A,B,W[ 7], 14,vdupq_n_u32(0x676f02d9));
GG_V(B,C,D,A,W[12], 20,vdupq_n_u32(0x8d2a4c8a));

/* ---------- Round 3 ---------- */
HH_V(A,B,C,D,W[ 5],  4,vdupq_n_u32(0xfffa3942));
HH_V(D,A,B,C,W[ 8], 11,vdupq_n_u32(0x8771f681));
HH_V(C,D,A,B,W[11], 16,vdupq_n_u32(0x6d9d6122));
HH_V(B,C,D,A,W[14], 23,vdupq_n_u32(0xfde5380c));
HH_V(A,B,C,D,W[ 1],  4,vdupq_n_u32(0xa4beea44));
HH_V(D,A,B,C,W[ 4], 11,vdupq_n_u32(0x4bdecfa9));
HH_V(C,D,A,B,W[ 7], 16,vdupq_n_u32(0xf6bb4b60));
HH_V(B,C,D,A,W[10], 23,vdupq_n_u32(0xbebfbc70));
HH_V(A,B,C,D,W[13],  4,vdupq_n_u32(0x289b7ec6));
HH_V(D,A,B,C,W[ 0], 11,vdupq_n_u32(0xeaa127fa));
HH_V(C,D,A,B,W[ 3], 16,vdupq_n_u32(0xd4ef3085));
HH_V(B,C,D,A,W[ 6], 23,vdupq_n_u32(0x04881d05));
HH_V(A,B,C,D,W[ 9],  4,vdupq_n_u32(0xd9d4d039));
HH_V(D,A,B,C,W[12], 11,vdupq_n_u32(0xe6db99e5));
HH_V(C,D,A,B,W[15], 16,vdupq_n_u32(0x1fa27cf8));
HH_V(B,C,D,A,W[ 2], 23,vdupq_n_u32(0xc4ac5665));

/* ---------- Round 4 ---------- */
II_V(A,B,C,D,W[ 0],  6,vdupq_n_u32(0xf4292244));
II_V(D,A,B,C,W[ 7], 10,vdupq_n_u32(0x432aff97));
II_V(C,D,A,B,W[14], 15,vdupq_n_u32(0xab9423a7));
II_V(B,C,D,A,W[ 5], 21,vdupq_n_u32(0xfc93a039));
II_V(A,B,C,D,W[12],  6,vdupq_n_u32(0x655b59c3));
II_V(D,A,B,C,W[ 3], 10,vdupq_n_u32(0x8f0ccc92));
II_V(C,D,A,B,W[10], 15,vdupq_n_u32(0xffeff47d));
II_V(B,C,D,A,W[ 1], 21,vdupq_n_u32(0x85845dd1));
II_V(A,B,C,D,W[ 8],  6,vdupq_n_u32(0x6fa87e4f));
II_V(D,A,B,C,W[15], 10,vdupq_n_u32(0xfe2ce6e0));
II_V(C,D,A,B,W[ 6], 15,vdupq_n_u32(0xa3014314));
II_V(B,C,D,A,W[13], 21,vdupq_n_u32(0x4e0811a1));
II_V(A,B,C,D,W[ 4],  6,vdupq_n_u32(0xf7537e82));
II_V(D,A,B,C,W[11], 10,vdupq_n_u32(0xbd3af235));
II_V(C,D,A,B,W[ 2], 15,vdupq_n_u32(0x2ad7d2bb));
II_V(B,C,D,A,W[ 9], 21,vdupq_n_u32(0xeb86d391));

/* 3.4 feed-forward */
A = vaddq_u32(A, AA);
B = vaddq_u32(B, BB);
C = vaddq_u32(C, CC);
D = vaddq_u32(D, DD);
}

/************ 4. 写回散列并做大小端转换 ************/
uint32_t tmp[4];
vst1q_u32(tmp, A); for (int i=0;i<4;++i) digests[i][0]=tmp[i];
vst1q_u32(tmp, B); for (int i=0;i<4;++i) digests[i][1]=tmp[i];
vst1q_u32(tmp, C); for (int i=0;i<4;++i) digests[i][2]=tmp[i];
vst1q_u32(tmp, D); for (int i=0;i<4;++i) digests[i][3]=tmp[i];

for (int m=0;m<4;++m)
for (int w=0;w<4;++w){
uint32_t v = digests[m][w];
digests[m][w] = ((v&0xff)<<24)|((v&0xff00)<<8)|
			  ((v&0xff0000)>>8)|((v&0xff000000)>>24);
}

/************ 5. 释放由 StringProcess malloc 的内存 ************/
for (int i = 0; i < 4; ++i)
delete[] buf[i].ptr;
}
void MD5Hash_SIMD(const std::string& input, uint32_t digest[4])
{
    std::vector<std::string> v{ input };   // 只有 1 条
    uint32_t buf[4][4] = {0};
    MD5Hash_SIMD(v, buf);                  // 调用刚才统一好的 vector 版本
    std::memcpy(digest, buf[0], 4 * sizeof(uint32_t));
}

