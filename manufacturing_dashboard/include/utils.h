#ifndef UTILS_H
#define UTILS_H

// Function declarations
void monitor_upload_dir();
void lock_directories();
void unlock_directories();
void backup_and_transfer();
int file_exists(const char *path);
void ensure_directory_exists(const char *path);
void check_missing_files();  // Add this line
void lock_upload_directory();  // Add this line

#endif