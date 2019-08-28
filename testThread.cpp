#include <iostream>
#include <pthread.h>

using namespace std;

int i = 0;

void *Print(void *thread_id){
    for (int o = 0; o < 100; o++) i++;
}

int main(){
    pthread_t test1;
    pthread_t test2;

    pthread_create(&test1, NULL, Print,(void *) 0);
    pthread_create(&test2, NULL, Print,(void *) 0);

    pthread_join(test1, NULL);
    pthread_join(test2, NULL);

    cout << i << endl;

    pthread_exit(NULL);
    return 0;
}