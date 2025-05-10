#include <sys/inotify.h>
#include <sys/stat.h>
#include <sys/wait.h>  // For wait()
#include <unistd.h>
#include <time.h>
#include <ftw.h>
#include <libgen.h>
#include <stdlib.h>
#include <linux/limits.h>    // For PATH_MAX, NAME_MAX
#include <stdio.h>     // For snprintf
#include "config.h"
#include "logger.h"
#include "daemon.h"
#include "utils.h"
#include <errno.h>
#include <string.h>

void monitor_upload_dir() 
{

    int fd = inotify_init();
    if (fd < 0) 
    {

        log_error("inotify_init failed");
        return;

    }

    int wd = inotify_add_watch(fd, UPLOAD_DIR, IN_CREATE | IN_MODIFY);
    if (wd < 0) 
    {

        log_error("Failed to watch upload directory");
        close(fd);
        return;

    }

    char buffer[sizeof(struct inotify_event) + NAME_MAX + 1];

    ssize_t len = read(fd, buffer, sizeof(buffer));
    
    if (len < 0) 
    {

        log_error("Failed to read inotify event");
        close(fd);
        return;

    }

    struct inotify_event *event = (struct inotify_event *)buffer;
    if (event->mask & IN_CREATE || event->mask & IN_MODIFY) {
        log_change(event->name);  // Log username, timestamp, filename
    }

    close(fd);
}

void lock_directories() 
{

    if (chmod(UPLOAD_DIR, LOCK_PERM) < 0) 
    {

        log_error("Failed to lock upload directory");

    }

    if (chmod(REPORTING_DIR, LOCK_PERM) < 0) 
    {

        log_error("Failed to lock reporting directory");

    }

    log_info("Directories locked");

}

void unlock_directories() 
{

    if (chmod(UPLOAD_DIR, UNLOCK_PERM) < 0) 
    {

        log_error("Failed to unlock upload directory");

    }

    if (chmod(REPORTING_DIR, UNLOCK_PERM) < 0) 
    {

        log_error("Failed to unlock reporting directory");

    }

    log_info("Directories unlocked");
}

void backup_and_transfer() 
{

    lock_directories();

    //Log to mv command
    log_info("Moving files from %s to %s", UPLOAD_DIR, REPORTING_DIR);

    // Move files from upload to reporting
    pid_t pid = fork();
    if (pid == 0) 
    {
        // Use a shell to handle wildcard expansion
        execl("/bin/sh", "sh", "-c", "mv " UPLOAD_DIR "/*.xml " REPORTING_DIR, NULL);
        log_error("exec mv failed");
        _exit(EXIT_FAILURE);
    }
    wait(NULL);

    log_info("File transfer completed");

    // Create timestamped backup
    time_t now = time(NULL);
    char backup_path[PATH_MAX];
    snprintf(backup_path, sizeof(backup_path), "%s/%ld.tar.gz", BACKUP_DIR, now);

    pid = fork();
    if (pid == 0) 
    {
        //Log tar being executed
        log_info("Executing: tar czf %s %s", backup_path, REPORTING_DIR);

        //execute tar command
        execl("/bin/tar", "tar", "czf", backup_path, REPORTING_DIR, NULL);

        //if fails log error
        log_error("exec tar failed");
        _exit(EXIT_FAILURE);

    }
    wait(NULL);

    unlock_directories();
    log_info("Backup and transfer completed");

}

int file_exists(const char *path) 
{

    return access(path, F_OK) == 0;

}

void ensure_directory_exists(const char *path) 
{

    if (!file_exists(path)) 
    {

        mkdir(path, 0775);
        log_info("Created directory: %s", path);

    }

}

void check_missing_files() 
{

    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char expected_filename[50];

    // Define the required departments
    const char *departments[] = {"Warehouse", "Manufacturing", "Sales", "Distribution"};

    int num_departments = sizeof(departments) / sizeof(departments[0]);

    // Generate the expected filenames
    for (int i = 0; i < num_departments; i++) 
    {

        snprintf(expected_filename, sizeof(expected_filename), "%s_%04d%02d%02d.xml",
                 departments[i], tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);

        // Check if the file exists
        if (access(expected_filename, F_OK) != 0) 
        {
            
            log_info("Missing file: %s", expected_filename);
        }
    }
}

void lock_upload_directory() 
{
    if (chmod(UPLOAD_DIR, 0555) < 0)  // Read+execute, no write
    {  
       
        log_error("Failed to lock upload directory: %s", strerror(errno));
    } else 
    {

        log_info("Upload directory locked");
    }
}