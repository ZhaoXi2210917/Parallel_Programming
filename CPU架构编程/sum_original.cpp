#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;
using namespace chrono;

// 设置数组大小（n）
const int N = 1 << 19;         // 这里相当于 2^15 = 32768
// 设置总操作数
const long long times = 100000000000LL;  
// 每次 sumArray() 做了 N 次加法，所以循环次数 t = times / N
const long long t = times / N;

// 全局数组，用来存放随机数据
static long long arr[N];

// 生成随机数据
void generateRandomData() {
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < N; ++i) {
        arr[i] = rand() % 100;  // 0~99 的随机整数
    }
}

// 计算 n 个数的和
long long sumArray() {
    long long sum = 0;
    for (int i = 0; i < N; i++) {
        sum += arr[i];
    }
    return sum;
}

int main() {
    cout << "Array size: " << N << endl;

    // 1. 生成随机数据
    generateRandomData();

    // 2. 开始计时
    auto start = high_resolution_clock::now();
    
    // 3. 重复 t 次，模拟总操作数
    long long finalResult = 0; // 用来存储结果，防止编译器过度优化
    for (int i = 0; i < t; i++) {
        finalResult += sumArray();
    }

    // 4. 结束计时
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    // 5. 输出平均耗时（ms）
    double avgTimeMs = static_cast<double>(duration.count()) / t;
    cout << "Execution time: " << avgTimeMs << " ms" << endl;

    // 输出 finalResult，避免被编译器优化掉
    cout << "Check result: " << finalResult << endl;

    return 0;
}
