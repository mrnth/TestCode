#include <iostream>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <memory>
#include <vector>
#include <mqueue.h>
#include <cstring>

namespace TestIPC
{
	using namespace boost::interprocess;
	using namespace std;

	class TestSemaphore
	{
	private:
		boost::interprocess::interprocess_semaphore mMutex(1);

	public:
		TestSemaphore();
		~TestSemaphore();

		void Lock();
		void UnLock();
	};

	TestSemaphore::TestSemaphore()
	{
	}

	TestSemaphore::~TestSemaphore()
	{
	}

	void TestSemaphore::Lock()
	{
		mMutex.wait(); // Chờ semaphore
	}

	void TestSemaphore::UnLock()
	{
		mMutex.post(); // Tăng giá trị của semaphore, giải phóng khóa
	}
}

namespace TestIPC
{
	using namespace boost::interprocess;
	using namespace std;

	class TestShareMem
	{
	public:
		TestShareMem();
		~TestShareMem();

		void WriteString();
		void WriteInt();
		void WriteVector();

		void ReadString();
		void ReadInt();
		void ReadVector();

	private:
		std::unique_ptr<boost::interprocess::mapped_region> mMappedRegion;
		std::unique_ptr<TestSemaphore> mMutex;
	};

	TestShareMem::TestShareMem()
	{
		// Tạo hoặc mở một vùng shared memory có tên "shared_memory"
		shared_memory_object share_mem_obj(open_or_create, "my_test_shared_memory", read_write);

		// Thiết lập kích thước của vùng shared memory 1000 bytes
		share_mem_obj.truncate(1000);

		// Tạo một vùng dữ liệu được ánh xạ cho shared memory
		mMappedRegion = make_unique<mapped_region>(share_mem_obj, read_write);

		mMutex = make_unique<TestSemaphore>();
	}

	TestShareMem::~TestShareMem()
	{
		shared_memory_object::remove("my_test_shared_memory");
	}

	void TestShareMem::WriteString()
	{
		// Lấy địa chỉ bắt đầu của vùng dữ liệu được ánh xạ
		void *addr = mMappedRegion->get_address();

		std::string data = "Hello from parent proc";

		mMutex->Lock();
		// Ghi dữ liệu vào vùng dữ liệu được ánh xạ
		std::strcpy(static_cast<char *>(addr), data.c_str());
		mMutex->UnLock();

		std::cout << "WriteString Done" << std::endl;
	}

	void TestShareMem::WriteInt()
	{
		// Lấy địa chỉ bắt đầu của vùng dữ liệu được ánh xạ và chuyển đổi thành con trỏ int*
		int *int_ptr = static_cast<int *>(mMappedRegion->get_address());

		mMutex->Lock();
		// Gán giá trị số nguyên vào vùng dữ liệu được ánh xạ
		*int_ptr = 123;
		mMutex->UnLock();

		std::cout << "WriteInt Done" << std::endl;
	}

	void TestShareMem::WriteVector()
	{
		// Lấy địa chỉ bắt đầu của vùng dữ liệu được ánh xạ và chuyển đổi thành con trỏ char*
		char *addr = static_cast<char *>(mMappedRegion->get_address());

		// Tạo vector dữ liệu mẫu
		std::vector<float> data;
		for (int i = 0; i < 10; i++)
		{
			data.push_back((float)i);
		}

		mMutex->Lock();
		// Sao chép dữ liệu từ vector vào vùng dữ liệu được ánh xạ
		std::memcpy(addr, data.data(), sizeof(float) * data.size());
		mMutex->UnLock();

		std::cout << "WriteVector Done" << std::endl;
	}

	void TestShareMem::ReadString()
	{
		// Lấy địa chỉ bắt đầu của vùng dữ liệu được ánh xạ
		void *addr = mMappedRegion->get_address();

		mMutex->Lock();
		std::string data = static_cast<char *>(addr);
		mMutex->UnLock();

		// In dữ liệu được đọc từ shared memory
		std::cout << "Value read from shared memory: " << data << std::endl;
	}

	void TestShareMem::ReadInt()
	{
		// Lấy địa chỉ bắt đầu của vùng dữ liệu được ánh xạ và chuyển đổi thành con trỏ int*
		int *int_ptr = static_cast<int *>(mMappedRegion->get_address());

		mMutex->Lock();
		int data = *int_ptr;
		mMutex->UnLock();

		// Đọc giá trị số nguyên từ vùng dữ liệu được ánh xạ và in ra màn hình
		std::cout << "Value read from shared memory: " << data << std::endl;
	}

	void TestShareMem::ReadVector()
	{
		// Lấy địa chỉ bắt đầu của vùng dữ liệu được ánh xạ và chuyển đổi thành con trỏ float*
		float *float_ptr = static_cast<float *>(mMappedRegion->get_address());

		// Số lượng phần tử trong vector
		int size = 10; // Giả sử số lượng phần tử là 10 (đã biết trước)

		mMutex->Lock();
		// Đọc dữ liệu từ vùng dữ liệu được ánh xạ và lưu vào vector
		std::vector<float> data(float_ptr, float_ptr + size);
		mMutex->UnLock();

		// In dữ liệu từ vector ra màn hình
		std::cout << "Data read from shared memory:" << std::endl;
		for (const auto &value : data)
		{
			std::cout << value << " ";
		}
		std::cout << std::endl;
	}
}

