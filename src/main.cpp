#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

void WorkerThread(const std::shared_ptr<boost::asio::io_service>& service, int counter)
{
    std::cout << counter << std::endl;
    service->run();
    std::cout << "End" << std::endl;
}

int main()
{
    std::shared_ptr<boost::asio::io_service> ioService(new boost::asio::io_service);
    std::shared_ptr<boost::asio::io_service::work> worker(new boost::asio::io_service::work(*ioService));

    std::cout << "Press ENTER to exit!" << std::endl;

    std::vector<std::thread> threads;

    for(int i=1; i<=5; i++){
        threads.emplace_back([ioService, i] { return WorkerThread(ioService, i); });
    }

    std::cin.get();
    ioService->stop();

    std::for_each(threads.begin(), threads.end(), [](std::thread &thread) {
        thread.join();
    });
    return 0;
}