#include <Windows.h>
#include "EncryptionRC2.h"
#include <thread>
#include <iostream>
#include <omp.h>

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


bool FLAG = false;

const int NUMBER_OF_THREADS = 4;
long long  KEY_BOARDER = 10000000000000000;


int main()
{

    unsigned char text[] = "AAABBAAA";
    unsigned char* desired_cipher{ encriptRC2(text, SEARCH_KEY, strlen((char*)SEARCH_KEY)) };

    omp_set_num_threads(NUMBER_OF_THREADS);

    cout << "start programm" << endl;

    #pragma omp parallel for schedule(static)
    for (long long i = 0; i < KEY_BOARDER; i++)
    {
        if (FLAG)
            break;

        char current_key[17];
        current_key[16] = 0;

        transferNumberToString(i, current_key, 15);

        unsigned char* current_cipher{ encriptRC2(text, (unsigned char*)current_key, 16) };

        if (isEqualChiper((char*)desired_cipher, (char*)current_cipher))
        {
            #pragma omp critical
            {
                cout << "Find correct key: " << current_key << endl;
                cout << "Chipper: " << current_cipher << endl;
                FLAG = true;
            }

            break;
        }

    }


    return 0;
}