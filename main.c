#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>



static bool g_opt_verbose_ = false;
static int g_opt_timeout_ms_ = -1;
static const char *g_opt_filepath_ = NULL;



static const char *g_command_name_ = "rfd";

static const char *SHORT_OPTIONS = "hvt:";

static const struct option LONG_OPTIONS[] = {
        { "help",       no_argument,        NULL, 'h' },
        { "verbose",    no_argument,        NULL, 'v' },
        { "timeout",    required_argument,  NULL, 't' },
        { NULL,         0,                  NULL, 0 }
    };

static void usage_(void)
{
    fprintf(stderr,
            "Usage: %s [OPTION]... FILE\n"
            "OPTIONs are:\n"
            "  -h, --help\n"
            "  -v, --verbose\n"
            "  -t, --timeout=[MSEC] (default -1)\n"
            , g_command_name_
            );
    return;
}



static int verbose_(const char *p_format_, ...)
{
    int result = 0;

    if (g_opt_verbose_) {
        va_list ap;

        va_start(ap, p_format_);
        result = vprintf(p_format_, ap);
        va_end(ap);
    }

    return result;
}



static void retrieve_options_(int argc_, char *argv_[])
{
    int c = 0;

    for ( ; /* FOREVER */; ) {
        c = getopt_long(argc_, argv_, SHORT_OPTIONS, LONG_OPTIONS, NULL);
        if (c < 0) {
            break;
        }

        switch (c) {
        case 'v':
            g_opt_verbose_ = true;
            verbose_("Verbose mode enabled.\n");
            break;

        case 't':
            g_opt_timeout_ms_ = (int)strtol(optarg, NULL, 0);
            verbose_("Timeout is %d.\n", g_opt_timeout_ms_);
            break;

        case 'h':
            usage_();
            exit(EXIT_SUCCESS);

        default:
            usage_();
            exit(EXIT_FAILURE);
        }
    }

    if (argc_ <= optind) {
        fprintf(stderr, "Any FILEs are specified.\n");
        usage_();
        exit(EXIT_FAILURE);
    }

    g_opt_filepath_ = argv_[optind];
    verbose_("FILE is %s.\n", g_opt_filepath_);

    return;
}



int main(int argc_, char *argv_[])
{
    int fd = -1;
    int ret = 0;
    struct pollfd fds = { 0 };

    g_command_name_ = basename(argv_[0]);
    retrieve_options_(argc_, argv_);

    fd = open(g_opt_filepath_, O_RDONLY);
    if (fd < 0) {
        verbose_("open(%s, O_RDONLY) failed.\n", g_opt_filepath_);
        fprintf(stderr, "Failed to open %s.\n", g_opt_filepath_);
        exit(EXIT_FAILURE);
    }
    fds.fd = fd;
    fds.events = POLLIN;
    fds.revents = 0;

    verbose_("poll() for %s, timeout=%d.\n",
            g_opt_filepath_, g_opt_timeout_ms_);
    ret = poll(&fds, 1, g_opt_timeout_ms_);
    verbose_("poll() retuened %d.\n", ret);

    close(fd), fd = -1;

    if (ret < 0) {
        exit(EXIT_FAILURE);
    }

    if (0 == ret) {
        printf("Timed out.\n");
    }

    return EXIT_SUCCESS;
}


/* vim: set ts=4 sts=4 sw=4 expandtab : */
