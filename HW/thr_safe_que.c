#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Structure for queue nodes
typedef struct Node {
    void *data;
    struct Node *next;
} Node;

// Structure for the thread-safe queue
typedef struct {
    Node *head;
    Node *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int shutdown;
} ThreadSafeQueue;

void queue_init(ThreadSafeQueue *q) {
    q->head = NULL;
    q->tail = NULL;
    q->shutdown = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

void queue_destroy(ThreadSafeQueue *q) {
    pthread_mutex_lock(&q->mutex);
    Node *current = q->head;
    while (current != NULL) {
        Node *next = current->next;
        free(current);
        current = next;
    }
    pthread_mutex_unlock(&q->mutex);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}

void queue_push(ThreadSafeQueue *q, void *data) {
    Node *new_node = malloc(sizeof(Node));
    if (!new_node) return;
    
    new_node->data = data;
    new_node->next = NULL;

    pthread_mutex_lock(&q->mutex);
    
    if (q->tail == NULL) {
        q->head = new_node;
        q->tail = new_node;
    } else {
        q->tail->next = new_node;
        q->tail = new_node;
    }

    // Wake up one worker thread waiting for data
    pthread_cond_signal(&q->cond); 
    pthread_mutex_unlock(&q->mutex);
}
void* queue_pop(ThreadSafeQueue *q) {
    pthread_mutex_lock(&q->mutex);

    // Use a while loop to handle spurious wakeups
    while (q->head == NULL && !q->shutdown) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }

    if (q->shutdown && q->head == NULL) {
        pthread_mutex_unlock(&q->mutex);
        return NULL; // Queue is closing down
    }

    Node *temp = q->head;
    void *data = temp->data;
    q->head = q->head->next;

    if (q->head == NULL) {
        q->tail = NULL;
    }

    free(temp);
    pthread_mutex_unlock(&q->mutex);
    return data;
}
void queue_shutdown(ThreadSafeQueue *q) {
    pthread_mutex_lock(&q->mutex);
    q->shutdown = 1;
    pthread_cond_broadcast(&q->cond); // Wake up all waiting threads
    pthread_mutex_unlock(&q->mutex);
}
