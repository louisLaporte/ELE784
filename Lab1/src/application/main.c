#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#define SCULL_IOC_MAGIC 'a'

#define SCULL_GETNUMDATA        _IOR(SCULL_IOC_MAGIC, 0, int)
#define SCULL_GETNUMREADER      _IOR(SCULL_IOC_MAGIC, 1, int)
#define SCULL_GETBUFSIZE        _IOR(SCULL_IOC_MAGIC, 2, int)
#define SCULL_SETBUFSIZE        _IOW(SCULL_IOC_MAGIC, 3, int)

void main (void)
{
	int i, fd, len, ioctl_code, arg, result;
	char ch, write_buf[100], read_buf[100];

	fd = open("/dev/scull_Node", O_RDWR);

	if (fd == -1)
	{
		printf("Error in opening file \n");
		exit(-1);
	}

	printf ("Press r to read from device or w to write the device or i to use ioctl \n");
	scanf ("%c", &ch);

	if(ch == 'w'){
		printf ("Enter the data to be written into device\n");
		scanf ("%s", write_buf);
		write(fd, write_buf, strlen(write_buf));

	} else if (ch == 'r')
	{
	printf ("Nombre de caractères à lire :\n");
	scanf ("%d", &len);
	read(fd, read_buf, len);
	printf ("The data in the device is %s\n", read_buf);

	}else if (ch == 'i')
	{
	printf ("Quelle commande voulez-vous utiliser ?\n");
	printf ("0 - GetNumData\n");
	printf ("1 - GetNumReader\n");
	printf ("2 - GetBufSize\n");
	printf ("3 - SetBufSize\n");
	do
	{
		scanf ("%d", &ioctl_code);
	}while(ioctl_code < 0 || ioctl_code > 3);

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
	}
	}else
	{
		printf("Wrong choice \n");
	}


close(fd);
}
