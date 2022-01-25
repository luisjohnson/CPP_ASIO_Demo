#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

std::mutex global_stream_lock;

void WorkerThread(const std::shared_ptr<boost::asio::io_service> &service, int counter) {
    global_stream_lock.lock();
    std::cout << "Thread " << counter << " Start." << std::endl;
    global_stream_lock.unlock();

    while (true) {
        try {
            boost::system::error_code ec;
            service->run(ec);
            if (ec) {
                global_stream_lock.lock();
                std::cout << "Message: " << ec << std::endl;
                global_stream_lock.unlock();
            }
            break;
        }
        catch (std::exception &exception) {
            global_stream_lock.lock();
            std::cout << exception.what() << std::endl;
            global_stream_lock.unlock();
        }
    }

    global_stream_lock.lock();
    std::cout << "Thread " << counter << std::endl;
    global_stream_lock.unlock();
}

void TimerHandler(const boost::system::error_code &ec,
                  std::shared_ptr<boost::asio::deadline_timer> timer) {
    if (ec) {
        global_stream_lock.lock();
        std::cout << "Error Message: " << ec << std::endl;
        global_stream_lock.unlock();
    } else {
        global_stream_lock.lock();
        std::cout << "You see this because you waited 10 seconds." << std::endl;
        std::cout << "Now press ENTER to exit" << std::endl;
        global_stream_lock.unlock();
        timer->expires_from_now(boost::posix_time::seconds(3));
        // The _1 parameter is a placeholder argument that will be sustitued
        // by the first input argument.
        timer->async_wait(boost::bind(&TimerHandler, _1, timer));
    }
}

int main() {
    std::shared_ptr<boost::asio::io_service> ioService(new boost::asio::io_service);
    std::shared_ptr<boost::asio::io_service::work> worker(new boost::asio::io_service::work(*ioService));
    boost::asio::io_service::strand strand(*ioService);

    global_stream_lock.lock();
    std::cout << "Wait 10 seconds to see what happen, otherwise press ENTER to exit" << std::endl;
    global_stream_lock.unlock();

    std::vector<std::thread> threads;

    for (int i = 1; i <= 5; i++) {
        threads.emplace_back([ioService, i] {
            return WorkerThread(ioService, i);
        });
    }

    std::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(*ioService));
    timer->expires_from_now(boost::posix_time::seconds(3));
    timer->async_wait(boost::bind(&TimerHandler, _1, timer));

    std::cin.get();

    ioService->stop();

    std::for_each(threads.begin(), threads.end(), [](std::thread &thread) {
        thread.join();
    });
    return 0;
}