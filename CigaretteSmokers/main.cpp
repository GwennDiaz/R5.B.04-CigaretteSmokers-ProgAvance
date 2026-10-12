#include <iostream>
#include <thread>
#include <semaphore>

using namespace std;

counting_semaphore<> agentSem(1);
counting_semaphore<> tobacco(0), paper(0), match(0);

void agentA() {
    while (true) {
        agentSem.acquire();
        tobacco.release();
        paper.release();
    }
}

void agentB() {
    while (true) {
        agentSem.acquire();
        paper.release();
        match.release();
    }
}

void agentC() {
    while (true) {
        agentSem.acquire();
        tobacco.release();
        match.release();
    }
}

void smokerMatches() {
    while (true) {
        tobacco.acquire();
        paper.acquire();
        agentSem.release();
    }
}

void smokerTobacco() {
    while (true) {
        paper.acquire();
        match.acquire();
        agentSem.release();
    }
}

void smokerPaper() {
    while (true) {
        tobacco.acquire();
        match.acquire();
        agentSem.release();
    }
}

int main()
{
    thread a(agentA), b(agentB), c(agentC);
    thread s1(smokerMatches), s2(smokerTobacco), s3(smokerPaper);
    a.join(); b.join(); c.join();
    s1.join(); s2.join(); s3.join();

    return 0;
}
