#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>

#include "TestIPC.hpp"

using namespace TestIpcShareMem;
using namespace TestIpcMsgQueue;

int num_children = 0;
struct sigaction sigIntHandler;
std::vector<pid_t> mListPid(2);

void signalHandler(int signal)
{
	// Reap all terminated child processes
	while (num_children > 0)
	{
		int pid = waitpid(0, nullptr, 0);
		std::cout << "pid = " << pid << std::endl;
		if (pid > 0)
		{
			num_children--;
		}
		std::cout << "num child = " << num_children << std::endl;
	}
}

int main(int argc, char **argv)
{
	sigIntHandler.sa_handler = signalHandler;
	sigfillset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
	// sigaction(SIGSEGV, &sigIntHandler, NULL);
	sigaction(SIGFPE, &sigIntHandler, NULL);
	sigaction(SIGABRT, &sigIntHandler, NULL);
	sigaction(SIGFPE, &sigIntHandler, NULL);
	sigaction(SIGTERM, &sigIntHandler, NULL);
	sigaction(SIGKILL, &sigIntHandler, NULL);

	std::unique_ptr<TestShareMem> mTestShareMem = std::make_unique<TestShareMem>();
	std::unique_ptr<TestMsgQueue> mTestMsgQueue = std::make_unique<TestMsgQueue>();

	std::cout << "Parent process, PID: " << getpid() << std::endl;
	// Create child process
	const int num_proc = 2;
	for (int proc_i = 0; proc_i < num_proc; ++proc_i)
	{
		mListPid[proc_i] = fork();
		if (mListPid[proc_i] == 0) // Child process
		{
			switch (proc_i)
			{
			case 0:
			{
				char *argv_list[] = {NULL};
				execv("../child_process_0/child_process.exe", argv_list);
				break;
			}
			case 1:
			{
				char *argv_list[] = {NULL};
				execv("../child_process_1/child_process.exe", argv_list);
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
			num_children++; // Increment the count of child processes created
	}
	sleep(3);

	// int num_msg = 0;
	while (num_children > 0)
	{
		// mTestShareMem->ReadInt();
		// mTestShareMem->ReadString();
		// mTestShareMem->ReadVector();
		mTestShareMem->ReadObject();

		// mTestMsgQueue->ReceiveMsg();

		usleep(450000);
	}

	return 0;
}