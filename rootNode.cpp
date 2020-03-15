#include <boost/interprocess/ipc/message_queue.hpp>
#include <chrono>
#include <iostream>
#include <thread>

#include "sharedData.hpp"

using namespace boost::interprocess;
using BoostQueue = message_queue;

void internalQueue()
{
    std::cout << "Listening to sensors ... \n";
    try
    {
        BoostQueue::remove(shared::constant::QUEUE_NAME);
        BoostQueue boostQueue(
            create_only,
            shared::constant::QUEUE_NAME,
            shared::constant::MAX_QUE_MSG,
            sizeof(shared::SensorData));

        while (true)
        {
            unsigned int priority;
            BoostQueue::size_type recvd_size;
            shared::SensorData data;
            if (boostQueue.try_receive(&data, sizeof(data), recvd_size, priority))
            {
                std::cout << "internalQueue received from sensor:" << data.id <<
                    " t: " << static_cast<int16_t>(data.temp) << " m: " <<
                    unsigned(data.moisture) << "\n";

                // then move data to some global vector
            }
        }
    }
    catch (interprocess_exception & ex)
    {
        BoostQueue::remove(shared::constant::QUEUE_NAME);
        std::cout << ex.what() << std::endl;
    }

    std::cout << "Listening to sensors - QUIT \n";
    return;
}

int main()
{
    std::cout << "Root Node started\n";

    std::thread receiver(&internalQueue);

    // std::deque thread that reads data from global vector every 1 sec
    // and sum it up to one deque record.
    // Then data from deque (with up to <defined> deque size) is printed to user

    receiver.join();
    return 0;
}