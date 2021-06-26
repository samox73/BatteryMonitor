#pragma once

#include <functional>
#include <thread>
#include <utility>

template<typename... T>
class Timer {
    std::function<void(T...)> function;
    size_t intervalInMillis{};
    std::thread thread;
    bool isRunning{false};
    size_t repetitionLimit{std::numeric_limits<size_t>::max()};
    size_t repetitionCount{0};
    std::tuple<T...> savedArgs;

public:
    Timer(std::function<void(T...)> function_, size_t intervalInMillis_, size_t repetitionCount_)
        : function{std::move(function_)}, intervalInMillis{intervalInMillis_}, repetitionLimit{repetitionCount_} {}

    Timer(std::function<void(T...)> function_, size_t intervalInMillis_)
        : Timer{function_, intervalInMillis_, std::numeric_limits<size_t>::max()} {}

    template<size_t ...sequence>
    void call_function(std::index_sequence<sequence...>) {
        function(std::get<sequence>(savedArgs)...);
    }

    void start(T... values) {
        isRunning = true;
        savedArgs = std::make_tuple(std::move(values)...);
        thread = std::thread([&]() -> void {
            while (isRunning) {
                auto wakeUpTime{std::chrono::steady_clock::now() + std::chrono::milliseconds(intervalInMillis)};
                call_function(typename std::make_index_sequence<sizeof...(T)>{});
                std::this_thread::sleep_until(wakeUpTime);
                if (++repetitionCount >= repetitionLimit) {
                    isRunning = false;
                }
            }
        });
    }

    void join() {
        if (!isRunning) {
            throw std::runtime_error("Cannot join timer: Thread is not running.");
        } else if (!thread.joinable()) {
            throw std::runtime_error("Cannot join timer: Thread is not joinable.");
        } else {
            thread.join();
        }
    }

    void stop() {
        isRunning = false;
    }
};