#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#include "libv4lconvert-priv.h"

#define READ_END            0
#define WRITE_END           1

static int v4lconvert_helper_start(struct v4lconvert_data *data, const char *helper)
{
    if (pipe(data->decompress_in_pipe)) {
        V4LCONVERT_ERR("with helper pipe: %s\n", strerror(errno));
        goto error;
    }

    if (pipe(data->decompress_out_pipe)) {
        V4LCONVERT_ERR("with helper pipe: %s\n", strerror(errno));
        goto error_close_in_pipe;
    }

    data->decompress_pid = fork();
    if (data->decompress_pid == -1) {
        V4LCONVERT_ERR("with helper fork: %s\n", strerror(errno));
        goto error_close_out_pipe;
    }

    if (data->decompress_pid == 0) {
        close(data->decompress_out_pipe[WRITE_END]);
        close(data->decompress_in_pipe[READ_END]);

        if (dup2(data->decompress_out_pipe[READ_END], STDIN_FILENO) == -1) {
            perror("libv4lconvert: error with helper dup2");
            exit(1);
        }

        if (dup2(data->decompress_in_pipe[WRITE_END], STDOUT_FILENO) == -1) {
            perror("libv4lconvert: error with helper dup2");
            exit(1);
        }

        execl(helper, helper, NULL);

        perror("libv4lconvert: error starting helper");
        exit(1);
    } else {
        close(data->decompress_out_pipe[READ_END]);
        close(data->decompress_in_pipe[WRITE_END]);
    }

    return 0;

error_close_out_pipe:
    close(data->decompress_out_pipe[READ_END]);
    close(data->decompress_out_pipe[WRITE_END]);

error_close_in_pipe:
    close(data->decompress_in_pipe[READ_END]);
    close(data->decompress_in_pipe[WRITE_END]);

error:
    return -1;
}

static int v4lconvert_helper_write(struct v4lconvert_data *data, const void *b, size_t count)
{
    size_t ret, written = 0;
    const unsigned char *buf = b;

    while (written < count) {
        ret = write(data->decompress_out_pipe[WRITE_END], buf + written, count - written);
        if (ret == -1) {
            if (errno == EINTR) {
                continue;
            }

            V4LCONVERT_ERR("writing to helper: %s\n", strerror(errno));
            return -1;
        }

        written += ret;
    }

    return 0;
}

static int v4lconvert_helper_read(struct v4lconvert_data *data, void *b, size_t count)
{
    size_t ret, r = 0;
    unsigned char *buf = b;

    while (r < count) {
        ret = read(data->decompress_in_pipe[READ_END], buf + r, count - r);
        if (ret == -1) {
            if (errno == EINTR) {
                continue;
            }

            V4LCONVERT_ERR("reading from helper: %s\n", strerror(errno));
            return -1;
        }

        if (ret == 0) {
            V4LCONVERT_ERR("reading from helper: unexpected EOF\n");
            return -1;
        }

        r += ret;
    }

    return 0;
}

int v4lconvert_helper_decompress(struct v4lconvert_data *data, const char *helper, const unsigned char *src, int src_size, unsigned char *dest, int dest_size, int width, int height, int flags)
{
    int r;

    if (data->decompress_pid == -1) {
        if (v4lconvert_helper_start(data, helper)) {
            return -1;
        }
    }

    if (v4lconvert_helper_write(data, &width, sizeof(int))) {
        return -1;
    }

    if (v4lconvert_helper_write(data, &height, sizeof(int))) {
        return -1;
    }

    if (v4lconvert_helper_write(data, &flags, sizeof(int))) {
        return -1;
    }

    if (v4lconvert_helper_write(data, &src_size, sizeof(int))) {
        return -1;
    }

    if (v4lconvert_helper_write(data, src, src_size)) {
        return -1;
    }

    if (v4lconvert_helper_read(data, &r, sizeof(int))) {
        return -1;
    }

    if (r < 0) {
        V4LCONVERT_ERR("decompressing frame data\n");
        return -1;
    }

    if (dest_size < r) {
        V4LCONVERT_ERR("destination buffer to small\n");
        return -1;
    }

    return v4lconvert_helper_read(data, dest, r);
}

void v4lconvert_helper_cleanup(struct v4lconvert_data *data)
{
    int status;

    if (data->decompress_pid != -1) {
        close(data->decompress_out_pipe[WRITE_END]);
        close(data->decompress_in_pipe[READ_END]);
        waitpid(data->decompress_pid, &status, 0);
        data->decompress_pid = -1;
    }
}
