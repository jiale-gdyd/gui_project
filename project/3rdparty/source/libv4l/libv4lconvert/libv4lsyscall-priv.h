#ifndef LIBV4L_LIBV4LSYSCALL_PRIV_H
#define LIBV4L_LIBV4LSYSCALL_PRIV_H

#include <sys/time.h>
#include <sys/syscall.h>
#include <linux/types.h>
#include <linux/ioctl.h>

#ifdef __NR_mmap2
#if !defined(SYS_mmap2)
#define SYS_mmap2                                   __NR_mmap2
#endif
#define MMAP2_PAGE_SHIFT                            12
#else
#define SYS_mmap2                                   SYS_mmap
#define MMAP2_PAGE_SHIFT                            0
#endif

#undef SYS_OPEN
#undef SYS_CLOSE
#undef SYS_IOCTL
#undef SYS_READ
#undef SYS_WRITE
#undef SYS_MMAP
#undef SYS_MUNMAP

#ifndef CONFIG_SYS_WRAPPER

#ifdef SYS_openat
#define SYS_OPEN(file, oflag, mode)                 syscall(SYS_openat, AT_FDCWD, (const char *)(file), (int)(oflag), (mode_t)(mode))
#else
#define SYS_OPEN(file, oflag, mode)                 syscall(SYS_open, (const char *)(file), (int)(oflag), (mode_t)(mode))
#endif

#define SYS_CLOSE(fd)                               syscall(SYS_close, (int)(fd))
#define SYS_IOCTL(fd, cmd, arg)                     syscall(SYS_ioctl, (int)(fd), (unsigned long)(cmd), (void *)(arg))
#define SYS_READ(fd, buf, len)                      syscall(SYS_read, (int)(fd), (void *)(buf), (size_t)(len));
#define SYS_WRITE(fd, buf, len)                     syscall(SYS_write, (int)(fd), (const void *)(buf), (size_t)(len));

#define SYS_MMAP(addr, len, prot, flags, fd, off)   syscall(SYS_mmap2, (void *)(addr), (size_t)(len), (int)(prot), (int)(flags), (int)(fd), (off_t)((off) >> MMAP2_PAGE_SHIFT))
#define SYS_MUNMAP(addr, len)                       syscall(SYS_munmap, (void *)(addr), (size_t)(len))

#else

int v4lx_open_wrapper(const char *, int, int);
int v4lx_close_wrapper(int);

int v4lx_ioctl_wrapper(int, unsigned long, void *);

int v4lx_read_wrapper(int, void *, size_t);
int v4lx_write_wrapper(int, const void *, size_t);

void *v4lx_mmap_wrapper(void *, size_t, int, int, int, off_t);
int v4lx_munmap_wrapper(void *, size_t);

#define SYS_OPEN(...)                               v4lx_open_wrapper(__VA_ARGS__)
#define SYS_CLOSE(...)                              v4lx_close_wrapper(__VA_ARGS__)
#define SYS_IOCTL(...)                              v4lx_ioctl_wrapper(__VA_ARGS__)
#define SYS_READ(...)                               v4lx_read_wrapper(__VA_ARGS__)
#define SYS_WRITE(...)                              v4lx_write_wrapper(__VA_ARGS__)
#define SYS_MMAP(...)                               v4lx_mmap_wrapper(__VA_ARGS__)
#define SYS_MUNMAP(...)                             v4lx_munmap_wrapper(__VA_ARGS__)

#endif

#endif
