#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

main ( )
 {
        int i,fd;
        char ch, write_buf[100], read_buf[100];
        fd = open("/dev/char_dev", O_RDWR);
        if (fd == -1)
        {
                printf("Error in opening file \n");
                exit(-1);
        }
        printf ("Press r to read from device or w to write the device \n");
        scanf ("%c", &ch);

#define BUF_SIZE 256

void print_menu();
void is_open(int fd);
char write_buf[BUF_SIZE];
char read_buf[BUF_SIZE];

const char *device = "/dev/scull_Node";
int main (void)
{

        int fd;
        int nb_data = 0;
        int i = 0;
        char c = -1;
        
        system("clear");

        print_menu();
        while ( c != 'q'){
                printf("> ");
                c = getchar();
                switch (c) {
                case 'r': 
                        printf("Nb data to read: ");
                        scanf("%d",&nb_data);
                        fd = open(device, (O_RDONLY | O_NONBLOCK), S_IRUSR);
                        is_open(fd);
                        read(fd, read_buf, nb_data);
                        for (i = 0; i < nb_data; i++)
                                printf("%c%s", read_buf[i], i == nb_data - 1 ? "\n" : "");
                        close(fd);
                        break;
                case 'R':
                        break;
                case 'w':
                        printf("Nb data to write: ");
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
                        break;
                case 'x':
                        break;
                case 'X':
                        break;
                case 'm':
                        print_menu();
                        break;
				case 'i':
printf ("Quelle commande voulez-vous utiliser ?\n");
	printf ("0 - GetNumData\n");
	printf ("1 - GetNumReader\n");
	printf ("2 - GetBufSize\n");
	printf ("3 - SetBufSize\n");
						switch(ioctl_code)
	{
		case 0 : result = ioctl(fd, SCULL_GETNUMDATA, &arg);
				printf("Nombre de données : %d\n", arg);
				break;
		case 1 : result = ioctl(fd, SCULL_GETNUMREADER, &arg);
				printf("Nombre de lecteur : %d\n", arg);
				break;
		case 2 : result = ioctl(fd, SCULL_GETBUFSIZE, &arg);
				printf("Taille du buffer : %d\n", arg);
				break;
		case 3 :
				printf("Nouvelle taille de buffer :\n");
				scanf("%d", &arg);
				result = ioctl(fd, SCULL_SETBUFSIZE, &arg);
				printf("Resultat de la commande : %d\n", result);
				break;
		default:
				break;
	}
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
void print_menu()
{

                printf("Menu\n\n");
                printf(" r : non-blocking read\n");
                printf(" R : blocking read\n");
                printf(" w : non-blocking write\n");
                printf(" W : blocking write\n\n");
                printf("[0] get number of data\n");
                printf("[1] get number of reader\n");
                printf("[2] get buffer size\n");
                printf("[3] set buffer size\n");
                printf(" m : show menu\n");
                printf(" q : quit\n");


}
