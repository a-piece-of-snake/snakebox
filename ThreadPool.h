// by Gemini
// �Ż����̳߳�

#pragma once
#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>
#include <functional>
#include <random>
#include <atomic>
#include <memory>
#include <algorithm>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads) : stop(false), nextQueueIndex(0), rng(std::random_device{}()) {
        if (numThreads == 0) numThreads = 1;

        queues.resize(numThreads);
        mutexes.reserve(numThreads);
        cvs.reserve(numThreads);
        for (size_t i = 0; i < numThreads; ++i) {
            mutexes.emplace_back(std::make_unique<std::mutex>());
            cvs.emplace_back(std::make_unique<std::condition_variable>());
        }

        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this, i] { this->workerLoop(i); });
        }
    }

    ~ThreadPool() {
        stop.store(true, std::memory_order_release);
        for (auto& cv : cvs) {
            cv->notify_all();
        }
        for (auto& t : workers) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using return_type = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();

        // �Ľ���ʹ��ԭ�Ӳ�����ʵ�������ѭ���ַ�������������
        size_t index = nextQueueIndex.fetch_add(1, std::memory_order_relaxed) % queues.size();

        {
            std::unique_lock<std::mutex> lock(*mutexes[index]);
            queues[index].emplace_back([task]() { (*task)(); });
        }
        cvs[index]->notify_one();
        return res;
    }

private:
    std::vector<std::deque<std::function<void()>>> queues;
    std::vector<std::unique_ptr<std::mutex>> mutexes;
    std::vector<std::unique_ptr<std::condition_variable>> cvs;
    std::vector<std::thread> workers;
    std::atomic<bool> stop;
    std::atomic<size_t> nextQueueIndex; // ��������ַ���ԭ�Ӽ�����

    std::mt19937 rng; // �����������
    std::mutex rngMutex; // ����������������Ļ�����

    void workerLoop(size_t index) {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(*mutexes[index]);
                cvs[index]->wait(lock, [this, index] {
                    return stop.load(std::memory_order_acquire) || !queues[index].empty();
                    });

                if (stop.load(std::memory_order_acquire) && queues[index].empty()) {
                    return;
                }

                if (!queues[index].empty()) {
                    task = std::move(queues[index].front());
                    queues[index].pop_front();
                }
            }

            if (task) {
                task();
            }
            else {
                // û�б������񣬳��Դ���������β����ȡ
                if (trySteal(index, task)) {
                    task();
                }
                else {
                    // û���������ȡ����΢�ȴ�һ�£������ת
                    std::this_thread::yield();
                }
            }
        }
    }

    bool trySteal(size_t myIndex, std::function<void()>& task) {
        size_t victim;
        {
            std::lock_guard<std::mutex> lock(rngMutex);
            std::uniform_int_distribution<size_t> dist(0, queues.size() - 1);
            victim = dist(rng);
        }

        if (victim == myIndex) {
            return false;
        }

        if (mutexes[victim]->try_lock()) {
            if (!queues[victim].empty()) {
                task = std::move(queues[victim].back());
                queues[victim].pop_back();
                mutexes[victim]->unlock();
                return true;
            }
            mutexes[victim]->unlock();
        }
        return false;
    }
};