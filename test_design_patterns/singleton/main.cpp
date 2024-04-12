#include <iostream>
#include <mutex>

// Khóa để đảm bảo tính an toàn của việc tạo Singleton trong môi trường đa luồng
static std::mutex mtx;

class Singleton
{
private:
	// Constructor riêng để ngăn chặn việc tạo ra các đối tượng mới từ bên ngoài
	Singleton()
	{
		std::cout << "Singleton được khởi tạo." << std::endl;
	}

public:
	// Phương thức để lấy đối tượng duy nhất của lớp Singleton
	static Singleton *getInstance()
	{
		// Đảm bảo tính an toàn trong môi trường đa luồng
		std::lock_guard<std::mutex> lock(mtx);

		static Singleton instance;
		return &instance;
	}

	// Phương thức hủy (destructor)
	~Singleton()
	{
		std::cout << "Singleton được hủy." << std::endl;
	}

	// Một số phương thức và dữ liệu khác cho lớp Singleton
	void doSomething()
	{
		std::cout << "Làm một việc gì đó với Singleton." << std::endl;
	}
};

int main()
{
	// Truy cập đến đối tượng duy nhất của Singleton thông qua getInstance
	Singleton *singleton = Singleton::getInstance();

	// Gọi phương thức của Singleton
	singleton->doSomething();

	return 0;
}
