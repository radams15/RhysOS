#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char** argv) {
    int fh;
    struct TimeDelta time;

    fh = open("/dev/time");

    read(fh, &time, sizeof(struct TimeDelta));

    close(fh);

    printf("%d:%d:%d:%d %s\n",
        time.hr > 12? time.hr-12 : time.hr,
        time.min, time.sec, time.tick,
        time.hr > 12? "pm" : "am");

    return 0;
}
