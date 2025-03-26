#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>  // 添加计时功能头文件

using namespace std;
using namespace chrono;  // 使用 chrono 命名空间简化写法

typedef unsigned long long ull;

const int N = 1 << 15;  // 矩阵和向量大小
const int times = 100000000000;//总计算次数
const int t = times / (N * N);//矩阵计算循环次数


int result[N];


// 矩阵数据
int matrix[N][N];
// 向量数据
int vec[N];

// 初始化随机数据
void generateRandomData() {
    srand(static_cast<unsigned int>(time(0)));  // 设置随机种子

    for (int i = 0; i < N; ++i) {
        vec[i] = rand() % 100;
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = rand() % 100;
        }
    }
}

// 循环展开
void columnVectorInnerProduct() {

    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; col+=2) {
            result[col] += matrix[row][col] * vec[row];
            result[col + 1] += matrix[row][col + 1] * vec[row];
        }
    }

}

int main() {
    cout << "Matrix size: " << N << " x " << N << endl;

    generateRandomData();

    // 开始计时
    auto start = high_resolution_clock::now();
    for (int i = 0;i < t;i++) {
        columnVectorInnerProduct();
    }
    // 结束计时
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);



    cout << "Execution time: " << duration.count() / t << " ms" << endl;

    return 0;
}
