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
    std::cout << "Thread " << counter << std::endl;
    global_stream_lock.unlock();

    service->run();

    global_stream_lock.lock();
    std::cout << "Thread " <<  counter << std::endl;
    global_stream_lock.unlock();
}

void PrintNumber(int number)
{
    std::cout << "Number :" << number << std::endl;
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

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    strand.post([]{return PrintNumber(1);});
    strand.post([]{return PrintNumber(2);});
    strand.post([]{return PrintNumber(3);});
    strand.post([]{return PrintNumber(4);});
    strand.post([]{return PrintNumber(5);});


    worker.reset();

    std::for_each(threads.begin(), threads.end(), [](std::thread &thread) {
        thread.join();
    });
    return 0;
}