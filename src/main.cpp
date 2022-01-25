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
    std::cout << "Thread " << counter << " Start." << std::endl;
    global_stream_lock.unlock();

    while (true)
    {
        try
        {
            boost::system::error_code ec;
            service->run(ec);
            if(ec)
            {
                global_stream_lock.lock();
                std::cout << "Message: " << ec << std::endl;
                global_stream_lock.unlock();
            }
            break;
        }
        catch(std::exception &exception)
        {
            global_stream_lock.lock();
            std::cout << exception.what() << std::endl;
            global_stream_lock.unlock();
        }
    }

        global_stream_lock.lock();
        std::cout << "Thread " <<  counter << std::endl;
        global_stream_lock.unlock();
}

void ThrowAnException(const std::shared_ptr<boost::asio::io_service>& service)
{
    global_stream_lock.lock();
    global_stream_lock.unlock();
    service->post([service]{return ThrowAnException(service);});
    throw(std::runtime_error("The exception!!!"));
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

    for (int i = 1; i <= 5; i++) {
        threads.emplace_back([ioService, i] {
            return WorkerThread(ioService, i);
        });
    }

    ioService->post([ioService] { return ThrowAnException(ioService); });

    std::for_each(threads.begin(), threads.end(), [](std::thread &thread) {
        thread.join();
    });
    return 0;
}