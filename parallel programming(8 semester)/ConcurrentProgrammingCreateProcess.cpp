#include <Windows.h>
#include "EncryptionRC2.h"
#include <iostream>
#include <fstream>
#include <tchar.h>
#include "atlconv.h" 
#include <io.h>


using namespace std;

//static const unsigned char SEARCH_KEY[] = "1234567890000000";
//static const unsigned char SEARCH_KEY[] = "0000000000012345";
static const unsigned char SEARCH_KEY[] = "2500000000017678";


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

long long  KEY_BOARDER = 10000000000000000;
const int NUMBER_OF_PROCESS = 4;

const char FILE_NAME_LOG[] = "log.txt";
const char FILE_NAME_RESULT[] = "result.txt";


int main(int argc, char* argv[])
{

    if (argc == 1) // main process
    {
        BOOL process[NUMBER_OF_PROCESS];
        HANDLE handles[NUMBER_OF_PROCESS];
        STARTUPINFO si[NUMBER_OF_PROCESS];
        PROCESS_INFORMATION pi[NUMBER_OF_PROCESS];

        for (int i = 0; i < NUMBER_OF_PROCESS; i++)
        {

            char buf[10000];

            sprintf(buf, "\"%s\" %d %s", argv[0], i, SEARCH_KEY);

            wchar_t* wbuf = 0;

            USES_CONVERSION;

            // Convert Ansi to Unicode. 
            wbuf = A2W(buf);
            
            ZeroMemory(&si[i], sizeof(si[i]));
            si[i].cb = sizeof(si[i]);
            ZeroMemory(&pi[i], sizeof(pi[i]));

            if (!CreateProcess(NULL, wbuf, NULL, NULL, FALSE, 0, NULL, NULL, &si[i], &pi[i]))
            {
                cout << "error: create process" << endl;
            }

            handles[i] = pi[i].hProcess;
        }

        while (true)
        {
            if (!_access(FILE_NAME_RESULT, 0))
            {
                printf("File Present\n");

                for (int i = 0; i < NUMBER_OF_PROCESS; i++)
                    TerminateProcess(pi[i].hProcess, NO_ERROR);

                break;
            }
        }
        
        WaitForMultipleObjects(NUMBER_OF_PROCESS, handles, true, INFINITE);

        for (int i = 0; i < NUMBER_OF_PROCESS; i++)
        {
            CloseHandle(pi[i].hProcess);
            CloseHandle(pi[i].hThread);
        }
        
        cout << "compleate work" << endl;
    }
    else
    {
        // child process
        int process_number = atoi(argv[1]);

        long long start = KEY_BOARDER * process_number / NUMBER_OF_PROCESS;
        long long end = KEY_BOARDER * (process_number + 1) / NUMBER_OF_PROCESS;


        ofstream out(FILE_NAME_LOG, ios::app);
        if (out.is_open())
        {
            out << process_number << "  " << start << "  " << end << endl;
        }
        out.close();

        
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
                ofstream out(FILE_NAME_RESULT, ios::app);
                if (out.is_open())
                {
                    out << process_number << "  " << start << "  " << end << endl;
                    out << "Find correct key: " << current_key << endl;
                    out << "Chipper: " << current_cipher << endl;
                }
                out.close();
                    
                break;
            }
            
        }


    }
    
    return 0;
}