namespace TestIPC
{
	using namespace boost::interprocess;
	using namespace std;

	// Định nghĩa cấu trúc cho Msg Queue
	struct QueueData
	{
		int int_value;
		float float_value;
		char string_value[100];
	};

	class TestMsgQueue
	{
	private:
		std::string mQueueName = "/my_queue";
		int mMaxMsgSize = sizeof(QueueData);
		int mMaxMsgCount = 10;
		mqd_t mMqdt;

	public:
		TestMsgQueue();
		~TestMsgQueue();

		void SendMsg();
		void ReceiveMsg();
		void GetQueueInfo();
	};

	/*
	mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);
	name: Tên của message queue. Trong Unix, message queue cũng được coi là một file nên cần phải có một tên định danh duy nhất.
	oflag: Cờ mở message queue. Các cờ có thể bao gồm:
	O_RDONLY: Mở message queue chỉ để đọc.
	O_WRONLY: Mở message queue chỉ để ghi.
	O_RDWR: Mở message queue để đọc và ghi.
	O_CREAT: Tạo message queue nếu nó chưa tồn tại.
	O_EXCL: Khi được sử dụng với O_CREAT, nếu message queue đã tồn tại, hàm mq_open sẽ fail và trả về lỗi EEXIST.
	mode: Quyền truy cập cho message queue mới nếu cờ O_CREAT được sử dụng.
	attr: Con trỏ đến một struct mq_attr chứa các thuộc tính của message queue như kích thước tối đa, số lượng tin nhắn tối đa, v.v.
	Hàm mq_open sẽ trả về một message queue descriptor (mqd_t). Nếu có lỗi, nó sẽ trả về -1 và set biến toàn cục errno để chỉ ra lý do lỗi.
*/
	TestMsgQueue::TestMsgQueue()
	{
		// Tạo message queue attributes
		struct mq_attr attr;
		attr.mq_flags = O_NONBLOCK;
		attr.mq_maxmsg = mMaxMsgCount;
		attr.mq_msgsize = mMaxMsgSize;
		attr.mq_curmsgs = 0;

		// Mở hoặc tạo message queue
		mMqdt = mq_open(mQueueName.c_str(), O_CREAT | O_RDWR | O_NONBLOCK, 0666, &attr);
		if (mMqdt == -1)
		{
			std::cerr << "Error opening message queue\n";
			abort();
		}
	}

	TestMsgQueue::~TestMsgQueue()
	{
		mq_close(mMqdt);
		mq_unlink(mQueueName.c_str());
	}

	void TestMsgQueue::SendMsg()
	{
		QueueData queue_data;
		queue_data.float_value = 0.1;
		queue_data.int_value = 69;
		strcpy(queue_data.string_value, "Hello, message queue!");

		// Chuyển đổi cấu trúc thành mảng byte
		char buffer[sizeof(QueueData)];
		memcpy(buffer, &queue_data, sizeof(QueueData));

		/*
		Gửi tin nhắn chứa cấu trúc qua message queue
		int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int msg_prio);
		mqdes: Message queue descriptor, được trả về từ hàm mq_open.
		msg_ptr: Con trỏ tới dữ liệu của tin nhắn.
		msg_len: Kích thước của tin nhắn.
		msg_prio: Ưu tiên của tin nhắn. Các message queue có thể hỗ trợ các cấp độ ưu tiên khác nhau, nhưng có thể không được hỗ trợ trên tất cả các hệ thống.
		Hàm mq_send trả về 0 nếu thành công. Nếu có lỗi, nó trả về -1 và set biến toàn cục errno để chỉ ra lý do lỗi.
		*/
		if (mq_send(mMqdt, buffer, sizeof(QueueData), 0) == -1)
			std::cout << "mq_send error" << std::endl;
	}

	void TestMsgQueue::ReceiveMsg()
	{
		QueueData received_data;
		ssize_t bytes_read = mq_receive(mMqdt, (char *)&received_data, sizeof(QueueData), NULL);
		if (bytes_read != -1)
		{
			std::cout << "----------------------- mq_receive success: " << std::endl;
			std::cout << received_data.float_value << std::endl;
			std::cout << received_data.int_value << std::endl;
			std::cout << received_data.string_value << std::endl;
		}
	}

	void TestMsgQueue::GetQueueInfo()
	{
		struct mq_attr mq_attr_info;
		if (mq_getattr(mMqdt, &mq_attr_info) == -1)
		{
			std::cerr << "Error getting attributes of message queue" << std::endl;
		}
		else
		{
			// In ra thông tin
			std::cout << "----------------------------Message queue attributes:" << std::endl;
			std::cout << "\tMaximum number of messages: " << mq_attr_info.mq_maxmsg << std::endl;
			std::cout << "\tMaximum message size: " << mq_attr_info.mq_msgsize << " bytes" << std::endl;
			std::cout << "\tNumber of messages currently in queue: " << mq_attr_info.mq_curmsgs << std::endl;
		}
	}
}
