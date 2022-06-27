#include <cstdio>
#include <map>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>

std::map<std::string, std::string> g_data;
std::mutex g_mtx;

void save_data_proc(const std::string &key) {
	// given dummy delay
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	std::string result = "dummy data for " + key;

	// lock for writing data, use lock_guard because only scope in whole thread
	std::lock_guard<std::mutex> lck(g_mtx);
	g_data[key] = result;
}

int main()
{
	std::thread save1(save_data_proc, "key1");
	std::thread save2(save_data_proc, "key2");
	save1.join();
	save2.join();

	// safe to access data without lock, as the thread are joined
	for (const auto &pair : g_data) {
		printf("key:%s data:%s\n", pair.first, pair.second);
	}
    
	return 0;
}