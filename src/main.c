#include <stdio.h>

#define APP_NAME	"DS1307"

#define VERSION_MAJOR	0
#define VERSION_MINOR	1

void print_usage(void)
{
	printf("%s %d.%d\n", APP_NAME, VERSION_MAJOR, VERSION_MINOR);
}

int main(int argc, const char* argv[])
{
	if(argc == 1){
		print_usage();
		return 0;
	}
	return 0;
}
