#include <Windows.h>
#include "EncryptionRC2.h"
#include "EncryptionAES.h"
#include <thread>
#include <iostream>

using namespace std;

//static const unsigned char SEARCH_KEY[] = "1234567890000000";
//static const unsigned char SEARCH_KEY[] = "0000000000012345";
static const unsigned char SEARCH_KEY[] = "2500000010007678";


bool isEqualChiper(char* first_chiper, char* second_chiper)
{
    if (!strcmp(first_chiper, second_chiper))
        return true;

    return false;
}

void transferNumberToString(long long number, char* str, int length)
{
    for (int i(length); i >= 0; i--)
    {
        str[i] = (number % 10) + '0';
        number /= 10;
    }

}


struct thread_params_st
{
    int thread_num;
    long long start;
    long long end;
};


DWORD correct_pass;

DWORD finished = 0;
CRITICAL_SECTION sc;

bool FLAG = false;

DWORD __stdcall iterateOverTheKeys(void* arg)
{
    thread_params_st* task = (thread_params_st*)arg;

    unsigned char text[] = "AAABBAAA";
    unsigned char* desired_cipher{ encriptRC2(text, SEARCH_KEY, strlen((char*)SEARCH_KEY)) };

    EnterCriticalSection(&sc);
    cout << "Thread " << (*task).thread_num << " start work with borders: " << (*task).start << " - " << (*task).end << endl;
    LeaveCriticalSection(&sc);


    for (long long i = (*task).start; i < (*task).end; i++)
    {
        if (FLAG)
            break;

        char current_key[17];
        current_key[16] = 0;

        transferNumberToString(i, current_key, 15);

        unsigned char* current_cipher{ encriptRC2(text, (unsigned char*)current_key, 16) };

        if (isEqualChiper((char*)desired_cipher, (char*)current_cipher))
        {   
            EnterCriticalSection(&sc);
            cout << "Find correct key: " << current_key << endl;
            cout << "Chipper: " << current_cipher << endl;
            FLAG = true;
            LeaveCriticalSection(&sc);
            
            break;
        }

    }

    EnterCriticalSection(&sc);
    cout << "Thread " << (*task).thread_num << " finished work" << endl;
    LeaveCriticalSection(&sc);

    InterlockedIncrement(&finished);
    

    return 1;
}

const int NUMBER_OF_THREADS = 4;
HANDLE handles[NUMBER_OF_THREADS];
DWORD thid[NUMBER_OF_THREADS];
thread_params_st params[NUMBER_OF_THREADS];

long long  KEY_BOARDER = 10000000000000000;


int main()
{
    InitializeCriticalSection(&sc);

    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        params[i].thread_num = i;
        params[i].start = KEY_BOARDER * i / NUMBER_OF_THREADS;
        params[i].end = KEY_BOARDER * (i + 1) / NUMBER_OF_THREADS;

        handles[i] = CreateThread(NULL, 0, iterateOverTheKeys, &params[i], 0, &thid[i]);

        if (!handles[i])
            printf("Error 1: %d\n", GetLastError());
    }

    WaitForMultipleObjects(NUMBER_OF_THREADS, handles, true, INFINITE);

    DeleteCriticalSection(&sc);

    cout << finished << endl;

    return 0;
}