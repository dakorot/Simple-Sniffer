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
    struct iphdr *ip_header = (struct iphdr*)buffer;

    ++total;
    switch(ip_header->protocol)
    {
        case 6:
            ++tcp;
            print_tcp_packet(buffer, data_size);
            break;
        case 17:
            ++udp;
            print_udp_packet(buffer, data_size);
            break;
        default:
            ++others;
            break;
    }

    printf("TCP: %d\tUDP: %d\tOthers: %d\tTotal: %d\n", tcp, udp, others, total);

}

void print_ip_header(unsigned char* buffer, int size)
{
    unsigned short ip_header_len;
    struct iphdr *ip_header = (struct iphdr*)buffer;
    ip_header_len = ip_header->ihl*4;

    memset(&src, 0, sizeof(src));
    src.sin_addr.s_addr = ip_header->saddr;

    memset(&dst, 0, sizeof(dst));
    dst.sin_addr.s_addr = ip_header->saddr;

    fprintf(logs_file, "\n");
    fprintf(logs_file, "\t\t\t=== IP Header Information ===\n");
    fprintf(logs_file, "Version:\t%u\n", (unsigned int)ip_header->version);
    fprintf(logs_file, "Header Length:\t%u\n", (unsigned int)ip_header_len);
    fprintf(logs_file, "Time to Live:\t%u\n", (unsigned int)ip_header->ttl);
    fprintf(logs_file, "Protocol:\t%u\n", (unsigned int)ip_header->protocol);
    fprintf(logs_file, "Source Address:\t%u\n", (unsigned int)ip_header->saddr);
    fprintf(logs_file, "Destination Address:\t%u\n", (unsigned int)ip_header->daddr);
}

void print_tcp_packet(unsigned char* buffer, int size)
{
    unsigned short int ip_header_len;
    struct iphdr* ip_header= (struct iphdr*)buffer;
    ip_header_len = ip_header->ihl*4;

    struct tcphdr* tcp_header= (struct tcphdr*)(buffer + ip_header_len);
    fprintf(logs_file, "\n");
    fprintf(logs_file, "\t\t\t=== TCP Header Information ===\n");
    fprintf(logs_file, "Source Port:\t%u\n", (unsigned int)tcp_header->th_sport);
    fprintf(logs_file, "Destination Port:\t%u\n", (unsigned int)tcp_header->th_dport);
    fprintf(logs_file, "Sequence Number:\t%u\n", (unsigned int)tcp_header->th_seq);
    fprintf(logs_file, "Acknowledgement Number:\t%u\n", (unsigned int)tcp_header->th_ack);
    fprintf(logs_file, "Header Length:\t%u\n", ((unsigned int)tcp_header->th_off*4));
}

void print_udp_packet(unsigned char* buffer, int size)
{
    unsigned short int ip_header_len;
    struct iphdr* ip_header= (struct iphdr*)buffer;
    ip_header_len = ip_header->ihl*4;

    struct udphdr* udp_header= (struct udphdr*)(buffer + ip_header_len);
    fprintf(logs_file, "\n");
    fprintf(logs_file, "\t\t\t=== UDP Header Information ===\n");
    fprintf(logs_file, "Source Port:\t%u\n", (unsigned int)udp_header->uh_sport);
    fprintf(logs_file, "Destination Port:\t%u\n", (unsigned int)udp_header->uh_dport);
    fprintf(logs_file, "Header Length:\t%u\n", ((unsigned int)udp_header->uh_ulen));
}