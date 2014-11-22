// example demo code for locks
// S.W. Smith, CS58, Dartmouth College


// Remember to compile with the "gcc -lpthread -m32" options


#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>   // for threads
#include <unistd.h>
#include <errno.h>

// Usual stuff
#define INPUT_LEN 1024
#define SCALE 100000

// Locations
#define NORWICH 0
#define BRIDGE 1
#define HANOVER 2

// Directions
#define TOHANOVER 1
#define TONORWICH -1
#define TOEITHER 0

#  define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP \
  { { 0, 0, 0, PTHREAD_MUTEX_ERRORCHECK_NP, 0, { 0 } } }

typedef struct CAR {
    int dir;
    int id;
    int loc;
} car_t;

// I use two separate condition var to avoid unnecessary waking
pthread_cond_t goHanoverCondition = PTHREAD_COND_INITIALIZER;
pthread_cond_t goNorwichCondition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock =  PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;

// pthread wrapping
int acquire(pthread_mutex_t* lock) {
    return pthread_mutex_lock(lock);
}
int release(pthread_mutex_t* lock) {
    return pthread_mutex_unlock(lock);
}
int start(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg) {
    return  pthread_create(thread, attr, start_routine, arg);
}
int broadcast(pthread_cond_t* cVar) {
    return pthread_cond_broadcast(cVar);
}
int signal(pthread_cond_t* cVar) {
    return pthread_cond_signal(cVar);
}
int wait(pthread_cond_t* cVar, pthread_mutex_t* lock) {
    return pthread_cond_wait(cVar, lock);
}

// Bridge states
int maxCar, car = 0;                                // Max load and curent load
int n2h, h2n;                                       // The prob on each side
int hCnt, nCnt;                                     // Queue length on each side
int flow = TOEITHER;                                // Flow direction on the bridge
int cross = 0;                                      // Time scale to cross 
int dirCnt = 0, starvingThreashold, forbidDir = 0;  // Starving control
car_t* cars;

void padding(char* pad, int n) {
    pad[0] = '\0';
    while(n--) {
        if(strlen(pad) == 0) {
            strcpy(pad, " ");
        } else {
            strcat(pad, " ");
        }
    }
}

void printOutsideLock() {
    int totalLen = maxCar * 8;
    int rc = acquire(&lock);
    if(rc) {
        fprintf(stderr, "Lock acquire fails.\n");
        exit(-1);
    }

    // Printing
    char hanover[INPUT_LEN] = "";
    char norwich[INPUT_LEN] = "";
    char bridge[INPUT_LEN] = "";
    char item[INPUT_LEN];
    bzero(hanover, INPUT_LEN);
    bzero(norwich, INPUT_LEN);
    bzero(bridge, INPUT_LEN);
    bzero(item, INPUT_LEN);

    int i, bCnt = 0;
    hCnt = 0;
    nCnt = 0;
    bzero(bridge, INPUT_LEN);
    for(i = 0; i < INPUT_LEN; i++) {
        switch(cars[i].loc) {
            case NORWICH:
                nCnt++;
                break;
            case HANOVER:
                hCnt++;
                break;
            case BRIDGE:
                bCnt++;
                sprintf(item, "(%s%d%s)",  cars[i].dir == -1 ? "<-" : "", cars[i].id, cars[i].dir == -1 ? "" : "->");
                strcat(bridge, item);
                break;
            default:
                break;
        }
    }
    char pad[INPUT_LEN];
    padding(pad, totalLen - strlen(bridge));
    if(flow == TONORWICH) {
        strcat(pad, bridge);
        strcpy(bridge, pad);
    } else {
        strcat(bridge, pad);
    }
    strcat(norwich, "");
    strcat(hanover, "");
    strcat(bridge, "");
    if(bCnt) {
        printf("               %d [%s] %d\n", nCnt, bridge, hCnt);
    } else {
        printf("[EMPTY BRIDGE] %d [%s] %d\n", nCnt, bridge, hCnt);
    } 


    // Checking stats
    if(car > maxCar) {
        fprintf(stderr, "#Cars on bridge %d overflow %d\n", car, maxCar);
    } 
    if(car < 0) {
        fprintf(stderr, "#Cars on bridge %d is negative\n", car);
    }
    rc = release(&lock);
    if(rc) {
        fprintf(stderr, "Lock acquire fails.\n");
        exit(-1);
    }
}

int safeGo(car_t* aCar) {
    int dir = aCar->dir;
    // Read and perceive the locked resource
    if(forbidDir == dir) {
        return 0;
    }
    if(car == 0) {
        return 1;
    } else if(car < maxCar && flow == dir) {
        return 1;
    }
    return 0;
}

