#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;
using namespace chrono;

// ���������С��n��
const int N = 1 << 15;         // �����൱�� 2^15 = 32768
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

// ��̬�滮���� n �����ĺ�
long long sumArray() {
    for (int m = N; m > 1; m /= 2)
    {
        for (int i = 0; i < m / 2; i++)
        {
            arr[i] = arr[i * 2] + arr[i * 2 + 1];
        }
    }
    return arr[0];
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
