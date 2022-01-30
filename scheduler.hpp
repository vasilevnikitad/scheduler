#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "task_queue.hpp"

#include <stdexcept>
#include <thread>
#include <vector>
#include <memory>

namespace sched {
    using namespace std::chrono_literals;
/*
    1. Задача (Task) – класс выполняющий какую-либо полезную работу
    1.1 Имеет метод Execute выполняющий работу
    1.2 Имеет метод Cancel позволяющий прервать выполнение работы
    1.3 Имеет приоритет – целое число от 1 до 10. 1 – максимальный приоритет, 10 –
    минимальный приоритет
    1.4 Может быть однократной или периодической с заданным интервалом
    2. Планировщик (Scheduler) – управляет очередью задач и выполняет задачи в пуле
    потоков согласно их приоритету, если несколько задач имеют одинаковый приоритет –
    первой выполняется та, что была добавлена раньше, если задачи с одинаковым
            приоритетом добавлены одновременно – порядок их выполнения произвольный.
    3. Очередь задач – может быть как частью планировщика, так и отдельной сущностью
    4. Пул потоков – количество потоков может задаваться при создании планировщика, но
            так же должен быть вариант «по-умолчанию»
    5. Использовать язык С++ и платформу Linux/Windows
    6. Использовать по выбору Makefile/Cmake/Visual Studio
*/

    class scheduler {
    public:
        explicit scheduler(std::size_t const workers_count = std::thread::hardware_concurrency()) {
            for (std::size_t i{}; i < workers_count; i++) {
                pool.emplace_back(std::jthread{worker_job, std::ref(task_queue)});
            }
        };

        void add_task(std::shared_ptr<task> task) {
            task_queue.push_task(std::move(task));
        }

        ~scheduler() {
        }

    private:
        static void worker_job(std::stop_token stoken, task_queue_t& task_queue) {
            while (!stoken.stop_requested()) {
                auto task_ptr{task_queue.pop_task_with_wait()};

                if (!task_ptr) {
                    return;
                }

                if (task_ptr->is_execute_once()) {
                    task_ptr->execute();
                } else {
                    do {
                        task_ptr->execute();
                        if (task_ptr->is_canceled())
                            break;
                        //TODO:FIXME: We should wake up from sleep in case of cancel request/dtor
                        std::this_thread::sleep_for(task_ptr->get_interval());
                    } while(!stoken.stop_requested());
                }
            }
        }

        std::vector<std::jthread> pool;
        task_queue_t task_queue;
    };

}

#endif //SCHEDULER_HPP
