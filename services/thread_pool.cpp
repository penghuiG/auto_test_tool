#include <thread_pool.h>

int tread_pool_sample() {
    // 创建一个包含4个线程的线程池
    ThreadPool pool(4);

    // 提交8个任务到线程池
    std::vector<std::future<int>> results;
    for (int i = 0; i < 8; ++i) {
        results.emplace_back(
            pool.enqueue([i] {
                std::cout << "Task " << i  << std::endl;//<< " is running on thread id: " << std::this_thread::get_id() << std::endl;
                          
                std::this_thread::sleep_for(std::chrono::seconds(1)); // 模拟耗时操作
                std::cout << "Task " << i << " finished" << std::endl;
                return i * i; // 返回平方值
            })
        );
    }

    // 获取任务结果
    for (auto &&result : results) {
        std::cout << "Result: " << result.get() << std::endl;
    }

    std::cout << "All tasks completed." << std::endl;
    return 0;
}