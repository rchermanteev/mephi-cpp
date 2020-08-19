#include<mpi.h>
#include<iostream>
#include <Windows.h>
#include "EncryptionRC2.h"

using namespace std;


//static const unsigned char SEARCH_KEY[] = "1234567890000000";
static const unsigned char SEARCH_KEY[] = "0000000000012345";
//static const unsigned char SEARCH_KEY[] = "2500000010007678";

long long  KEY_BOARDER = 10000000000000000;


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

void actionChildProcess(int number_of_process, int process_number)
{
    
    int child_process_number = process_number - 1;
    int number_of_child_process = number_of_process - 1;

    cout << "The number of child processes: " << number_of_child_process << " my number is " << child_process_number << endl;

    long long start = KEY_BOARDER * child_process_number / number_of_child_process;
    long long end = KEY_BOARDER * (child_process_number + 1) / number_of_child_process;

    cout << child_process_number << "  " << start << "  " << end << endl;

    unsigned char text[] = "AAABBAAA";
    unsigned char* desired_cipher{ encriptRC2(text, SEARCH_KEY, strlen((char*)SEARCH_KEY)) }; 
        
    for (long long i = start; i < end; i++)
    {

        char current_key[17];
        current_key[16] = 0;
        
        transferNumberToString(i, current_key, 15);
        
        unsigned char* current_cipher{ encriptRC2(text, (unsigned char*)current_key, 16) };
        
        if (isEqualChiper((char*)desired_cipher, (char*)current_cipher))
        {   

            cout << process_number << "  " << start << "  " << end << endl;
            cout << "Find correct key: " << current_key << endl;
            cout << "Chipper: " << current_cipher << endl;

            MPI_Send(current_key, sizeof(current_key), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
       
            break;
        }
            
    }


}

void actionRootProcess()
{
    MPI_Status status;  
    char received_key[17];
    received_key[16] = 0;

    MPI_Recv(received_key, sizeof(received_key), MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    cout << "Child process: " << status.MPI_SOURCE << " Find correct key: " << received_key << endl;

}

int main(int argc, char** argv)
{

    int process_number, number_of_process;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_number);

    if (process_number == 0)
    {
        // root process 
        cout << "The number of processes: " << number_of_process << " my number is " << process_number << " i am root" << endl;
        actionRootProcess();
        MPI_Abort(MPI_COMM_WORLD, 1);

    }
    else
    {
        // child process
        actionChildProcess(number_of_process, process_number);

    }

    MPI_Finalize();
    return 0;

}