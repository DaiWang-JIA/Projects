/*

                      //线程库的基本使用
#include <iostream>
#include <thread>
#include <string>





//void pritnHelloWorld(std:: string msg)
//{
//	std::cout << msg << std::endl;
//	return;
//}

void printHelloWorld(std::string msg)
{
	for (int i = 0; i < 10000; i++)
	{
		std::cout << i;
	}
	std::cout << std::endl;
}

int main()
{
	//1.创建子线程
	std::thread  thread1(printHelloWorld,"Hello Thread");
	//thread1.detach();
	bool isJoin = thread1.joinable();
	if (isJoin)
	{
		thread1.join();
	}

	std::cout << "over" << std::endl;

	return 0;
}

*/


/*

                   //几个常见问题和技巧

#include <iostream>
#include <thread>
#include <memory>

//void foo(int& x)
//{
//	x += 1;
//}
//
//int main()
//{
//	int a = 1;
//	std::thread t(foo, std::ref(a));
//	t.join();
//	std::cout << a << std:: endl;
//	return 0;
//}

//class A
//{
//public:
//	void foo() {
//		std::cout << "Hello World" << std::endl;
//
//	}
//};
//
//int main()
//{
//	std::shared_ptr<A> a = std::make_shared<A>();  //智能指针
//	std::thread t(&A::foo, a);
//
//	t.join();
//
//	return 0;
//}


class A {
private:
	//友元
	friend void thread_foo();
	void foo() {
		std::cout << "hello" << std::endl;
	}
};

void thread_foo()
{
	std::shared_ptr<A> a = std::make_shared<A>();

	std::thread t(&A::foo, a);

	t.join();

}


int main()
{
	thread_foo();

	return 0;
}

*/



/*
                  //互斥量解决多线程共享问题
//线程安全：如果多线程程序每一次运行结果和单线程运行结果一样，那么你的线程就是安全的

#include <iostream>
#include <thread>
#include <mutex>

int a = 0;
std::mutex mtx;  //互斥锁

void func() {
	for (int i = 0; i < 10000; i++) {
		mtx.lock();
		a += 1;
		mtx.unlock();
	}
}

int main()
{
	std::thread t1(func);
	std::thread t2(func);

	t1.join();
	t2.join();

	std::cout << a << std::endl;

	return 0;
}

*/



/*
            //互斥量死锁

// 解决：改变顺序
 
#include <iostream>
#include <thread>
#include <mutex>
#include <windows.h>

std::mutex m1, m2;

void func1() {
	for (int i = 0; i < 50; i++) {
		m1.lock();
		m2.lock();
		m1.unlock();
		m2.unlock();
	}


}

void func2() {
	for (int i = 0; i < 50; i++) {
		m1.lock();
		m2.lock();
		m1.unlock();
		m2.unlock();
	}

}


int main()
{
	std::thread t1(func1);
	std::thread t2(func2);

	t1.join();
	t2.join();

	std::cout << "over" << std::endl;

	return 0;
}

*/


/*

             //lock_guard 与c++11 std::unique_lock
#include <iostream>
#include <thread>
#include <mutex>

int shared_data = 0;

std::mutex mtx;

void func() {
	for (int i = 0; i < 10000; i++) {
		std::lock_guard<std::mutex> lg(mtx);
		shared_data++;
	}
}

int main()
{
	std::thread t1(func);
	std::thread t2(func);
	t1.join();
	t2.join();

	std::cout << shared_data << std::endl;
	return 0;
}

*/

/*

        // std::call_once与其使用场景
#include <iostream>
#include <thread>
#include <mutex> 
#include <string>


static Log* log = nullptr;
static std::once_flag once;

//单例模式
class Log {
public:
	Log() {};
	Log(const Log& log) = delete;
	Log& operator=(const Log& log) = delete;

	static Log& GetInstance() {
		if (!log) log = new Log;
		std::call_once(once, init);
		return *log;
	}

	static void init()
	{
		if (!log) log = new Log;
	}

	void PrintLog(std:: string msg)
	{
		std::cout <<__TIME__<<' '<< msg << std::endl;
	}
};

void print_error()
{
	Log::GetInstance().PrintLog("error");
}

int main()
{
	std::thread t1(print_error);
	std::thread t2(print_error);
	t1.join();
	t2.join();

	return 0;
}

*/



