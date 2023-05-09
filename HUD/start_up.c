#include "contain_files.h"
#include "display_files.h"
#include "logging.h"
#include <dirent.h>


void display_hud(DIRECTORY *directory);

int main() {
	LOGGING(LOG_INFO, "Calling get_files");
	init_terminal();
	DIRECTORY *directories = malloc(sizeof(DIRECTORY));
	get_files(directories, ".");
	display_hud(directories);
	return 0;
}
