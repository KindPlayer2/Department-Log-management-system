#ifndef DAEMON_H
#define DAEMON_H

#include <unistd.h>
#include <sys/types.h>

// Function declarations
void daemonize();
void handle_sigterm(int sig);
void handle_sigusr1(int sig);

#endif