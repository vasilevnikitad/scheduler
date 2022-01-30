#ifndef SCHEDULER_PRIORITY_HPP
#define SCHEDULER_PRIORITY_HPP

#include <stdexcept>
#include <limits>

namespace sched {
    class priority_t {
    public:
        explicit constexpr priority_t(unsigned new_value) {
            if (new_value > max_priority)
                throw std::overflow_error{"value is too big. Max priority is " + std::to_string(max_priority)};
            else if (new_value < min_priority)
                throw std::overflow_error{"value is too small. Min priority is " + std::to_string(min_priority)};

            value = new_value;
        };

        priority_t(priority_t const& prio) = default;
        priority_t& operator = (priority_t const& prio) = default;

        explicit constexpr operator unsigned () const {
            return value;
        }

        friend bool operator == (priority_t const& l, priority_t const& r) { return l.value == r.value; }

        friend bool operator != (priority_t const& l, priority_t const& r) { return !(l == r); }

        friend bool operator < (priority_t const& l, priority_t const& r) { return l.value < r.value; }

        friend bool operator > (priority_t const& l, priority_t const& r) { return r < l; }

        friend bool operator <= (priority_t const& l, priority_t const& r) { return !(l > r); }

        friend bool operator >= (priority_t const& l, priority_t const& r) { return !(l < r); }

        static constexpr std::uint8_t max_priority{10};
        static constexpr std::uint8_t min_priority{1};
    private:
        std::uint8_t value{min_priority};
    };


}

namespace std {
    template<>
    struct numeric_limits<sched::priority_t> : public numeric_limits<uint8_t> {
        /*TODO: FIXME: TBD */
        static constexpr sched::priority_t min() noexcept { return sched::priority_t{sched::priority_t::min_priority}; }
        static constexpr sched::priority_t max() noexcept { return sched::priority_t{sched::priority_t::max_priority}; }
    };
}

#endif //SCHEDULER_PRIORITY_HPP
