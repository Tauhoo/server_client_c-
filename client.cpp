/*
** client.c -- a stream socket client demo
*/

#include <pthread.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to

#define MAXDATASIZE 100 // max number of bytes we can get at once

using namespace std;

float sumtime = 0;
pthread_mutex_t lock;

typedef struct thread_arg
{
    int argc;
    char **argv;
} thread_arg;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void *sendRequest(void *arg) //int argc, char *argv[])
{
    thread_arg *argument = (thread_arg *)arg;
    int argc = argument->argc;
    char **argv = argument->argv;

    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2)
    {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }
    clock_t t1, t2;
    t1 = clock();
    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);

    freeaddrinfo(servinfo); // all done with this structure

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }
    t2 = clock();
    float diff((float)t2 - (float)t1);
    float seconds = diff / CLOCKS_PER_SEC;

    pthread_mutex_lock(&lock);
    sumtime += seconds;
    pthread_mutex_unlock(&lock);

    buf[numbytes] = '\0';

    close(sockfd);
}

int main(int argc, char *argv[])
{

    cout << "respondtime" << endl;

    // Call the function, here sort()
    for (int o = 165; o <= 1000; o++)
    {
        pthread_t th[o];
        sumtime = 0;
        for (int i = 0; i < o; i++)
        {
            thread_arg *arg = new thread_arg();
            arg->argc = argc;
            arg->argv = argv;
            pthread_create(&th[i], NULL, sendRequest, arg);
        }
        for (int p = 0; p < o; o++)
        {
            pthread_join(th[p], NULL);
        }
        cout << sumtime / o << endl;
    }

    return 0;
}