#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include "monitor.h"
#include <unistd.h>
#include <string>
#include <queue>

#define N 10

int doughID = 0, meatID = 0, cheeseID = 0, cabbageID = 0;   // insertion indexes

pthread_mutex_t doughMutex;
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
        if (buffer.size() == N)
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
        if (buffer.size() == N - 1)
            signal(full);
        leave();
        return number;
    }
};

PackageBufferMonitor doughBuffer;
PackageBufferMonitor meatBuffer;
PackageBufferMonitor cheeseBuffer;
PackageBufferMonitor cabbageBuffer;

void produceIngredient(PackageBufferMonitor *buffer, pthread_mutex_t *mutex, int producerID, std::string itemToProduce) {
    while(true) {
        buffer->insert(1);
        printf("Producer num %d inserted item %s\n", producerID, itemToProduce.c_str());
        sleep(2);
        pthread_mutex_unlock(mutex);
    }
}

void consumeIngredients(PackageBufferMonitor *buffer, int consumerID, std::string product) {
    while(true) {
        buffer->remove();
        doughBuffer.remove();
        printf("Consumer num %d: Made dumplings with %s\n", consumerID, product.c_str());
        sleep(2);
        }
}


void *producer(void *producerID) {
    switch (*(int*)producerID) {
        case 0:
            produceIngredient(&doughBuffer, &doughMutex, *(int*)producerID, std::string("Dough"));
            break;
        case 1:
            produceIngredient(&meatBuffer, &meatMutex, *(int*)producerID, std::string("Meat"));
            break;
        case 2:
            produceIngredient(&cheeseBuffer, &cheeseMutex, *(int*)producerID, std::string("Cheese"));
            break;
        case 3:
            produceIngredient(&cabbageBuffer, &cabbageMutex, *(int*)producerID, std::string("Cabbage"));
            break;
    }
    return 0;
}

void *consumer(void *consumerID) {
    switch (*(int*)consumerID) {
        case 0:
            consumeIngredients(&meatBuffer, *(int*)consumerID, std::string("Meat"));
            break;
        case 1:
            consumeIngredients(&cheeseBuffer, *(int*)consumerID, std::string("Cheese"));
            break;
        case 2:
            consumeIngredients(&cabbageBuffer, *(int*)consumerID, std::string("Cabbage"));
            break;
    }
    return 0;
}

int main(int argc, char** argv)
{
    // Getting data
    int doughProd, meatProd, cheeseProd, cabbageProd, meatCons, cheeseCons, cabbageCons;
    if (argc != 8)
    {
		std::cout << "\nWrong parameters, correct syntax: ./<filename>.out <doughProd> <meatProd> <cheeseProd> <cabbageProd> <meatCons> <cheeseCons> <cabbageCons>";
		return 1;
	}
    doughProd = atoi(argv[1]);
    meatProd = atoi(argv[2]);
    cheeseProd = atoi(argv[3]);
    cabbageProd = atoi(argv[4]);
    meatCons = atoi(argv[5]);
    cheeseCons = atoi(argv[6]);
    cabbageCons = atoi(argv[7]);
    // got it
    int prodArray[4] = {doughProd, meatProd, cheeseProd, cabbageProd};
    int consArray[3] = {meatCons, cheeseCons, cabbageCons};

    int totalProd = 0;
    for (int i = 0; i < 4; ++i)
        totalProd += prodArray[i];

    int totalCons = 0;
    for (int i = 0; i < 4; ++i)
        totalProd += prodArray[i];

    // thread init
    pthread_t pro[totalProd], con[totalCons];
    pthread_mutex_init(&doughMutex, NULL);
    pthread_mutex_init(&meatMutex, NULL);
    pthread_mutex_init(&cheeseMutex, NULL);
    pthread_mutex_init(&cabbageMutex, NULL);

    for(int i = 0; i < 4; ++i) {
        for (int j = 0; j < prodArray[i]; ++j) {
            pthread_create(&pro[i], NULL, producer, &j);
        }
    }

    for(int i = 0; i < 3; ++i) {
        for (int j = 0; j < consArray[i]; ++j) {
            pthread_create(&con[i], NULL, consumer, &j);
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
