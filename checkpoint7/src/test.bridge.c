// Remember to compile with the "gcc -lpthread -m32" options

#include <common.h>
#include <stdlib.h>
#include <time.h>

// Usual stuff
#define INPUT_LEN 256
#define SCALE 100

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

#define abs(x) ((x)<0 ? -(x) : (x))

typedef struct CAR {
    int dir;
    int id;
    int loc;
} car_t;

// I use two separate condition var to avoid unnecessary waking
int goHanoverCondition;
int goNorwichCondition;
int lock;
int income;
int exitchildren;
//pthread_cond_t goHanoverCondition = PTHREAD_COND_INITIALIZER;
//pthread_cond_t goNorwichCondition = PTHREAD_COND_INITIALIZER;
//pthread_mutex_t lock =  PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;

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
    int rc = Acquire(lock);
    if(rc) {
        log_err("Lock Acquire fails.");
        Exit(-1);
    }

    // Printing
    char hanover[INPUT_LEN] = "";
    char norwich[INPUT_LEN] = "";
    char bridge[INPUT_LEN] = "";
    char item[INPUT_LEN];
    char id[INPUT_LEN];
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
                strcpy(item, "(");
                strcat(item, cars[i].dir == -1 ? "<-" : "");
                sprintf(id, "%d", cars[i].id);
                strcat(item, id);
                strcat(item, cars[i].dir == -1 ? "" : "->");
                strcat(item, ")");
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
        TtyPrintf(1, "               %d [%s] %d\n", nCnt, bridge, hCnt);
    } else {
        TtyPrintf(1, "[EMPTY BRIDGE] %d [%s] %d\n", nCnt, bridge, hCnt);
    } 


    // Checking stats
    if(car > maxCar) {
        log_err("#Cars on bridge %d overflow %d\n", car, maxCar);
    } 
    if(car < 0) {
        log_err("#Cars on bridge %d is negative\n", car);
    }
    rc = Release(lock);
    if(rc) {
        log_err("Lock Acquire fails.\n");
        Exit(-1);
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
       //  TtyPrintf(1, "%s%s\n", starve ? "[STARVE WARNING]" : "", !car ? "[EMPTY BRIDGE]" : "");
        
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

void ExitBridge(car_t* aCar) {
    // Lock the share resource
    int rc = Acquire(lock);
    if(rc) {
        log_err("Lock Acquire fails.\n");
        Exit(-1);
    }

    // Update condition and CvarSignal for anyone that might be CvarWaiting
    // This part is dvery delicate,
    // For the cars on the same side I use CvarSignal, for those on the other side I use CvarBroadcast
    car--;
    if(aCar->dir == TOHANOVER) {
        if(switchSide()) {
            rc = CvarBroadcast(goNorwichCondition);
        } else {
            rc = CvarSignal(goHanoverCondition);
        }
    } else if (aCar->dir == -1) { 
        if(switchSide()) {
            rc = CvarBroadcast(goHanoverCondition);
        } else {
            rc = CvarSignal(goNorwichCondition);
        }
    }
    if(rc) {
        TtyPrintf(1, "Condition CvarBroadcast by %d fails\n", aCar->id);
        Exit(-1);
    }
    
    // Reset car state
    aCar->loc = -1;
    aCar->dir = 0;
    aCar->id = -1;

    // Release the lock
    rc = Release(lock);
    if(rc) {
        log_err("Lock Acquire fails.\n");
        Exit(-1);
    }
    printOutsideLock();
}

void onBridge(car_t* aCar) {
    Pause();
}

void arriveBridge(car_t* aCar) {
    int rc;

    // Lock the share resource
    rc = Acquire(lock);
    if(rc) {
        log_err("Lock Acquire fails.\n");
        Exit(-1);
    }

    // Wait if necessary
    while(!safeGo(aCar)) {
        if(aCar->dir == TOHANOVER) {
            rc = CvarWait(goHanoverCondition, lock);
        } else if (aCar->dir == TONORWICH) {
            rc = CvarWait(goNorwichCondition, lock);
        }
        if(rc) {
            TtyPrintf(1, "Condition CvarWait for %d fails\n", aCar->id);
            Exit(-1);
        }
    }

    // Do critical things
    dirCnt += aCar->dir;
    aCar->loc += aCar->dir;
    flow = aCar->dir;
    car++;
    
    // Release the lock
    rc = Release(lock);
    if(rc) {
        log_err("Lock Acquire fails.");
        Exit(-1);
    }
    
    printOutsideLock();
}

int oneVehicle(car_t* aCar) {
    int pid = GetPid();
    user_log("%d: Before arrive", pid);
    arriveBridge(aCar);
    user_log("%d: Done arrive", pid);
    onBridge(aCar);
    user_log("%d: Done on bridge", pid);
    ExitBridge(aCar);
    user_log("%d: Done exit bridge", pid);
    exitchildren++;
    Exit(0);
}

int main(int argc, char *argv[]) {
    int rc, i, j; 
    TtyPrintf(1, "brdige simulation program\n");
    if(argc != 5) {
        TtyPrintf(1, "brdige usage: ./bridge <#maxCar on the bridge> <car prob to Hanover (0~100)> <car prob to Norwich (0~100)> <starving threshold>\n");
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

    rc = LockInit(&lock);
    if(rc) {
        log_err("Cannot init lock");
        Exit(-1);
    }
    rc = CvarInit(&goHanoverCondition);
    if(rc) {
        log_err("Cannot init goHanoverCondition");
        Exit(-1);
    }
    rc = CvarInit(&goNorwichCondition);
    if(rc) {
        log_err("Cannot init goNorwichCondition");
        Exit(-1);
    }

    // Start simulation
    //srand(time(NULL));
    i = 0;
    j = 0;
    int ppid = GetPid(), pid;
    int stat;
    income = 0;
    user_log("Parent PID is %d", ppid);
    int swtich = 0;
    TtyPrintf(1, "n2h %d, h2n %d\n", n2h, h2n);
    while(1) {
        if(exitchildren > 5) {
            while(exitchildren > 1) {
                user_log("Try collect zombie.");
                Wait(&stat);
                exitchildren--;
            }  
        }
        int rnd1 = rand() % 100;
        int rnd2 = rand() % 100;
        //TtyPrintf(1, "rnd1 %d, rnd2 %d\n", rnd1, rnd2);
        if(rnd1 < n2h ) {
            pid = Custom1();
            if(pid == -1) {
                TtyPrintf(1, "Create thread for %d fails\n", i);
                TtyPrintf(1, "Too many threads are jaming at bridge waiting queue.\n");
                free(cars);
                log_info("%d: I am stopping", GetPid());
                while(1)
                    Pause();
            }
            cars[i % INPUT_LEN].loc = NORWICH;
            cars[i % INPUT_LEN].id = i;
            cars[i % INPUT_LEN].dir = TOHANOVER;
            if(pid == 0)
                oneVehicle(cars + (i % INPUT_LEN));
            i++;
        }
        if(rnd2 < h2n ) {
            pid = Custom1();
            if(pid == -1) {
                TtyPrintf(1, "Create thread for %d fails\n", i);
                TtyPrintf(1, "Too many threads are jaming at bridge waiting queue.\n");
                free(cars);
                while(1)
                    Pause();
            }
            cars[i % INPUT_LEN].loc = HANOVER;
            cars[i % INPUT_LEN].id = i;
            cars[i % INPUT_LEN].dir = TONORWICH;
            if(pid == 0)
                oneVehicle(cars + (i % INPUT_LEN));
            i++;
        }
        if(rand() % 100 < 30)
            Delay(1);
    }

    // Never reached
    return 0;
}
  
