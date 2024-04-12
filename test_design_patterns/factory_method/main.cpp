#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Lớp sản phẩm cơ bản (base product)
class Product
{
public:
	Product()
	{
		std::cout << "Constructor Product" << std::endl;
	}

	virtual ~Product()
	{
		std::cout << "Destructor Product" << std::endl;
	}

	virtual void GetInfo() const = 0; // Hàm ảo thuần túy phải được các lớp dẫn xuất triển khai (implement)
};

// Lớp sản phẩm cụ thể A (concrete product A)
class ConcreteProductA : public Product
{
public:
	ConcreteProductA()
	{
		std::cout << "Constructor ConcreteProductA" << std::endl;
	}

	virtual ~ConcreteProductA()
	{
		std::cout << "Destructor ConcreteProductA" << std::endl;
	}

	void GetInfo() const override
	{
		std::cout << "name: " << mName << std::endl;
		std::cout << "price: " << mPrice << std::endl;
	}

private:
	std::string mName = "product A";
	int mPrice = 10000;
};

// Lớp sản phẩm cụ thể B (concrete product B)
class ConcreteProductB : public Product
{
public:
	ConcreteProductB()
	{
		std::cout << "Constructor ConcreteProductB" << std::endl;
	}

	virtual ~ConcreteProductB()
	{
		std::cout << "Destructor ConcreteProductB" << std::endl;
	}

	void GetInfo() const override
	{
		std::cout << "name: " << mName << std::endl;
		std::cout << "price: " << mPrice << std::endl;
	}

private:
	std::string mName = "product B";
	int mPrice = 20000;
};

// Lớp sản phẩm cụ thể C (concrete product C)
class ConcreteProductC : public Product
{
public:
	ConcreteProductC()
	{
		std::cout << "Constructor ConcreteProductC" << std::endl;
	}

	virtual ~ConcreteProductC()
	{
		std::cout << "Destructor ConcreteProductC" << std::endl;
	}

	void GetInfo() const override
	{
		std::cout << "name: " << mName << std::endl;
		std::cout << "price: " << mPrice << std::endl;
	}

private:
	std::string mName = "product C";
	int mPrice = 30000;
};

// Lớp nhà máy cơ bản (base factory)
class Factory
{
public:
	enum ProductType
	{
		TYPE_A,
		TYPE_B,
		TYPE_C
	};

public:
	Factory()
	{
		std::cout << "Constructor Factory" << std::endl;
	}

	virtual ~Factory()
	{
		std::cout << "Destructor Factory" << std::endl;
	}

	std::unique_ptr<Product> CreateProduct(ProductType type)
	{
		std::unique_ptr<Product> product = NULL;
		switch (type)
		{
		case TYPE_A:
			product = std::make_unique<ConcreteProductA>();
			break;

		case TYPE_B:
			product = std::make_unique<ConcreteProductB>();
			break;

		case TYPE_C:
			product = std::make_unique<ConcreteProductC>();
			break;

		default:
			break;
		}
		return product;
	}
};

int main()
{
	std::unique_ptr<Factory> mFactory = std::make_unique<Factory>();
	std::vector<std::unique_ptr<Product>> Products;
	Products.push_back(mFactory->CreateProduct(mFactory->TYPE_A));
	Products.push_back(mFactory->CreateProduct(mFactory->TYPE_B));
	Products.push_back(mFactory->CreateProduct(mFactory->TYPE_C));

	for (unsigned int i = 0; i < Products.size(); i++)
		Products[i]->GetInfo();

	return 0;
}

/* output
Constructor Factory
Constructor Product
Constructor ConcreteProductA
Constructor Product
Constructor ConcreteProductB
Constructor Product
Constructor ConcreteProductC
name: product A
price: 10000
name: product B
price: 20000
name: product C
price: 30000
Destructor ConcreteProductA
Destructor Product
Destructor ConcreteProductB
Destructor Product
Destructor ConcreteProductC
Destructor Product
Destructor Factory
*/