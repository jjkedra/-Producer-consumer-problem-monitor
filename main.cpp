#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include "monitor.h"
#include <unistd.h>
#include <string>
#include <queue>

#define BufferSize 10

int pastryID = 0, meatID = 0, cheeseID = 0, cabbageID = 0;   // insertion indexes

pthread_mutex_t pastryMutex;
pthread_mutex_t meatMutex;
pthread_mutex_t cheeseMutex;
pthread_mutex_t cabbageMutex;

class PackageBufferMonitor : Monitor {
public:
    Condition empty, full;
    std::queue<unsigned char> buffer;
    PackageBufferMonitor() = default;

    void insert(int number) {
        enter();
        if (buffer.size() == BufferSize)
            wait(full);

        buffer.push(number);
        if (buffer.size() == 1) {
            signal(empty);
        }
        leave();
    }
    int remove() {
        enter();
        int number;
        if (buffer.empty())
            wait(empty);
        number = buffer.front();
        buffer.pop();
        if (buffer.size() == BufferSize - 1)
            signal(full);
        leave();
        return number;
    }
};

PackageBufferMonitor pastryBuffer;
PackageBufferMonitor meatBuffer;
PackageBufferMonitor cheeseBuffer;
PackageBufferMonitor cabbageBuffer;

void produce(PackageBufferMonitor *buffer, pthread_mutex_t *mutex, int producerID, std::string itemToProduce) {
    while(true) {
        buffer->insert(1);
        printf("\nProducer num %d inserted item %s at index %ld", producerID, itemToProduce.c_str(), buffer->buffer.size());
        sleep(1);
        pthread_mutex_unlock(mutex);
    }
}

void consume(PackageBufferMonitor *buffer, int consumerID, std::string product) {
    while(true) {
        buffer->remove();
        pastryBuffer.remove();
        printf("\nConsumer num %d: made dumplings with %s", consumerID, product.c_str());
        sleep(1);
        }
}


void *producer(void *producerID) {
    switch (*(int*)producerID) {
        case 0:
            produce(&pastryBuffer, &pastryMutex, *(int*)producerID, std::string("🥟"));
            break;
        case 1:
            produce(&meatBuffer, &meatMutex, *(int*)producerID, std::string("🥩"));
            break;
        case 2:
            produce(&cheeseBuffer, &cheeseMutex, *(int*)producerID, std::string("🧀"));
            break;
        case 3:
            produce(&cabbageBuffer, &cabbageMutex, *(int*)producerID, std::string("🥬"));
            break;
    }
    return 0;
}
// 🥟 🥩 🧀 🥬
void *consumer(void *consumerID) {
    switch (*(int*)consumerID) {
        case 0:
            consume(&meatBuffer, *(int*)consumerID, std::string("🥩"));
            break;
        case 1:
            consume(&cheeseBuffer, *(int*)consumerID, std::string("🧀"));
            break;
        case 2:
            consume(&cabbageBuffer, *(int*)consumerID, std::string("🥬"));
            break;
    }
    return 0;
}

int main(int argc, char** argv)
{
    int helper[] = {0, 1, 2, 3};
    // Getting data
    int pastryProd, meatProd, cheeseProd, cabbageProd, meatCons, cheeseCons, cabbageCons;
    if (argc != 8)
    {
		std::cout << "\nUsage: ./name pastryProd meatProd meatCons cheeseProd cheeseCons cabbageProd cabbageCons";
		return 1;
	}
    pastryProd = atoi(argv[1]);
    meatProd = atoi(argv[2]);
    cheeseProd = atoi(argv[3]);
    cabbageProd = atoi(argv[4]);
    meatCons = atoi(argv[5]);
    cheeseCons = atoi(argv[6]);
    cabbageCons = atoi(argv[7]);
    // got it
    int prodArray[4] = {pastryProd, meatProd, cheeseProd, cabbageProd};
    int consArray[3] = {meatCons, cheeseCons, cabbageCons};

    int totalProd = 0;
    for (int i = 0; i < 4; ++i)
        totalProd += prodArray[i];

    int totalCons = 0;
    for (int i = 0; i < 4; ++i)
        totalProd += prodArray[i];

    // thread init
    pthread_t pro[totalProd], con[totalCons];
    pthread_mutex_init(&pastryMutex, NULL);
    pthread_mutex_init(&meatMutex, NULL);
    pthread_mutex_init(&cheeseMutex, NULL);
    pthread_mutex_init(&cabbageMutex, NULL);

    for(int i = 0; i < 4; ++i) {
        int temp = i;
        for (int j = 0; j < prodArray[i]; ++j) {
            pthread_create(&pro[i], NULL, producer, &helper[i]);
        }
    }

    for(int i = 0; i < 3; ++i) {
        for (int j = 0; j < consArray[i]; ++j) {
            pthread_create(&con[i], NULL, consumer, &helper[i]);
        }
    }

    for(int i = 0; i < totalProd; ++i) {
        pthread_join(pro[i], NULL);
    }
    for(int i = 0; i < totalCons; ++i) {
        pthread_join(con[i], NULL);
    }

    return 0;
}
