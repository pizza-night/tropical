#include "ui.hpp"

#include <array>
#include <sys/epoll.h>
#include <unistd.h>

namespace tropical {

void start_ui(std::span<SpscQueue> tx, MpscQueue& rx) {
    extern int input_fd; // will be replaced with a real input fd.

    int epoll_fd = epoll_create1(O_CLOEXEC);
    if (epoll_fd == -1) {
        todo(); // error handling
    }

    // Watch for user input.
    {
        epoll_event event = {.events = EPOLLIN, .data = {.fd = input_fd}};
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, input_fd, &event) == -1) {
            todo(); // error handling
        }
    }

    // Watch for messages from other threads.
    for (SpscQueue& queue : tx) {
        int queue_fd = queue.fd();
        epoll_event event = {.events = EPOLLIN, .data = {.fd = queue_fd}};
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, queue_fd, &event) == -1) {
            todo(); // error handling
        }
    }

    epoll_event events[128];

    for (;;) {
        int num_events = epoll_wait(epoll_fd, events, std::size(events), -1);
        if (num_events == -1) {
            todo(); // error handling
        }
        for (int i = 0; i < num_events; ++i) {
            if (events[i].data.fd == input_fd) {
                todo(); // handle user input.
            } else {
                todo(); // handle message.
            }
        }
    }

    if (close(epoll_fd) == -1) {
        todo(); // error handling
    }
}

} // namespace tropical
