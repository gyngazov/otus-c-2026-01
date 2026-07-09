#include <pthread.h>

typedef struct Node {
    void *data;
    struct Node *next;
} Node;
/**
 * Потокобезопасный ФИФО
 */
typedef struct {
    Node *head;
    Node *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int shutdown;
    int size;
} ThreadSafeQueue;

void queue_init(ThreadSafeQueue *q);
void queue_destroy(ThreadSafeQueue *q);
int queue_push(ThreadSafeQueue *q, void *data);
void* queue_pop(ThreadSafeQueue *q);
void queue_shutdown(ThreadSafeQueue *q);