/*
               //condition_variable与其使用场景
#include <iostream>
#include <thread>
#include <mutex> 
#include <string>
#include <condition_variable>
#include <queue>


std::queue<int> q_queue;
std::condition_variable g_cv;
std::mutex mtx;

void Producer() {
	for (int i = 0; i < 10; i++) {
		{
			std::unique_lock<std::mutex> lock(mtx);
			q_queue.push(i);
			//通知消费者来取任务
			g_cv.notify_one();
			std::cout << "Producer: " << i << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::microseconds(100));

	}
}

void Consumer() {
	while (1) {
		std::unique_lock<std::mutex> lock(mtx);
		//bool isempty = q_queue.empty();
		//如果队列为空  就要等待
		//g_cv.wait(lock, !isempty);
		g_cv.wait(lock, []() {return !q_queue.empty(); });
		int value = q_queue.front();
		q_queue.pop();

		std::cout << "Consumer: " << value << std::endl;
	}
}

int main()
{
	std::thread t1(Producer);
	std::thread t2(Consumer);
	t1.join();
	t2.join();

	return 0;
}
*/



/*
        //C++ 11 跨平台线程池
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>


class ThreadPool {
public:
	ThreadPool(int numThreads) :stop(false) {
		for (int i = 0; i < numThreads; i++)
		{
			threads.emplace_back([this] {
				while (1) {
					std::unique_lock<std::mutex> lock(mtx);
					condition.wait(lock, [this] {
						return !tasks.empty() || stop;
						});
					if (stop) {
						return;
					}

					std::function<void()> task(std::move(tasks.front()));
					tasks.pop();
					lock.unlock();
					task();
				}
				});
		}
	}

	~ThreadPool() {
		{
			std::unique_lock<std::mutex> lock(mtx);
			stop = true;
		}
		condition.notify_all();
		for (auto& t : threads) {
			t.join();
		}
	}
	template<class F,class...Args>
	void enqueue(F && f,Args&&...args){
		std::function<void()>task = std::bind(std::forward<F>(f),std::forward<Args>( args)...);
		{
			std::unique_lock<std::mutex> lock(mtx);
			tasks.emplace(std::move(task));
		}
		condition.notify_one();
	}

private:
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;

	std::mutex mtx;
	std::condition_variable condition;

	bool stop;
};


int main()
{
	ThreadPool  pool(4);
	for (int i = 0; i < 10; i++)
	{
		pool.enqueue([i]
			{
				std::cout << "task: " << i << "is runing" << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(1));
				std::cout << "task: " << i << "is done" << std::endl;
			});
	}
	std::this_thread::sleep_for(std::chrono::seconds(10));
	return 0;
}
*/



/*
                //异步并发--async future packaged_task  promise
#include <iostream>
#include<future>
using namespace std;


void func(std::promise<int>f)
{
	f.set_value(1000);
}


int main()
{
	//std::future<int> future_result = std::async(std::launch::async, func);
	
	//std::packaged_task<int()> task(func);
	//auto future_result = task.get_future();

	//std::thread t1(std::move(task));
	//cout << func ()<< endl;

	//t1.join();
	//cout << future_result.get() << endl;

	std::promise<int> f;

	auto future_result = f.get_future();
	
	std::thread t1(func, std::ref(f));

	t1.join();
	cout << future_result.get() << endl;

	return 0;
}

*/


/*
                //原子操作   std::atomic
#include <iostream>
#include<thread>
#include<mutex>
#include<atomic>



std::atomic<int> shared_data = 0;

void func() {
	for (int i = 0; i < 100000; i++) {
		shared_data++;
	}
}
int main()
{
	std::thread t1(func);
	std::thread t2(func);
	t1.join();
	t2.join();
	std::cout << "shared_data=" << shared_data << std::endl;
	return 0;
}
*/