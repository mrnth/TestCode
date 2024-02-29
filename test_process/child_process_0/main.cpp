#include <signal.h>
#include <setjmp.h>
#include <chrono>
#include <iostream>
#include <atomic>
#include <unistd.h>
#include <mqueue.h>
#include <cstring>

#define QUEUE_NAME "/my_queue"
#define MAX_MSG_SIZE 1024
#define MAX_MSG_COUNT 10

using namespace std;
using namespace chrono;

struct sigaction sigIntHandler;
volatile std::atomic<bool> program_running;
void signalHandler(int signal)
{
	std::cout << "Child process 0, PID: " << getpid() << ", signal: " << signal << std::endl;
	if (signal == SIGINT) // Ctrl-C
	{
		program_running = false;
	}
}

int main(int argc, char **argv)
{
	program_running = true;
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

	// Tạo message queue attributes
	struct mq_attr attr;
	attr.mq_flags = O_NONBLOCK;
	attr.mq_maxmsg = MAX_MSG_COUNT;
	attr.mq_msgsize = MAX_MSG_SIZE;
	attr.mq_curmsgs = 0;

	mqd_t mq;

	// Mở hoặc tạo message queue
	mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
	if (mq == -1)
	{
		std::cerr << "Error opening message queue\n";
		return 1;
	}

	char buffer[MAX_MSG_SIZE];
	memset(buffer, 0, sizeof(buffer));

	while (program_running == true)
	{
		// Nhận thông điệp từ message queue
		mq_receive(mq, buffer, MAX_MSG_SIZE, NULL);

		std::cout << "Child process 0, PID: " << getpid() << std::endl;
		std::cout << buffer << std::endl;
		sleep(3);
	}

	mq_close(mq);
	mq_unlink(QUEUE_NAME);

	return 0;
}
