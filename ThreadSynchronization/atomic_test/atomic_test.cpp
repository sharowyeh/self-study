#include <cstdio>
#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>

// test for https://stackoverflow.com/questions/26787086/atomic-pointers-in-c-and-passing-objects-between-threads

int secret;
int answer;
std::atomic<int*> ptr;

void f1() {
    secret = 50;
    answer = 60;
    ptr = &secret;
    secret = 888;
    // the pointer itself is atomic, but may not apply to the value pointered to
    ptr.store(&answer);

    std::this_thread::yield();
}

void f2() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    int i = *ptr;
    std::cout << "ptr:" << ptr << " value:" << i << std::endl;
}

int main()
{
    std::thread t1 = std::thread(f1);
    t1.join();
    std::thread t2 = std::thread(f2);
    t2.join();
    system("pause");
    return 0;
}
