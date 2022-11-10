#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static int v4lconvert_helper_write(int fd, const void *b, size_t count, char *progname)
{
    size_t ret, written = 0;
    const unsigned char *buf = b;

    while (written < count) {
        ret = write(fd, buf + written, count - written);
        if (ret == -1) {
            if (errno == EINTR) {
                continue;
            }

            if (errno == EPIPE) {
                exit(0);
            }

            fprintf(stderr, "%s: error writing: %s\n", progname, strerror(errno));
            return -1;
        }

        written += ret;
    }

    return 0;
}

static int v4lconvert_helper_read(int fd, void *b, size_t count, char *progname)
{
    size_t ret, r = 0;
    unsigned char *buf = b;

    while (r < count) {
        ret = read(fd, buf + r, count - r);
        if (ret == -1) {
            if (errno == EINTR) {
                continue;
            }

            fprintf(stderr, "%s: error reading: %s\n", progname, strerror(errno));
            return -1;
        }

        if (ret == 0) {
            exit(0);
        }

        r += ret;
    }

    return 0;
}