int switchSide() {
    // Switch when starve or bridge went empty
    int starve = starvingThreashold > 0 && abs(dirCnt) > starvingThreashold;
    if(starve || !car) {
        forbidDir = flow;
       //  printf("%s%s\n", starve ? "[STARVE WARNING]" : "", !car ? "[EMPTY BRIDGE]" : "");
        
        // If no car on the other side, don't take side
        if((forbidDir == TOHANOVER && !hCnt) || (forbidDir == TONORWICH && !nCnt)) {
            forbidDir = 0;
            return 0;
        } else {
            dirCnt = 0;
            return 1;
        }
    }
    return 0;
}

void exitBridge(car_t* aCar) {
    // Lock the share resource
    int rc = acquire(&lock);
    if(rc) {
        fprintf(stderr, "Lock acquire fails.\n");
        exit(-1);
    }

    // Update condition and signal for anyone that might be waiting
    // This part is dvery delicate,
    // For the cars on the same side I use signal, for those on the other side I use broadcast
    car--;
    if(aCar->dir == TOHANOVER) {
        if(switchSide()) {
            rc = broadcast(&goNorwichCondition);
        } else {
            rc = signal(&goHanoverCondition);
        }
    } else if (aCar->dir == -1) { 
        if(switchSide()) {
            rc = broadcast(&goHanoverCondition);
        } else {
            rc = signal(&goNorwichCondition);
        }
    }
    if(rc) {
        printf("Condition broadcast by %d fails\n", aCar->id);
        exit(-1);
    }
    
    // Reset car state
    aCar->loc = -1;
    aCar->dir = 0;
    aCar->id = -1;

    // Release the lock
    rc = release(&lock);
    if(rc) {
        fprintf(stderr, "Lock acquire fails.\n");
        exit(-1);
    }
    printOutsideLock();
}

void onBridge(car_t* aCar) {
    usleep(SCALE * cross);
}

void arriveBridge(car_t* aCar) {
    int rc;

    // Lock the share resource
    rc = acquire(&lock);
    if(rc) {
        fprintf(stderr, "Lock acquire fails.\n");
        exit(-1);
    }

    // Wait if necessary
    while(!safeGo(aCar)) {
        if(aCar->dir == TOHANOVER) {
            rc = wait(&goHanoverCondition, &lock);
        } else if (aCar->dir == TONORWICH) {
            rc = wait(&goNorwichCondition, &lock);
        }
        if(rc) {
            printf("Condition wait for %d fails: %s\n", aCar->id, strerror(errno));
            exit(-1);
        }
    }

    // Do critical things
    dirCnt += aCar->dir;
    aCar->loc += aCar->dir;
    flow = aCar->dir;
    car++;
    
    // Release the lock
    rc = release(&lock);
    if(rc) {
        fprintf(stderr, "Lock acquire fails.\n");
        exit(-1);
    }
    
    printOutsideLock();
}

void* oneVehicle(car_t* aCar) {
    arriveBridge(aCar);
    onBridge(aCar);
    exitBridge(aCar);
}


int main(int argc, char *argv[]) {
    int rc, i, j; 
    if(argc != 5) {
        printf("brdige usage: ./bridge <#maxCar on the bridge> <car prob to Hanover (0~100)> <car prob to Norwich (0~100)> <starving threshold>\n");
        return;
    } else {
        maxCar = atoi(argv[1]);
        n2h = atoi(argv[2]);
        cross = 2;
        h2n = atoi(argv[3]);
        starvingThreashold = atoi(argv[4]) > 0 ? atoi(argv[4]) : maxCar;
    }
    cars = (car_t*) malloc(sizeof(car_t) * INPUT_LEN);
    for(i = 0; i < INPUT_LEN; i++) {
        cars[i].loc = -1;
        cars[i].dir = 0;
        cars[i].id = -1;
    }

    // Start simulation
    srand(time(NULL));
    i = 0;
    j = 0;
    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    rc = pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED); // Set detach attribute
    if(rc) {
        printf("Setting detach state fails\n");
    }
    while(1) {
        pthread_t thread;
        if((rand() % 100) < n2h ) {
            cars[i % INPUT_LEN].loc = NORWICH;
            cars[i % INPUT_LEN].id = i;
            cars[i % INPUT_LEN].dir = TOHANOVER;
            rc = start(&thread, &tattr, oneVehicle, cars + (i % INPUT_LEN));
            if(rc) {
                printf("Create thread for %d fails: %s\n", i, strerror(errno));
                printf("Too many threads are jaming at waiting queue.\n");
                free(cars);
                exit(-1);
            }
            i++;
        }
        if((rand() % 100) < h2n ) {
            cars[i % INPUT_LEN].loc = HANOVER;
            cars[i % INPUT_LEN].id = i;
            cars[i % INPUT_LEN].dir = TONORWICH;
            rc = start(&thread, &tattr, oneVehicle, cars + (i % INPUT_LEN));
            if(rc) {
                printf("Create thread for %d fails: %s\n", i, strerror(errno));
                printf("Too many threads are jaming at waiting queue.\n");
                free(cars);
                exit(-1);
            }
            i++;
        }
        usleep(SCALE / 6);
    }

    // Never reached
    return 0;
}
  
