#include<iostream>
#include<Windows.h>
#include<stdlib.h>      
#include<time.h>   
using namespace std;

#define BUFFER_SIZE 10;//��������СΪ10
HANDLE Mutex, Empty, Full;//�����ź���Mutex,��Դ�ź���Empty,Full
int producerPtr = 0, consumerPtr = 0;//�����ɾ����Ԫ���ڻ����������
int buffer[10];//��ʾ������

void output() {//���������
	cout << "������:";
	for (int i = 0; i < 10; i++)
		cout << buffer[i] << ' ';
	cout << endl;
}

bool insert(int x) {//����Ԫ��
	if (producerPtr >= consumerPtr&&producerPtr < consumerPtr + 10) {//��������δ��,��ɳɹ�����Ԫ��
		buffer[producerPtr++ % 10] = x;
		return true;
	}
	else
		return false;//�������ʧ��
}

bool remove() {//�Ƴ�Ԫ��
	if (consumerPtr <= producerPtr) {//���������ǿ���ɳɹ��Ƴ�һ��Ԫ��
		buffer[consumerPtr++ % 10] = 0;
		return true;
	}
	else
		return false;//�����Ƴ�ʧ��
}

DWORD WINAPI producer(LPVOID param)//�������߳�
{
	srand(3 * time(0) + GetCurrentThreadId());
	do {
		int random = rand() % 100 + 1;
		Sleep(random);//����ȴ�1~100ms
		WaitForSingleObject(Empty, INFINITE);//������������,������ǰ�������߳�
		WaitForSingleObject(Mutex, INFINITE);//ʵ�ֶԻ������Ļ������
		cout << "--------------------------------" << endl;
		cout << "Producer " << GetCurrentThreadId() << ": insert ";
		if (insert(random))
			cout << "successfully\nproducerd " << (producerPtr - 1) % 10 + 1 << endl;
		else
			cout << "failed\n" << endl;
		output();
		cout << "--------------------------------" << endl << endl;
		ReleaseMutex(Mutex);
		ReleaseSemaphore(Full, 1, NULL);
	} while (true);
	return 0;
}

DWORD WINAPI consumer(LPVOID param)//�������߳�
{
	srand(2 * time(0) - GetCurrentThreadId());
	do {
		int random = rand() % 100 + 1;
		Sleep(random);//����ȴ�1~100ms
		WaitForSingleObject(Full, INFINITE);//���������ѿ�,������ǰ�������߳�
		WaitForSingleObject(Mutex, INFINITE);//ʵ�ֶԻ������Ļ������
		cout << "********************************" << endl;
		cout << "Consumer " << GetCurrentThreadId() << ": remove ";
		if (remove())
			cout << "successfully\nconsumed " << (consumerPtr - 1) % 10 + 1 << endl;
		else
			cout << "failed\n" << endl;
		output();
		cout << "********************************" << endl << endl;
		ReleaseMutex(Mutex);
		ReleaseSemaphore(Empty, 1, NULL);
	} while (true);
	return 0;
}

int main() {
	int SleepTime, NumOfProducer, NumOfConsumer;
	cout << "����������˯��ʱ��(millisecond)" << endl;
	cin >> SleepTime;
	cout << "���������߽�������(1~10)" << endl;
	cin >> NumOfProducer;
	cout << "���������߽�������(1~10)" << endl;
	cin >> NumOfConsumer;

	cout << "��ʼ��" << endl;
	memset(buffer, 0, 10);//����������ʼ��Ϊ0
	output();
	cout << endl;

	Empty = CreateSemaphore(NULL, 10, 10, NULL);
	Full = CreateSemaphore(NULL, 0, 10, NULL);
	Mutex = CreateMutex(NULL, FALSE, NULL);

	DWORD *PThreadId = new DWORD[NumOfProducer];
	DWORD *CThreadId = new DWORD[NumOfConsumer];
	HANDLE *PThreadHandle = new HANDLE[NumOfProducer];
	HANDLE *CThreadHandle = new HANDLE[NumOfConsumer];

	for (unsigned short i = 0; i < NumOfProducer; i++)//�����������߳�
	{
		PThreadId[i] = i;
		PThreadHandle[i] = CreateThread(NULL, 0, producer, NULL, 0, &PThreadId[i]);
	}
	for (unsigned short i = 0; i < NumOfConsumer; i++)//�����������߳�
	{
		CThreadId[i] = i;
		CThreadHandle[i] = CreateThread(NULL, 0, consumer, NULL, 0, &CThreadId[i]);
	}

	Sleep(SleepTime);//���߳�˯������ʱ��
	cout << "End" << endl;
	return 0;
}