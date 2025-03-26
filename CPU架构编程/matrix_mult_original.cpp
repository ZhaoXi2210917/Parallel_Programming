#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>  // ��Ӽ�ʱ����ͷ�ļ�

using namespace std;
using namespace chrono;  // ʹ�� chrono �����ռ��д��

typedef unsigned long long ull;

const int N = 1 << 15;  // �����������С
const int times = 100000000000;//�ܼ������
const int t = times / (N * N);//�������ѭ������


int result[N];


// ��������
int matrix[N][N];
// ��������
int vec[N];

// ��ʼ���������
void generateRandomData() {
    srand(static_cast<unsigned int>(time(0)));  // �����������

    for (int i = 0; i < N; ++i) {
        vec[i] = rand() % 100;
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = rand() % 100;
        }
    }
}

// ��������ÿһ�����������ڻ�
void columnVectorInnerProduct() {


    for (int col = 0; col < N; ++col) {
        for (int row = 0; row < N; ++row) {
            result[col] += matrix[row][col] * vec[col];
        }
    }

}

int main() {
    cout << "Matrix size: " << N << " x " << N << endl;

    generateRandomData();

    // ��ʼ��ʱ
    auto start = high_resolution_clock::now();
    for (int i = 0;i < t;i++) {
        columnVectorInnerProduct();
    }
    // ������ʱ
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);



    cout << "Execution time: " << duration.count() / t << " ms" << endl;

    return 0;
}
