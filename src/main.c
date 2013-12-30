#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#define APP_NAME	"DS1307"

#define VERSION_MAJOR	0
#define VERSION_MINOR	1

static char i2c_addr[50];
static char i2c_bus[50];
static int fd = 0;

void print_usage(void)
{
	printf("%s %d.%d\n", APP_NAME, VERSION_MAJOR, VERSION_MINOR);
	printf("-a<I2C device address>\n");
	printf("-b<I2C bus name>\n");
	printf("-r<Register to read>\n");
/*	printf("-w<Register to write>=<value>\n");
 */
}

int ds1307_init(void)
{
	/* Open up the I2C bus
	*/
	fd = open(i2c_bus, O_RDWR);
	if (fd == -1)
	{
		perror(i2c_bus);
		exit(1);
	}

	/* Specify the address of the slave device.
	 */
	if (ioctl(fd, I2C_SLAVE, atoi(i2c_addr)) < 0)
	{
		perror("Failed to acquire bus access and/or talk to slave");
		exit(1);
	}

	return 0;
}

char ds1307_read(char reg)
{
	char retval=0;

	/* Write a byte to the slave.
	 */
	if (write(fd, &reg, 1) != 1)
	{
	  perror("Failed to write to the i2c bus");
	  exit(1);
	}

	/* Read a byte from the slave.
	 */
	if (read(fd,&retval,1) != 1)
	{
	  perror("Failed to read from the i2c bus");
	  exit(1);
	}

	return retval;
}

char ds1307_write(char reg, char val)
{
	char retval=0;

	/* Write a byte to the slave.
	 */
	if (write(fd, &reg, 1) != 1)
	{
		perror("Failed to write to the i2c bus");
		exit(1);
	}

	/* Write the value to the slave.
	 */
	if (write(fd,&val,1) != 1)
	{
		perror("Failed to write to the i2c bus");
		exit(1);
	}

	return retval;
}

int ds1307_cleanup(void)
{
	if(fd)
		close(fd);

	return 0;
}

int main(int argc, const char* argv[])
{
	int i;

	memset(i2c_addr, 0, sizeof(i2c_addr));
	memset(i2c_bus, 0, sizeof(i2c_bus));

	if(argc == 1){
		print_usage();
		return 0;
	}

	for (i=1; i< argc; i++){
		if (memcmp(argv[i], "-a", 2) == 0){
			if(strlen(&argv[i][2]) >= sizeof(i2c_addr)){
				printf("Invalid device address\n\n");
				print_usage();
				return -1;
			}

			printf("Address %s\n", &argv[i][2]);
			memcpy(i2c_addr, &argv[i][2], strlen(&argv[i][2]));

		}else if (memcmp(argv[i], "-b", 2) == 0){
			if(strlen(&argv[i][2]) >= sizeof(i2c_bus)){
				printf("Invalid bus name\n\n");
				print_usage();
				return -1;
			}

			printf("Bus %s\n", &argv[i][2]);
			memcpy(i2c_bus, &argv[i][2], strlen(&argv[i][2]));
		}
	}

	if(strlen(i2c_addr) == 0){
		printf("Device address missing\n");
		exit(1);
	}

	if(strlen(i2c_bus) == 0){
		printf("Bus name missing\n");
		exit(1);
	}

	ds1307_init();

	for (i=1; i< argc; i++){

		if (memcmp(argv[i], "-r", 2) == 0){
			char ret = ds1307_read((char)atoi(&argv[i][2]));
			printf("Register %d = 0x%02x\n", (char)atoi(&argv[i][2]), ret);
		}
	}

	ds1307_cleanup();

	return 0;
}
