#include <iostream>
#include <windows.h>

template<typename T>
class ThreadSafeData {
    T data_;
    CRITICAL_SECTION cs_;

    T read() {
        EnterCriticalSection(&cs_);
        T temp = data_;
        LeaveCriticalSection(&cs_);
        return temp;
    }

    void write(const T& newVal) {
        EnterCriticalSection(&cs_);
        //br0. wait for testing
        Sleep(3000);
        //*/
        data_ = newVal;
        LeaveCriticalSection(&cs_);
    }

public:
    ThreadSafeData() {
        InitializeCriticalSection(&cs_);
    }

    ~ThreadSafeData() {
        DeleteCriticalSection(&cs_);
    }

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

    const HANDLE reader = CreateThread(nullptr, 0, [](const LPVOID lpParam) -> DWORD {
        const auto tsd = static_cast<ThreadSafeData<INT>*>(lpParam);
        for (;;) {
            std::cout << "Thread 1 read data: " << *tsd << '\n';
            Sleep(1);
        }
        }, &thread_safe_data, 0, nullptr);

    const HANDLE writer = CreateThread(nullptr, 0, [](const LPVOID lpParam) -> DWORD {
        const auto tsd = static_cast<ThreadSafeData<INT>*>(lpParam);
        for (;;) {
            *tsd = *tsd + 1;
            std::cout << "Thread 2 wrote data" << '\n';
            Sleep(1);
        }
        }, &thread_safe_data, 0, nullptr);

    WaitForSingleObject(reader, INFINITE);
    WaitForSingleObject(writer, INFINITE);

    CloseHandle(reader);
    CloseHandle(writer);

    return EXIT_SUCCESS;
}