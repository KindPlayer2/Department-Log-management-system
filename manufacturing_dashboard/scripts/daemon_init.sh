#!/bin/bash
DAEMON_BIN="/home/sean/manufacturing_dashboard/bin/daemon"
PID_FILE="/var/run/manufacturing_daemon.pid"

case "$1" in
    start)
        if [ -f "$PID_FILE" ]; then
            echo "Daemon already running"
        else
            $DAEMON_BIN && echo "Daemon started"
        fi
        ;;
    stop)
        if [ -f "$PID_FILE" ]; then
            kill -SIGTERM $(cat $PID_FILE) && rm $PID_FILE
            echo "Daemon stopped"
        else
            echo "Daemon not running"
        fi
        ;;
    backup-now)
        kill -SIGUSR1 $(cat $PID_FILE) 2>/dev/null || echo "Daemon not running"
        ;;
    check-missing)
        kill -SIGUSR2 $(cat $PID_FILE) 2>/dev/null || echo "Daemon not running"
        ;;
    *)
        echo "Usage: $0 {start|stop|backup-now}"
        exit 1
        ;;
esac