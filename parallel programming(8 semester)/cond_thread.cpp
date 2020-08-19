#include <openssl/rc2.h>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <signal.h>

using namespace std;

// static const unsigned char SEARCH_KEY[] = "1234567890000000";
// static const unsigned char SEARCH_KEY[] = "0000000010123000"; 
static const unsigned char SEARCH_KEY[] = "7500000011200000";

unsigned char* encriptRC2(unsigned char* text, const unsigned char* key, const int length_key)
{
    unsigned char* out = new unsigned char[8];
    RC2_KEY wctx;
    RC2_set_key(&wctx, length_key, key, 8 * length_key);
    RC2_ecb_encrypt(text, out, &wctx, RC2_ENCRYPT);
    return out;
}
unsigned char* decryptRC2(unsigned char* cipher, const unsigned char* key, const int length_key)
{
    unsigned char* decout = new unsigned char[8];
    RC2_KEY wctx;
    RC2_set_key(&wctx, length_key, key, 8 * length_key);
    RC2_ecb_encrypt(cipher, decout, &wctx, RC2_DECRYPT);

    return decout;
}

bool isEqualChiper(char* first_chiper, char* second_chiper)
{
    if (!memcmp(first_chiper, second_chiper, 16))
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
pthread_mutex_t mutex;
pthread_cond_t cv;

struct thread_params_st
{
    int thread_num;
    long long start;
    long long end;
};
void* iterateOverTheKeys(void* arg)
{
    thread_params_st* task = (thread_params_st*)arg;
    unsigned char text[] = "AAABBAAA";
    unsigned char* desired_cipher{ encriptRC2(text, SEARCH_KEY, 16) };

    pthread_mutex_lock(&mutex);
    cout << "Thread " << (*task).thread_num << " start work with borders: " << (*task).start << " - " << (*task).end << endl;
    pthread_mutex_unlock(&mutex);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    for (long long i = (*task).start; i < (*task).end; i++)
    {
        pthread_testcancel();

        char current_key[17];
        current_key[16] = 0;

        transferNumberToString(i, current_key, 15);

        unsigned char* current_cipher{ encriptRC2(text, (unsigned char*)current_key, 16) };

        if (isEqualChiper((char*)desired_cipher, (char*)current_cipher))
        {
            pthread_mutex_lock(&mutex);
            cout << "Find correct key: " << current_key << endl;
            cout << "Chipper: " << current_cipher << endl;
            pthread_cond_signal(&cv);
            pthread_mutex_unlock(&mutex);

            delete[] current_cipher;
            delete[] desired_cipher;
            break;
        }
        delete[] current_cipher;
    }
    pthread_mutex_lock(&mutex);
    cout << "Thread " << (*task).thread_num << " finished work" << endl;
    pthread_mutex_unlock(&mutex);
}
const int NUMBER_OF_THREADS = 4;
thread_params_st params[NUMBER_OF_THREADS];
long long  KEY_BOARDER = 10000000000000000;
int main(int argc, char* argv[])
{

    pthread_t thread[NUMBER_OF_THREADS];
    int status[NUMBER_OF_THREADS];
    int status_addr[NUMBER_OF_THREADS];
    int status_signal;
    pthread_mutex_init(&mutex, 0);
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        params[i].thread_num = i;
        params[i].start = KEY_BOARDER * i / NUMBER_OF_THREADS;
        params[i].end = KEY_BOARDER * (i + 1) / NUMBER_OF_THREADS;
        status[i] = pthread_create(&thread[i], NULL, iterateOverTheKeys, &params[i]);

        if (status[i] != 0)
            printf("main error: create %d\n", status[i]);
    }
    pthread_cond_wait(&cv, &mutex);
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        cout << "cancel thread: " << thread[i] << endl;
        pthread_cancel(thread[i]);
    }

    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        status[i] = pthread_join(thread[i], (void**)&status_addr[i]);
    }
    return 0;
}
