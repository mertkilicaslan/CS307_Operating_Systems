// Mert Kilicaslan - Programming Assingment 4
#ifndef ALLOCATOR_CPP
#define ALLOCATOR_CPP

#include <iostream>
#include <list>
#include <pthread.h>
#include <unistd.h>
using namespace std;

// In order to ensure atomicity of the operations under concurrency, lock synchronization mechanisms will be used
pthread_mutex_t lockPrint = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockMalloc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockFree = PTHREAD_MUTEX_INITIALIZER;

struct node{
    // Each thread has a unique non-negative ID given to them, if the chunk is free, this field is -1.
    // SIZE and INDEX fields represent the size and the starting address of the chunk, respectively.
    int id, size, index;

    node() {}
    node(int idP, int sizeP, int idxP) : id(idP), size(sizeP), index(idxP) {}

};

class HeapManager{
    list<node> freeList;                // Private doubly linked list to track free chunks but also occupied spaces inside the heap

    public:
        int initHeap(int size);         // Initializes the heap with the requested size, Returns 1
        int myMalloc(int ID, int size); // Tries to allocate heap space of requested size. Returns the index of the new chunk, otherwise returns -1
        int myFree(int ID, int index);  // Frees the allocated chunk. Returns 1 for successful free, otherwise it returns -1
        void print();                   // Prints the memory layout
};


// Prints the list in a specific format
void HeapManager::print(){
    pthread_mutex_lock(&lockPrint); 

    list<node>::iterator temp = freeList.begin();
    for(list<node>::iterator it = freeList.begin(); it != freeList.end(); it++){
        cout << "[" << it->id << "][" << it->size << "][" << it->index << "]";

        if(++temp != freeList.end()) // Node is not tail
            cout << "---";
        else                        // Node is tail
            cout << endl;
    }
    pthread_mutex_unlock(&lockPrint); 
}

// Initializes the list with a single free node with given input size and start index 0, before returning prints the list.
int HeapManager::initHeap(int size){
    node head(-1, size, 0);
    freeList.push_front(head);

    print();
    return 1;
}

// Allocates space from the first free node that has enough space
int HeapManager::myMalloc(int ID, int size){
    pthread_mutex_lock(&lockMalloc); 

    for(list<node>::iterator it = freeList.begin(); it != freeList.end(); it++){
        // Candidate free node is found
        if(it->size >= size && it->id == -1){
            
            // Newly allocated space
            node newNode(ID, size, it->index);
            freeList.insert(it, newNode);

            // Updating remaining free space
            it->size -= size;
            it->index += size;

            cout << "Allocated for thread " << ID << endl;
            print();

            pthread_mutex_unlock(&lockMalloc); 
            return newNode.index;
        }
    }

    // There is no such candidate node
    cout << "Can not allocate, requested size " << size << " for thread " << ID << " is bigger than remaining size" << endl;
    pthread_mutex_unlock(&lockMalloc); 
    return -1;

}

// Node with given index and ID is turned into a free node, it merges with the neighbors if they are free as well
int HeapManager::myFree(int ID, int index){
    pthread_mutex_lock(&lockFree); 

    // Traverses the list for finding a node with the given ID and index
    bool nodeFound = false;
    for(list<node>::iterator it = freeList.begin(); it != freeList.end(); it++)
        if(it->id == ID && it->index == index){
            nodeFound = true;
            it->id = -1;
        }
    
    // Merges new free node with the neighbors if they are free as well (Coalescing)
    for(list<node>::iterator it = freeList.begin(); it != freeList.end(); it++){
        list<node>:: iterator temp = it;
        if(it->id == -1 && it->id == (++temp)->id){
            it->size += temp->size;
            freeList.erase(temp);
            it--;
        }
    }

    cout << "Freed for thread " << ID << endl;
    print();

    pthread_mutex_unlock(&lockFree); 
    return nodeFound? 1 : -1;
}

#endif