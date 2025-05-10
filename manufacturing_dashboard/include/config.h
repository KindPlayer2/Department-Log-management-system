#ifndef CONFIG_H
#define CONFIG_H

//  Directories
#define UPLOAD_DIR      "/home/sean/manufacturing_dashboard/upload"
#define REPORTING_DIR   "/home/sean/manufacturing_dashboard/reporting"
#define BACKUP_DIR      "/home/sean/manufacturing_dashboard/backups"
#define LOG_DIR         "/home/sean/manufacturing_dashboard/logs"

//  Files
#define PID_FILE    "/var/run/manufacturing_daemon.pid"
#define CHANGE_LOG  LOG_DIR "/changes.log"
#define ERROR_LOG   LOG_DIR "/errors.log"

//  TImes
#define BACKUP_TIME "01:00"

//  Locking
#define LOCK_PERM   0555 // Read+execute for all, no write
#define UNLOCK_PERM 0775 // Full access for owner/group

#endif