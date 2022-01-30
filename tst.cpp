#include "scheduler.hpp"

#include <gtest/gtest.h>

using namespace std::chrono_literals;

class print_messages : public sched::task {
public:
    print_messages(sched::priority_t const priority,
                   std::chrono::seconds interval, std::string const& msg)
                   : task{priority, interval}, message{msg} {};

    void execute() override {
        std::cout << message << std::endl;
    };

    ~print_messages() override {
        std::cout << "I'm dead!!!" << std::endl;
    }
private:
    void force_stop() override {
        std::cout << "They trying to kill me!!!" << std::endl;
    }

    std::string message;
};


TEST(sched_test, always_true) {
    EXPECT_EQ(true, 1);
    EXPECT_EQ(2 + 2, 4);
}

TEST(sched_test, ctor_dtor) {
    sched::scheduler scheduler;
}

TEST(sched_test, print_alive) {
    auto task{std::shared_ptr<sched::task>{new print_messages{sched::priority_t{10}, 1s, "I'm alive!!!"}}};
    sched::scheduler scheduler;
    scheduler.add_task(task);

    std::this_thread::sleep_for(10s);
}

TEST(sched_test, cancel_task) {
    auto task{std::shared_ptr<sched::task>{new print_messages{sched::priority_t{10}, 1s, "I'm alive!!!"}}};
    sched::scheduler scheduler;
    scheduler.add_task(task);
    std::this_thread::sleep_for(2s);
    task->cancel();
    std::this_thread::sleep_for(10s);
}

int main(int argc, char* argv[]) try {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} catch (std::exception const& e) {
    std::cerr << "Exception in main: " << e.what() << std::endl;
    return EXIT_FAILURE;
} catch (...) {
    std::cerr << "Unknown exception in main: " << std::endl;
    return EXIT_FAILURE;
}

