#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <limits.h>

/* ================= 配置区 ================= */

static const char *SRC_PREFIX = "/opt/sslvpnclient";

/* ================= 工具函数 ================= */

static const char* remap_path(const char *path, char *buf, size_t size) {
    if (!path) return path;

    size_t plen = strlen(SRC_PREFIX);
    if (strncmp(path, SRC_PREFIX, plen) == 0) {
        const char *home = getenv("HOME");
        if (!home) home = "";
        snprintf(buf, size, "%s%s", home, path);
        return buf;
    }
    return path;
}

/* ================= open / open64 ================= */

int open(const char *pathname, int flags, ...) {
    static int (*real_open)(const char*, int, ...) = NULL;
    if (!real_open)
        real_open = dlsym(RTLD_NEXT, "open");

    va_list ap;
    va_start(ap, flags);
    mode_t mode = (flags & O_CREAT) ? va_arg(ap, int) : 0;
    va_end(ap);

    char buf[PATH_MAX];
    const char *p = remap_path(pathname, buf, sizeof(buf));

    return (flags & O_CREAT)
        ? real_open(p, flags, mode)
        : real_open(p, flags);
}

int open64(const char *pathname, int flags, ...) {
    static int (*real_open64)(const char*, int, ...) = NULL;
    if (!real_open64)
        real_open64 = dlsym(RTLD_NEXT, "open64");

    va_list ap;
    va_start(ap, flags);
    mode_t mode = (flags & O_CREAT) ? va_arg(ap, int) : 0;
    va_end(ap);

    char buf[PATH_MAX];
    const char *p = remap_path(pathname, buf, sizeof(buf));

    return (flags & O_CREAT)
        ? real_open64(p, flags, mode)
        : real_open64(p, flags);
}

/* ================= openat ================= */

int openat(int dirfd, const char *pathname, int flags, ...) {
    static int (*real_openat)(int, const char*, int, ...) = NULL;
    if (!real_openat)
        real_openat = dlsym(RTLD_NEXT, "openat");

    va_list ap;
    va_start(ap, flags);
    mode_t mode = (flags & O_CREAT) ? va_arg(ap, int) : 0;
    va_end(ap);

    char buf[PATH_MAX];
    const char *p = remap_path(pathname, buf, sizeof(buf));

    return (flags & O_CREAT)
        ? real_openat(dirfd, p, flags, mode)
        : real_openat(dirfd, p, flags);
}

/* ================= fopen ================= */

FILE *fopen(const char *pathname, const char *mode) {
    static FILE* (*real_fopen)(const char*, const char*) = NULL;
    if (!real_fopen)
        real_fopen = dlsym(RTLD_NEXT, "fopen");

    char buf[PATH_MAX];
    return real_fopen(remap_path(pathname, buf, sizeof(buf)), mode);
}

FILE *fopen64(const char *pathname, const char *mode) {
    static FILE* (*real_fopen64)(const char*, const char*) = NULL;
    if (!real_fopen64)
        real_fopen64 = dlsym(RTLD_NEXT, "fopen64");

    char buf[PATH_MAX];
    return real_fopen64(remap_path(pathname, buf, sizeof(buf)), mode);
}

/* ================= access ================= */

int access(const char *pathname, int mode) {
    static int (*real_access)(const char*, int) = NULL;
    if (!real_access)
        real_access = dlsym(RTLD_NEXT, "access");

    char buf[PATH_MAX];
    return real_access(remap_path(pathname, buf, sizeof(buf)), mode);
}

/* ================= stat ================= */

int stat(const char *pathname, struct stat *st) {
    static int (*real_stat)(const char*, struct stat*) = NULL;
    if (!real_stat)
        real_stat = dlsym(RTLD_NEXT, "stat");

    char buf[PATH_MAX];
    return real_stat(remap_path(pathname, buf, sizeof(buf)), st);
}

int lstat(const char *pathname, struct stat *st) {
    static int (*real_lstat)(const char*, struct stat*) = NULL;
    if (!real_lstat)
        real_lstat = dlsym(RTLD_NEXT, "lstat");

    char buf[PATH_MAX];
    return real_lstat(remap_path(pathname, buf, sizeof(buf)), st);
}

/* glibc 内部版本 */
int __xstat(int ver, const char *pathname, struct stat *st) {
    static int (*real___xstat)(int, const char*, struct stat*) = NULL;
    if (!real___xstat)
        real___xstat = dlsym(RTLD_NEXT, "__xstat");

    char buf[PATH_MAX];
    return real___xstat(ver, remap_path(pathname, buf, sizeof(buf)), st);
}

int __lxstat(int ver, const char *pathname, struct stat *st) {
    static int (*real___lxstat)(int, const char*, struct stat*) = NULL;
    if (!real___lxstat)
        real___lxstat = dlsym(RTLD_NEXT, "__lxstat");

    char buf[PATH_MAX];
    return real___lxstat(ver, remap_path(pathname, buf, sizeof(buf)), st);
}

/* ================= realpath ================= */

char *realpath(const char *path, char *resolved) {
    static char* (*real_realpath)(const char*, char*) = NULL;
    if (!real_realpath)
        real_realpath = dlsym(RTLD_NEXT, "realpath");

    char buf[PATH_MAX];
    return real_realpath(remap_path(path, buf, sizeof(buf)), resolved);
}
