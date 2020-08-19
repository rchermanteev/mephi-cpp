#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <openssl/rc2.h>
#include <string.h>
#include <fstream> 


using namespace std;

//static const unsigned char SEARCH_KEY[] = "1234567890000000";
static const unsigned char SEARCH_KEY[] = "0000000010123000"; 
//static const unsigned char SEARCH_KEY[] = "7500000001200000";
// static const unsigned char SEARCH_KEY[] = "5000000001200000";

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


const int NUMBER_OF_PROCESS = 4;
long long  KEY_BOARDER = 10000000000000000;
//const char FILE_NAME_LOG[] = "log.txt";
const char FILE_NAME_RESULT[] = "result.txt"; ///home/rammi/CPP_projects/MEPHI_project/parallel_programming/process/


int main()
{
	pid_t cpid;
	vector<pid_t> started_processes;
	started_processes.reserve(NUMBER_OF_PROCESS);


	for (int num_process = 0; num_process < NUMBER_OF_PROCESS; num_process++)
	{
		cpid = fork();

		
		if (cpid == -1)
		{
			cout << "fork faild: " << num_process << endl;
		}

		if (cpid == 0)
		{
			// Strt child process
			sleep(2);

	        long long start = KEY_BOARDER * num_process / NUMBER_OF_PROCESS;
	        long long end = KEY_BOARDER * (num_process + 1) / NUMBER_OF_PROCESS;

	        cout << "start child process: " << cpid << "  " << start << "  " << end << endl;
	        
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
	                   
	                ofstream out;
	                out.open(FILE_NAME_RESULT);
	                out << num_process << "  " << start << "  " << end << endl;
	             	out << "Find correct key: " << current_key << endl;
	             	out << "Chipper: " << current_cipher << endl;
	                out.close();

	                break;
	            }
	            
	        }


			break;
		}
		else
		{
			// Continue parent process
			cout << "In parent: " << cpid << endl;
			started_processes.push_back(cpid);
		}


	}

	if (cpid != 0)
	{

		cout << "size vector: " << started_processes.size() << endl;

		while (true)
		{
			if (!access(FILE_NAME_RESULT, 0))
			{
				
				printf("File Present\n");

				for(int num_process=0; num_process < NUMBER_OF_PROCESS; num_process++)
				{

					int kill_result = kill(started_processes[num_process], SIGKILL);
					
					if (kill_result)
						cout << "failed to kill process " << started_processes[num_process] << endl;
					else 
						cout << "killed the process " << started_processes[num_process] << endl;

				}

				break;
			}
		}

	}

	return 0;
}