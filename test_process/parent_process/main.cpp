#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <vector>

using namespace std;

int num_children = 0;
struct sigaction sigIntHandler;

void signalHandler(int signal)
{
	// Reap all terminated child processes
	while (waitpid(-1, nullptr, WNOHANG) > 0)
		num_children--;

}

int main(int argc, char **argv)
{
	sigIntHandler.sa_handler = signalHandler;
	sigfillset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
	sigaction(SIGSEGV, &sigIntHandler, NULL);
	sigaction(SIGILL, &sigIntHandler, NULL);
	sigaction(SIGFPE, &sigIntHandler, NULL);
	sigaction(SIGABRT, &sigIntHandler, NULL);
	sigaction(SIGFPE, &sigIntHandler, NULL);
	sigaction(SIGTERM, &sigIntHandler, NULL);
	sigaction(SIGKILL, &sigIntHandler, NULL);

	std::cout << "Parent process, PID: " << getpid() << std::endl;

	// Create child process
	const int num_proc = 2;
	vector<pid_t> mListPid(2);
	for (int proc_i = 0; proc_i < num_proc; ++proc_i)
	{
		std::cout << "Parent process create child process" << proc_i << std::endl;
		mListPid[proc_i] = fork();
		if (mListPid[proc_i] == 0)
		{
			// Child process
			switch (proc_i)
			{
			case 0:
			{
				char *argv_list[] = {"input0", NULL};
				execv("../child_process_0/child_process.elf", argv_list);
				break;
			}
			case 1:
			{
				char *argv_list[] = {"input1", NULL};
				execv("../child_process_1/child_process.elf", argv_list);
				break;
			}
			default:
				break;
			}
			return 0;
		}
		else if (mListPid[proc_i] < 0)
		{
			std::cerr << "Fork failed" << std::endl;
			return 1;
		}
		else
		{
			num_children++; // Increment the count of child processes created
		}
	}
	sleep(10);

	while (num_children > 0)
	{
		std::cout << "-------------------------------" << std::endl;
		for (size_t i = 0; i < mListPid.size(); i++)
		{
			if (mListPid[i] > 0)
			{
				if (kill(mListPid[i], SIGQUIT) != 0)
				{
					std::cout << "Success. Parent proc send signal to proc " << mListPid[i] << std::endl;
					std::cout << "num_child = " << num_children << std::endl;
				}
			}
			sleep(1);
		}
		sleep(3);
	}

	return 0;
}