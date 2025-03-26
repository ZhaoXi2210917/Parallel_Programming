#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;
using namespace chrono;

// ���������С��n��
const int N = 1 << 19;         // �����൱�� 2^15 = 32768
// �����ܲ�����
const long long times = 100000000000LL;  
// ÿ�� sumArray() ���� N �μӷ�������ѭ������ t = times / N
const long long t = times / N;

// ȫ�����飬��������������
static long long arr[N];

// �����������
void generateRandomData() {
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < N; ++i) {
        arr[i] = rand() % 100;  // 0~99 ���������
    }
}

// ���� n �����ĺ�
long long sumArray() {
    long long sum = 0;
    for (int i = 0; i < N; i++) {
        sum += arr[i];
    }
    return sum;
}

int main() {
    cout << "Array size: " << N << endl;

    // 1. �����������
    generateRandomData();

    // 2. ��ʼ��ʱ
    auto start = high_resolution_clock::now();
    
    // 3. �ظ� t �Σ�ģ���ܲ�����
    long long finalResult = 0; // �����洢�������ֹ�����������Ż�
    for (int i = 0; i < t; i++) {
        finalResult += sumArray();
    }

    // 4. ������ʱ
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    // 5. ���ƽ����ʱ��ms��
    double avgTimeMs = static_cast<double>(duration.count()) / t;
    cout << "Execution time: " << avgTimeMs << " ms" << endl;

    // ��� finalResult�����ⱻ�������Ż���
    cout << "Check result: " << finalResult << endl;

    return 0;
}
