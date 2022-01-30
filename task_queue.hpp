#ifndef SCHEDULER_TASK_QUEUE_HPP
#define SCHEDULER_TASK_QUEUE_HPP

#include "task.hpp"

#include <array>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

#include <iostream>

namespace sched {

    class task_queue_t {
    public:
        void push_task(std::shared_ptr<task>&& tsk_ptr) {
            priority_t priority{tsk_ptr->get_priority()};

            std::scoped_lock<std::mutex> const lock{mutex};
            std::size_t const idx{static_cast<unsigned>(priority) - static_cast<unsigned>(std::numeric_limits<decltype(priority)>::min())};
            queues[idx].emplace(std::move(tsk_ptr));
            total_tasks++;
            cv.notify_one();
            std::cout << '[' << std::this_thread::get_id() << "] Adding task with priority " << static_cast<unsigned>(priority) << std::endl;
        }

        //FIXME: stop_token should be in the task_queue impl and not a referenced one. But I don't have time and wish to change it.
        std::shared_ptr<task> pop_task_with_wait() {
            std::unique_lock<std::mutex> lock{mutex}; //FIXME: it can be ctored as unlocked!

            std::cout << '[' << std::this_thread::get_id() << "] Waiting for new tasks" << std::endl;
            //TODO:: FIXME: not the best way to finish job in case of dtoring request
            cv.wait(lock, [queue = this]{return queue->total_tasks != 0 || queue->wait_cancelled;});
            if (wait_cancelled.load())
                return nullptr;
            std::cout << '[' << std::this_thread::get_id() << "] Waking up to take new task!" << std::endl;


            for (auto& q: queues) {
                if (std::size(q)) {
                    auto tsk_ptr{std::move(q.front())};
                    q.pop();
                    total_tasks--;
                    return tsk_ptr;
                }
            }
            throw std::runtime_error{"We should never be here"};
        };

        task_queue_t() = default;

        ~task_queue_t() {
            wait_cancelled.store(true);
            cv.notify_all();
        }

        task_queue_t(task_queue_t&) = delete;

    private:
        static constexpr std::size_t queues_cnt{static_cast<unsigned>(std::numeric_limits<priority_t>::max()) -
                                                static_cast<unsigned>(std::numeric_limits<priority_t>::min()) + 1};
        using queues_t = std::array<std::queue<std::shared_ptr<task>>, queues_cnt>;
        queues_t queues{};
        std::mutex mutex;
        std::condition_variable cv;
        std::atomic_bool wait_cancelled{false};
        std::size_t total_tasks{0};
    };

}
#endif //SCHEDULER_TASK_QUEUE_HPP
