#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

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


int main() {
    std::shared_ptr<boost::asio::io_service> ioService(new boost::asio::io_service);
    std::shared_ptr<boost::asio::io_service::work> worker(new boost::asio::io_service::work(*ioService));
    std::shared_ptr<boost::asio::io_service::strand> strand( new boost::asio::io_service::strand(*ioService));

    global_stream_lock.lock();
    std::cout << "Press ENTER to exit" << std::endl;
    global_stream_lock.unlock();

    std::vector<std::thread> threads;

    for (int i = 1; i <= 2; i++) {
        threads.emplace_back([ioService, i] {
            return WorkerThread(ioService, i);
        });
    }

    boost::asio::ip::tcp::socket socket(*ioService);

    try {
        boost::asio::ip::tcp::resolver resolver(*ioService);
        boost::asio::ip::tcp::resolver::query query("www.packtpub.com",
                                                    boost::lexical_cast<std::string>(80));
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
        boost::asio::ip::tcp::endpoint endpoint = *iterator;
        global_stream_lock.lock();
        std::cout << "Connecting to: " << endpoint << std::endl;
        global_stream_lock.unlock();
        std::cout << "Connected!" << std::endl;

    }
    catch(std::exception &exception) {
        global_stream_lock.lock();
        std::cout << exception.what() << std:: endl;
        global_stream_lock.unlock();
    }

    std::cin.get();
    boost::system::error_code ec;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket.close();

    ioService->stop();

    std::for_each(threads.begin(), threads.end(), [](std::thread &thread) {
        thread.join();

    });
    return 0;
}