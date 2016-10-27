#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define SCULL_IOC_MAGIC 'a'
#define SCULL_IOC_MAXNR 3

#define SCULL_GETNUMDATA _IOR(SCULL_IOC_MAGIC, 0, int)
#define SCULL_GETNUMREADER _IOR(SCULL_IOC_MAGIC, 1, int)
#define SCULL_GETBUFSIZE _IOR(SCULL_IOC_MAGIC, 2, int)
#define SCULL_SETBUFSIZE _IOW(SCULL_IOC_MAGIC, 3, int)
#define BUF_SIZE 256

void print_main_menu();
void print_ioctl_menu();
void is_open(int fd);
char write_buf[BUF_SIZE];
char read_buf[BUF_SIZE];
const char *device = "/dev/scull_Node";

int main (void)
{

        int fd;
        int nb_data = 0;
        int arg;
        int result;
        int i = 0;
        char c = -1;
        
        system("clear");
        print_main_menu();

        while (c != 'q') {
                printf("main> ");
                while ((c = getchar()) == '\n'){}
                switch (c) {
                case 'r': 
                        printf("Data length to read: ");
                        scanf("%d",&nb_data);
                        fd = open(device, (O_RDONLY | O_NONBLOCK), S_IRUSR);
                        is_open(fd);
                        read(fd, read_buf, nb_data);
                        for (i = 0; i < nb_data; i++)
                                printf("%c%s", read_buf[i], i == nb_data - 1 ? "\n" : "");
                        close(fd);
                        break;
                case 'R':
                        printf("Data length to read: ");
                        scanf("%d",&nb_data);
                        fd = open(device, (O_RDONLY), S_IRUSR);
                        is_open(fd);
                        read(fd, read_buf, nb_data);
                        for (i = 0; i < nb_data; i++)
                                printf("%c%s", read_buf[i], i == nb_data - 1 ? "\n" : "");
                        close(fd);
                        break;
                case 'w':
                        printf("Data length to write: ");
                        scanf("%d",&nb_data);
                        fd = open(device, (O_WRONLY | O_NONBLOCK), S_IWUSR);
                        is_open(fd);
                        for (i = 0; i < nb_data; i++)
                                write_buf[i] = i + 0x20;
                        write(fd, write_buf, nb_data);
                        printf("%s\n",write_buf);
                        close(fd);
                        break;
                case 'W':
                        printf("Data length to write: ");
                        scanf("%d",&nb_data);
                        fd = open(device, (O_WRONLY), S_IWUSR);
                        is_open(fd);
                        for (i = 0; i < nb_data; i++)
                                write_buf[i] = i + 0x20;
                        write(fd, write_buf, nb_data);
                        printf("%s\n",write_buf);
                        close(fd);
                        break;
                case 'x':
                        printf("Data length to write: ");
                        scanf("%d",&nb_data);
                        fd = open(device, (O_RDWR | O_NONBLOCK));
                        is_open(fd);
                        for (i = 0; i < nb_data; i++)
                                write_buf[i] = i + 0x20;
                        write(fd, write_buf, nb_data);
                        printf("%s\n",write_buf);
                        close(fd);
                        break;
                case 'X':
                        printf("Data length to write: ");
                        scanf("%d",&nb_data);
                        fd = open(device, (O_RDWR));
                        is_open(fd);
                        for (i = 0; i < nb_data; i++)
                                write_buf[i] = i + 0x20;
                        write(fd, write_buf, nb_data);
                        printf("%s\n",write_buf);
                        close(fd);
                        break;
		case 'i':
                        print_ioctl_menu();
                        while (c != 'q') {
                                printf("ioctl> ");
                                while ((c = getchar()) == '\n'){}
                                switch (c) {
                                        case '0' : 
                                                result = ioctl(fd, SCULL_GETNUMDATA, &arg);
                                                printf("Data length: %d\n", arg);
                                                break;
                                        case '1' : 
                                                result = ioctl(fd, SCULL_GETNUMREADER, &arg);
                                                printf("Number of reader: %d\n", arg);
                                                break;
                                        case '2' : 
                                                result = ioctl(fd, SCULL_GETBUFSIZE, &arg);
                                                printf("Buffer size: %d\n", arg);
                                                break;
                                        case '3' :
                                                printf("New buffer size: ");
                                                scanf("%d", &arg);
                                                result = ioctl(fd, SCULL_SETBUFSIZE, &arg);
                                                printf("Result: %d\n", result);
                                                break;
                                        default:
                                                break;
                                }
                        }
                        c = -1;     
                        break;
                case 'm':
                        print_main_menu();
                        break;
                default:
                        break;
                }

        }
        return EXIT_SUCCESS;
}

void is_open(int fd)
{
        if (fd == -1) {
                printf("Error in opening file \n");
                exit(-1);
        }
}
void print_main_menu()
{
        printf("Menu\n\n");
        printf(" r : non-blocking read\n");
        printf(" R : blocking read\n");
        printf(" w : non-blocking write\n");
        printf(" W : blocking write\n");
        printf(" i : ioctl\n\n");
        printf(" m : show menu\n");
        printf(" q : quit\n");
}

void print_ioctl_menu()
{
        printf(" 0 : get number of data\n");
        printf(" 1 : get number of reader\n");
        printf(" 2 : get buffer size\n");
        printf(" 3 : set buffer size (root)\n");
        printf(" q : quit ioctl\n");
}
