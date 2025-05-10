#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "config.h"
#include "logger.h"
#include "utils.h"

// Daemon singleton enforcement
static void create_pidfile() 
{
    FILE *fp = fopen(PID_FILE, "w");
    if (!fp) log_error("Failed to create PID file");
    fprintf(fp, "%d", getpid());
    fclose(fp);
}

static void daemonize() 
{
    pid_t pid = fork();
    if (pid < 0) {
        log_error("Fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) exit(EXIT_SUCCESS);  // Parent exits

    // Child becomes daemon
    setsid();
    umask(0);
    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    create_pidfile();
}

// Signal handlers
static void handle_sigterm(int sig) 
{

    (void)sig;  // Mark parameter as unused
    log_info("Received SIGTERM, shutting down");
    remove(PID_FILE);
    exit(EXIT_SUCCESS);

}

static void handle_sigusr1(int sig) 
{

    (void)sig;  // Mark parameter as unused
    log_info("backup/transfer triggered");
    backup_and_transfer();  // Immediate action
    
}

// Signal handler for SIGUSR2
static void handle_sigusr2(int sig) {
    (void)sig;  // Mark parameter as unused
    log_info("Received SIGUSR2, checking for missing files");
    check_missing_files();
}

int main() 
{

    // Singleton check
    if (access(PID_FILE, F_OK) == 0) 
    {
        
        fprintf(stderr, "Daemon already running\n");
        exit(EXIT_FAILURE);
    }

    daemonize();
    signal(SIGTERM, handle_sigterm);
    signal(SIGUSR1, handle_sigusr1);
    signal(SIGUSR2, handle_sigusr2);

    while (1) 
    {

        time_t now = time(NULL);
        struct tm *tm = localtime(&now);

        // Check if it's 11:30 PM
        if (tm->tm_hour == 23 && tm->tm_min == 30) 
        {
            check_missing_files();  // Check for missing files
            lock_upload_directory();  // Lock the upload directory
        }

        // Check if it's 1 AM
        if (tm->tm_hour == 1 && tm->tm_min == 0) 
        {

            backup_and_transfer();

            sleep(1);  // Prevent duplicate runs
        }

        monitor_upload_dir();  // inotify-based file monitoring
        sleep(1);  // Reduce CPU usage
    }
    return 0;
}