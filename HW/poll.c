#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_EVENTS 64

void run_etl_loop(int epoll_fd, int source_fd) {
    struct epoll_event event;
    struct epoll_event *events = calloc(MAX_EVENTS, sizeof(struct epoll_event));

    // Register our Extractor FD
    event.events = EPOLLIN | EPOLLET; // Edge-triggered read
    event.data.fd = source_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, source_fd, &event) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    // The ETL Event Loop
    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == source_fd) {
                // EXTRACT & TRANSFORM
                handle_data_extraction(source_fd); 
            }
            // Add additional logic here for Load FDs (EPOLLOUT)
        }
    }
    free(events);
}

