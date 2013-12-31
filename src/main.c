#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

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
	printf("-w<Register to write>=<value>\n");
}

const char    hexlookup[] = {"0123456789ABCDEF"};

void hex_to_ascii(const unsigned char *source, char *dest, unsigned int source_length)
{
    unsigned int i;
    unsigned char temp;

  for (i = 0; i < source_length; i++) {
        temp = source[i];
        temp >>= 4;
        dest[i*2] = hexlookup[temp];

        temp = source[i];
        temp &= 0x0f;
        dest[(i*2)+1] = hexlookup[temp];
    }
}

int ascii_to_hex(const char *source, int source_len, char *dest)
{
	int i, pos=0;

	if((source_len > 1) && (source_len%2))
		return -1;		/* Error */

	for(i=0; i<source_len; i++){
		dest[pos] = 0;

		if(source_len > 1){
			if(((source[i] <= 'F') && (source[i] >= 'A')) || ((source[i] <= 'f') && (source[i] >= 'a'))){
				dest[pos] = (toupper(source[i]) - 'A' + 0x0a) << 4;
			}else if((source[i] <= '9') && (source[i] >= '0')){
				dest[pos] = (source[i] - '0') << 4;
			}else
				return -1;	/* Error */

			i++;
		}

		if(((source[i] <= 'F') && (source[i] >= 'A')) || ((source[i] <= 'f') && (source[i] >= 'a'))){
			dest[pos] |= (toupper(source[i]) - 'A' + 0x0a) & 0x0f;
		}else if((source[i] <= '9') && (source[i] >= '0')){
			dest[pos] |= (source[i] - '0') & 0x0f;
		}else
			return -1;	/* Error */

		pos++;
	}

	return pos;
}

int ds1307_init(void)
{
	char i2c_hex_addr[10];
	int i2c_addr_len=0;

	if((i2c_addr_len = ascii_to_hex(i2c_addr, strlen(i2c_addr), i2c_hex_addr)) <= 0){
		printf("Device address format error\n");
		exit(1);
	}

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
	if (ioctl(fd, I2C_SLAVE, i2c_hex_addr[0]) < 0)
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
	char buffer[2];

	buffer[0] = reg;
	buffer[1] = val;

	if (write(fd, buffer, 2) != 2)
	{
		perror("Failed to write to the i2c bus");
		exit(1);
	}

	return 0;
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

		}else if (memcmp(argv[i], "-r", 2) == 0){
			/* Place holder */

		}else if (memcmp(argv[i], "-w", 2) == 0){
			/* Place holder */

		}else{
			printf("Invalid argument %s\n", argv[i]);
			exit(1);
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

	/* Perform writing of individual registers */
	for (i=1; i< argc; i++){

		if (memcmp(argv[i], "-w", 2) == 0){
			char *reg, *val;
			char reg_hex[10], val_hex[10];

			reg = strtok((char*)&argv[i][2], "=");
			val = strtok(NULL, "\0");

			if((reg == NULL) || (val == NULL)){
				printf("1 Invalid argument %s\n", argv[i]);
				ds1307_cleanup();
				exit(1);
			}

			if(ascii_to_hex(reg, strlen(reg), reg_hex) <= 0){
				printf("2 Invalid argument %s\n", argv[i]);
				ds1307_cleanup();
				exit(1);
			}

			if(ascii_to_hex(val, strlen(val), val_hex) <= 0){
				printf("3 Invalid argument %s\n", argv[i]);
				ds1307_cleanup();
				exit(1);
			}

			ds1307_write(reg_hex[0], val_hex[0]);
			printf("Wrote register 0x%02x = 0x%02x\n", reg_hex[0], val_hex[0]);
		}
	}

	/* Perform reading of individual registers */
	for (i=1; i< argc; i++){

		if (memcmp(argv[i], "-r", 2) == 0){
			char ret = ds1307_read((char)atoi(&argv[i][2]));
			printf("Register %d = 0x%02x\n", (char)atoi(&argv[i][2]), ret);
		}
	}

	ds1307_cleanup();

	return 0;
}
