#include <iostream>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

void Print1()
{
    for(int i=0; i<10; i++)
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
        std::cout << "[Print 1]: " << i << std::endl;
    }

}

void Print2()
{
    for(int i=0; i<10; i++)
    {
        boost::this_thread::sleep_for(boost::chrono::milliseconds (500));
        std::cout << "[Print 2]: " << i << std::endl;
    }
}

int main() {

    boost::thread_group threads;
    threads.create_thread(Print1);
    threads.create_thread(Print2);

    threads.join_all();

    return 0;
}
