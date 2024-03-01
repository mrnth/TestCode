#include <signal.h>
#include <setjmp.h>
#include <chrono>
#include <iostream>
#include <atomic>
#include <unistd.h>
#include "TestIPC.hpp"

using namespace std;
using namespace chrono;
using namespace TestIpcShareMem;
using namespace TestIpcMsgQueue;

struct sigaction sigIntHandler;
volatile std::atomic<bool> program_running;
void signalHandler(int signal)
{
	std::cout << "Child process 1, PID: " << getpid() << ", signal: " << signal << std::endl;
	program_running = false;
}

int main(int argc, char **argv)
{
	program_running = true;
	sigIntHandler.sa_handler = signalHandler;
	sigfillset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
	// sigaction(SIGSEGV, &sigIntHandler, NULL);
	sigaction(SIGILL, &sigIntHandler, NULL);
	sigaction(SIGFPE, &sigIntHandler, NULL);
	sigaction(SIGABRT, &sigIntHandler, NULL);
	sigaction(SIGFPE, &sigIntHandler, NULL);
	sigaction(SIGTERM, &sigIntHandler, NULL);
	sigaction(SIGKILL, &sigIntHandler, NULL);

	std::unique_ptr<TestShareMem> mTestShareMem = std::make_unique<TestShareMem>();
	std::unique_ptr<TestMsgQueue> mTestMsgQueue = std::make_unique<TestMsgQueue>();

	while (program_running == true)
	{
		// std::cout << "--------------Child process 1, PID: " << getpid() << std::endl;

		// mTestShareMem->WriteInt(10);

		// std::string test_string = "Hi sharemem from proc 1";
		// mTestShareMem->WriteString(test_string);

		// std::vector<float> test_vector = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
		// mTestShareMem->WriteVector(test_vector);

		ShareMemData test_obj;
		test_obj.float_value = 9.8;
		test_obj.int_value = 6;
		test_obj.is_new = true;
		strcpy(test_obj.string_value, "Hello share mem obj from proc 1");
		mTestShareMem->WriteObject(test_obj);

		// mTestMsgQueue->SendMsg();
		// mTestMsgQueue->GetQueueInfo();

		usleep(900000);
	}
	return 0;
}
