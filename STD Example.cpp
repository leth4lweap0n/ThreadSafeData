#include <iostream>
#include <mutex>
#include <thread>
#include <windows.h>

template<typename T>
class ThreadSafeData {
    T data_;
    _STD mutex mtx_;

    T read() {
        _STD lock_guard<_STD mutex> lock(mtx_);
        return data_;
    }

    void write(const T& newVal) {
        {
            _STD lock_guard<_STD mutex> lock(mtx_);
            //br0. wait for testing
            _STD this_thread::sleep_for(_STD chrono::seconds(3));
            //*/
            data_ = newVal;
        }
    }

public:
    ThreadSafeData() = default;
    ThreadSafeData& operator=(const T& newData) {
        write(newData);
        return *this;
    }
    operator T() {
        return read();
    }

};

INT main() {
    ThreadSafeData<INT> thread_safe_data;
    thread_safe_data = 0;
    _STD thread reader([&thread_safe_data]() {
        for (;;) {
           
            _STD cout << "Thread 1 read data: " << thread_safe_data << '\n';
        }
        });

    _STD thread writer([&thread_safe_data]() {
        for (;;) {
            thread_safe_data = thread_safe_data + 1;
            _STD cout << "Thread 2 wrote data" << '\n';
        }
        });

    reader.detach();
    writer.detach();

    for (;;)
        _STD this_thread::sleep_for(_STD chrono::milliseconds(1));

    return EXIT_SUCCESS;
}
