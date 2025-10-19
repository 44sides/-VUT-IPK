#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h> 
#include<netinet/in.h>
#include<unistd.h>

float getCPUload()
{
	FILE* file;
	unsigned long long int prevuser, prevnice, prevsystem, previdle, previowait, previrq, prevsoftirq, prevsteal;
	unsigned long long int user, nice, system, idle, iowait, irq, softirq, steal;
	unsigned long long int PrevIdle, Idle, PrevNonIdle, NonIdle, PrevTotal, Total, totald, idled;
	float CPU_Percentage;

	if (!(file = fopen("/proc/stat", "r")))
	{
    	perror("Could not open /proc/stat");
    	return -1;
    }

	fscanf(file, "%*s %llu %llu %llu %llu %llu %llu %llu %llu", &prevuser, &prevnice, &prevsystem, &previdle, &previowait, &previrq, &prevsoftirq, &prevsteal);
	fclose(file);

	sleep(1);

	if (!(file = fopen("/proc/stat", "r")))
	{
    	perror("Could not open /proc/stat");
    	return -1;
    }

	fscanf(file, "%*s %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
	fclose(file);

	PrevIdle = previdle + previowait;
	Idle = idle + iowait;

	PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
	NonIdle = user + nice + system + irq + softirq + steal;

	PrevTotal = PrevIdle + PrevNonIdle;
	Total = Idle + NonIdle;

	totald = Total - PrevTotal;
	idled = Idle - PrevIdle;

	CPU_Percentage = (totald - idled)/(long double)totald * 100;

	return CPU_Percentage;
}

void getHostname(char *name)
{
	FILE *file;
	if (!(file = fopen("/proc/sys/kernel/hostname", "r")))
	{
    	perror("Could not open /proc/sys/kernel/hostname");
    }
	fscanf(file, "%s", name);
	fclose(file);
}

void getCPUname(char *name)
{
	FILE *file;
	if (!(file = popen("cat /proc/cpuinfo | grep \"model name\" | head -n 1 | awk '{for(i=4;i<=NF;i++) printf $i \" \"}'", "r")))
	{
    	perror("Could not open /proc/cpuinfo");
    }
	fgets(name,100,file);
	fclose(file);
}

int main(int argc , char *argv[])
{
	float cpuload;
	char hostname[256], cpuname[128];
	int sockfd, new_sockfd, valread;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	char buffer[1024], response[1024];
	char OKmessage[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n";
	char BADmessage[] = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain;\r\n\r\nBad Request\n";

	if (argc != 2)
	{
		printf("The argument is uncorrect\n");
		return 0;
	}

	char portstr[7]; strcpy(portstr, argv[1]);
	int port = atoi(portstr);

	for(int i=0; portstr[i]!='\0'; i++)
	{
		if(!(isdigit(portstr[i])))
		{
			printf("The argument is uncorrect\n");
			return 0;
		}
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("Could not create socket\n");
		return 0;
	}

    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
	{
        perror("setsockopt(SO_REUSEADDR) failed");
		return 0;
	}
	#ifdef SO_REUSEPORT
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
	{
        perror("setsockopt(SO_REUSEPORT) failed");
		return 0;
	}
	#endif

	address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
        perror("bind failed");
		return 0;
	}

	if (listen(sockfd, 3) < 0)
    {
        perror("listen failed");
		return 0;
    }

while(true)
{
	new_sockfd = accept(sockfd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
	if (new_sockfd == -1)
	{
		perror("accept failed");
		return 0;
	}

	valread = read(new_sockfd, buffer, 1023);
	buffer[valread] = '\0';
    printf("%s",buffer);

	if (!(strncmp("GET /hostname ", buffer, 14)))
	{
		getHostname(hostname);
		sprintf(response, "%s%s\n", OKmessage, hostname);
		send(new_sockfd, response, strlen(response), 0);
		printf("Message sent! (hostname)\n");
	}
	else if (!(strncmp("GET /cpu-name ", buffer, 14)))
	{
		getCPUname(cpuname);
		sprintf(response, "%s%s\n", OKmessage, cpuname);
		send(new_sockfd, response, strlen(response), 0);
		printf("Message sent! (cpu-name)\n");
	}
	else if (!(strncmp("GET /load ", buffer, 10)))
	{
		cpuload = getCPUload();
		sprintf(response, "%s%.2f%%\n", OKmessage, cpuload);
		send(new_sockfd, response, strlen(response), 0);
		printf("Message sent! (load)\n");
	}
	else
	{
		send(new_sockfd, BADmessage, strlen(BADmessage), 0);
		printf("400 BAD REQUEST sent!\n");
	}
	
	close(new_sockfd);
}
	return 0;
}