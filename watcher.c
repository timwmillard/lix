#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#define WATCHER_POLL_TIME

int file_is_modified(const char *path, time_t oldMTime) {
    struct stat file_stat;
    int err = stat(path, &file_stat);
    if (err != 0) {
        perror(" [file_is_modified] stat");
        exit(errno);
    }
    return file_stat.st_mtime > oldMTime;
}


void *watcher_poll(void *args)
{
    char *filename = (char *) args;

    static time_t now;

    while (1) {
        now = time(NULL);

        if (file_is_modified(filename, now)) {
            // signal update
        }
        sleep(WATCHER_POLL_TIME);
    }
}

pthread_t start_watcher(char *filename){
    pthread_t t;
    pthread_create(&t, NULL, watcher_poll, filename);
    return t;
}
