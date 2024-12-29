#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

void process_packet(unsigned char* buffer, int data_size);
void print_ip_header(unsigned char* buffer, int size);
void print_tcp_packet(unsigned char* buffer, int size);
void print_udp_packet(unsigned char* buffer, int size);

FILE* logs_file;
int raw_socket;
struct sockaddr_in src, dst;
int total=0, tcp=0, udp=0, others=0;

int main()
{
    int saddr_size, data_size;
    struct sockaddr saddr;
    struct in_addr in;
    unsigned char* buffer = (unsigned char*)malloc(65536); //2^16 - 1


    logs_file = fopen("logs.txt", "w");
    if(logs_file==NULL)
        return -1;

    raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(raw_socket < 0)
        return 1;

    while(1)
    {
        saddr_size = sizeof saddr;
        data_size = recvfrom(raw_socket, buffer, 65536, 0, &saddr,&saddr_size);
        if(data_size < 0)
            return 1;
        process_packet(buffer, data_size);
    }

    close(raw_socket);
    free(buffer);
    buffer = NULL;

    printf("Done\n");
    return 0;
}

void process_packet(unsigned char* buffer, int data_size)
{
    struct iphdr *iph = (struct iphdr*)buffer;

    ++total;
    switch(iph->protocol)
    {
        case 6:
            ++tcp;
//            print_tcp_packet(buffer, data_size);
            break;
        case 17:
            ++udp;
            break;
        default:
            ++others;
            break;
    }

    printf("TCP: %d\tUDP: %d\t Others: %d\t Total: %d\n", tcp, udp, others, total);

}

void print_ip_header(unsigned char* buffer, int size)
{
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr*)buffer;
    iphdrlen = iph->ihl*4;

    memset(&src, 0, sizeof(src));
    src.sin_addr.s_addr = iph->saddr;

    memset(&dst, 0, sizeof(dst));
    dst.sin_addr.s_addr = iph->saddr;

    fprintf(logs_file, "\n");
    fprintf(logs_file, "\t\t\t=== IP Header Information ===\n");
    fprintf(logs_file, "Version:\t%d\n", (unsigned int)iph->version);
    fprintf(logs_file, "Header Length:\t%d\n", (unsigned int)iphdrlen);
    fprintf(logs_file, "Time to Live:\t%d\n", (unsigned int)iph->ttl);
    fprintf(logs_file, "Protocol:\t%d\n", (unsigned int)iph->protocol);
    fprintf(logs_file, "Source Address:\t%d\n", (unsigned int)iph->saddr);
    fprintf(logs_file, "Destination Address:\t%d\n", (unsigned int)iph->daddr);
}

void print_tcp_packet(unsigned char* buffer, int size)
{

}

void print_udp_packet(unsigned char* buffer, int size)
{

}