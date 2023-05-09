#include "logging.h"

static char *ANSI_LIGHT_GREEN = "\033[1;32m";
// static char *ANSI_LIGHT_BLUE  = "\x1b[94m";
static char *ANSI_RED         = "\x1b[31m";
static char *ANSI_CLEAR       = "\x1b[0m";


void LOGGING(LOG_DEBUG log, const char *message) {
	switch(log) {
		case LOG_INFO:
			printf("\t%s[LOGGING] %s%s\n", ANSI_LIGHT_GREEN, message, ANSI_CLEAR); break;
		case LOG_FATAL:
			printf("\t%s[FATAL] %s%s\n", ANSI_RED, message, ANSI_CLEAR); break;
	}
}
