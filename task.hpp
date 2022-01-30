#ifndef SCHEDULER_TASK_HPP
#define SCHEDULER_TASK_HPP

#include "priority.hpp"

#include <atomic>
#include <chrono>

namespace sched {
    using namespace std::chrono_literals;

    class task {
    public:
        explicit task(priority_t const& priority)
                : priority{priority}, execute_once{true}, interval{0s} {
        }

        task(priority_t const& priority, std::chrono::seconds const& repeat_interval)
                : priority{priority}, execute_once{false}, interval{repeat_interval} {
        }

        virtual void execute() = 0;

        void cancel() {
            force_stop();
            canceled.store(true);
        }

        [[nodiscard]]
        bool is_canceled() const {
            return canceled.load();
        }

        [[nodiscard]]
        priority_t get_priority() const {
            return priority;
        };

        [[nodiscard]]
        std::chrono::seconds get_interval() const {
            return interval;
        }

        [[nodiscard]]
        bool is_execute_once() const {
            return execute_once;
        }

        virtual ~task() = default;
    private:
        virtual void force_stop() = 0;

        priority_t priority;
        bool const execute_once;
        std::atomic_bool canceled{};
        std::chrono::seconds const interval;
    };

}

#endif //SCHEDULER_TASK_HPP
