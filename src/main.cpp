#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

std::mutex global_stream_lock;

void WorkerThread(const std::shared_ptr<boost::asio::io_service>& service, int counter)
{
    global_stream_lock.lock();
    std::cout << "Thread " << counter << " Start." << std::endl;
    global_stream_lock.unlock();

    try {
        service->run();
        global_stream_lock.lock();
        std::cout << "Thread " <<  counter << std::endl;
        global_stream_lock.unlock();
    }
    catch(std::exception &exception)
    {
        global_stream_lock.lock();
        std::cout << "Message: " <<  exception.what() << std::endl;
        global_stream_lock.unlock();
    }
}

void ThrowAnException(const std::shared_ptr<boost::asio::io_service>& service, int counter)
{
    global_stream_lock.lock();
    std::cout << "Throw Exception : " << counter << std::endl;
    global_stream_lock.unlock();
    throw(std::runtime_error("The exception!!"));
}

int main()
{
    std::shared_ptr<boost::asio::io_service> ioService(new boost::asio::io_service);
    std::shared_ptr<boost::asio::io_service::work> worker(new boost::asio::io_service::work(*ioService));
    boost::asio::io_service::strand strand(*ioService);

    global_stream_lock.lock();
    std::cout << "The Program will exit once all the work has finished." << std::endl;
    global_stream_lock.unlock();

    std::vector<std::thread> threads;

    for (int i = 1; i <= 2; i++) {
        threads.emplace_back([ioService, i] {
            return WorkerThread(ioService, i);
        });
    }

    //  We can also see that although we post work for the io_service object five times,
    //  the exception handling only handle two exceptions because once the thread has finished,
    //  the join_all() function in the thread will finish the thread and exit the program.
    //  In other words, we can say that once the exception is handled, the thread exits to join the call.
    `//  Additional code that might have thrown an exception will never be called.

    ioService->post([ioService] { return ThrowAnException(ioService, 1); });
    ioService->post([ioService] { return ThrowAnException(ioService, 2); });
    ioService->post([ioService] { return ThrowAnException(ioService, 3); });
    ioService->post([ioService] { return ThrowAnException(ioService, 4); });
    ioService->post([ioService] { return ThrowAnException(ioService, 5); });

    std::for_each(threads.begin(), threads.end(), [](std::thread &thread) {
        thread.join();
    });
    return 0;
}