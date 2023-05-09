#include <stdio.h>
#include <stdlib.h>

typedef enum {
        LOG_INFO,
        LOG_FATAL,
} LOG_DEBUG;

void LOGGING(LOG_DEBUG log, const char *);
