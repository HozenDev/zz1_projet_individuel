#include "utils.h"
#include "../log/log.h"

/**
 * @brief get display resolution
 *
 * @param int * w, width of the display
 * @param int * h, width of the display
 */
void utils_get_screen_resolution(int * w, int * h)
{
    FILE *fp;

    /* Open the command for reading. */
    fp = popen("xdpyinfo | grep dimensions | sed -r 's/^[^0-9]*([0-9]+x[0-9]+).*$/\\1/'", "r");
    if (fp == NULL) {
        zlog(stderr, ERROR, "Failed to run command", NULL);
        exit(1);
    }

    fscanf(fp, "%dx%d", w, h);

    /* close */
    pclose(fp);
}
