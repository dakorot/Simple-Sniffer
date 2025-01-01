#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

void process_packet(unsigned char* buffer, int data_size);
void print_ethernet_header(unsigned char* buffer, int data_size);
void print_ip_header(unsigned char* buffer, int data_size);
void print_tcp_packet(unsigned char* buffer, int data_size);
void print_udp_packet(unsigned char* buffer, int data_size);

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
        return 1;

    raw_socket = socket(AF_PACKET, SOCK_RAW,htons(ETH_P_ALL));
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
    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));

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

void print_ethernet_header(unsigned char* buffer, int data_size)
{
    struct ethhdr* eth_header = (struct ethhdr*)buffer;

    fprintf(logs_file, "\n");
    fprintf(logs_file, "\t\t* Ethernet Header Information *\n");
    fprintf(logs_file, "Destination Address:\t%.2X %.2X %.2X %.2X %.2X %.2X\n", eth_header->h_dest[0], eth_header->h_dest[1], eth_header->h_dest[2], eth_header->h_dest[3], eth_header->h_dest[4], eth_header->h_dest[5]);
    fprintf(logs_file, "Source Address:\t\t\t%.2X %.2X %.2X %.2X %.2X %.2X\n", eth_header->h_source[0], eth_header->h_source[1], eth_header->h_source[2], eth_header->h_source[3], eth_header->h_source[4], eth_header->h_source[5]);
    fprintf(logs_file, "Protocol:\t\t\t\t%u\n", (unsigned short int)eth_header->h_proto);
}

void print_ip_header(unsigned char* buffer, int data_size)
{
    print_ethernet_header(buffer, data_size);

    unsigned short ip_header_len;
    struct iphdr *ip_header = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    ip_header_len = ip_header->ihl*4;

    memset(&src, 0, sizeof(src));
    src.sin_addr.s_addr = ip_header->saddr;

    memset(&dst, 0, sizeof(dst));
    dst.sin_addr.s_addr = ip_header->saddr;

    fprintf(logs_file, "\n");
    fprintf(logs_file, "\t\t* IP Header Information *\n");
    fprintf(logs_file, "Version:\t\t\t\t%u\n", (unsigned int)ip_header->version);
    fprintf(logs_file, "Header Length:\t\t\t%u\n", (unsigned int)ip_header_len);
    fprintf(logs_file, "Time to Live:\t\t\t%u\n", (unsigned int)ip_header->ttl);
    fprintf(logs_file, "Protocol:\t\t\t\t%u\n", (unsigned int)ip_header->protocol);
    fprintf(logs_file, "Source Address:\t\t\t%u\n", (unsigned int)ip_header->saddr);
    fprintf(logs_file, "Destination Address:\t%u\n", (unsigned int)ip_header->daddr);
}

void print_tcp_packet(unsigned char* buffer, int data_size)
{
    unsigned short int ip_header_len;
    struct iphdr* ip_header= (struct iphdr*)(buffer + sizeof(struct ethhdr));
    ip_header_len = ip_header->ihl*4;

    struct tcphdr* tcp_header= (struct tcphdr*)(buffer + sizeof(struct ethhdr) + ip_header_len);
    fprintf(logs_file, "\n");
    fprintf(logs_file, "\t\t\t=== TCP Header Information ===\n");
    print_ip_header(buffer, data_size);
    fprintf(logs_file, "\n");
    fprintf(logs_file, "Source Port:\t\t\t%u\n", ntohs(tcp_header->th_sport));
    fprintf(logs_file, "Destination Port:\t\t%u\n", ntohs(tcp_header->th_dport));
    fprintf(logs_file, "Sequence Number:\t\t%u\n", ntohl(tcp_header->th_seq));
    fprintf(logs_file, "Acknowledgement Number:\t%u\n", ntohl(tcp_header->th_ack));
    fprintf(logs_file, "Header Length:\t\t\t%u\n", (unsigned int)tcp_header->th_off*4);
    fprintf(logs_file, "\n\t**********************************************\n\n");
}

void print_udp_packet(unsigned char* buffer, int data_size)
{
    unsigned short int ip_header_len;
    struct iphdr* ip_header= (struct iphdr*)(buffer + sizeof(struct ethhdr));
    ip_header_len = ip_header->ihl*4;

    struct udphdr* udp_header= (struct udphdr*)(buffer + sizeof(struct ethhdr) + ip_header_len);
    fprintf(logs_file, "\n");
    fprintf(logs_file, "\t\t\t=== UDP Header Information ===\n");
    print_ip_header(buffer, data_size);
    fprintf(logs_file, "\n");
    fprintf(logs_file, "Source Port:\t\t\t%u\n", ntohs(udp_header->uh_sport));
    fprintf(logs_file, "Destination Port:\t\t%u\n", ntohs(udp_header->uh_dport));
    fprintf(logs_file, "Header Length:\t\t\t%u\n", (unsigned int)udp_header->uh_ulen);
    fprintf(logs_file, "\n\t**********************************************\n\n");
}