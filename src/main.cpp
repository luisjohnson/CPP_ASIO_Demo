#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <algorithm>

#include <boost/asio.hpp>

std::mutex global_stream_lock;

void WorkerThread(const std::shared_ptr<boost::asio::io_service>& service, int counter)
{
    global_stream_lock.lock();
    std::cout << counter << std::endl;
    global_stream_lock.unlock();

    service->run();

    global_stream_lock.lock();
    std::cout << "End" << std::endl;
    global_stream_lock.unlock();
}

size_t  fac(size_t n)
{
    if(n <= 1)
    {
        return n;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return n * fac(n - 1);
}

void CalculateFactorial(size_t n)
{
    global_stream_lock.lock();
    std::cout << "Calculating " << n << "! factorial" << std::endl;
    global_stream_lock.unlock();

    size_t f = fac(n);

    global_stream_lock.lock();
    std::cout << n << "! = " << f << std::endl;
    global_stream_lock.unlock();
}

int main()
{
    std::shared_ptr<boost::asio::io_service> ioService(new boost::asio::io_service);
    std::shared_ptr<boost::asio::io_service::work> worker(new boost::asio::io_service::work(*ioService));

    global_stream_lock.lock();
    std::cout << "The Program will exit once all the work has finished." << std::endl;
    global_stream_lock.unlock();

    std::vector<std::thread> threads;

    for (int i = 1; i <= 5; i++) {
        threads.emplace_back([ioService, i] {
            return WorkerThread(ioService, i);
        });
    }

    ioService->post([] { return CalculateFactorial(5); });
    ioService->post([] { return CalculateFactorial(6); });
    ioService->post([] { return CalculateFactorial(7); });

    worker.reset();

    std::for_each(threads.begin(), threads.end(), [](std::thread &thread) {
        thread.join();
    });
    return 0;
}