#include <iostream>
#include <thread>
#include <semaphore>

using namespace std;

const bool DEBUG = true;

counting_semaphore<> agentSem(1);
counting_semaphore<> tobacco(0), paper(0), match(0);

//  Actions possible de l'agent après reveille

void agentNoMatches() {
    agentSem.acquire();
    tobacco.release();
    paper.release();
    if (DEBUG) cout << "L'agent distribue du TABAC et du PAPIER" << endl;
}

void agentNoTobacco() {
    agentSem.acquire();
    paper.release();
    match.release();
    if (DEBUG) cout << "L'agent distribue du PAPIER et des ALLUMETTES" << endl;
}

void agentNoPaper() {
    while (true) {
        agentSem.acquire();
        tobacco.release();
        match.release();
        if (DEBUG) cout << "L'agent distribue du TABAC et des ALLUMETTES" << endl;
    }
}

//  Thread Agent (Producteur)

void agent() {
    while (true) {
        short action = rand() % 3;
        switch (action) {
        case 0:
            agentNoMatches();
        case 1:
            agentNoTobacco();
        case 2:
            agentNoPaper();
        }
        agentSem.release();
    }
}

//  Thread Fumeurs (consommateurs)

void smokerMatches() {
    while (true) {
        tobacco.acquire();
        if (DEBUG) cout << "FumeurA à pris du TABAC , plus qu'un  PAPIER   pour finir sa cigarette..." << endl;
        paper.acquire();
        if (DEBUG) cout << "FumeurA a eu son PAPIER, il fabrique une cigarette et attendra du TABAC.." << endl;
        agentSem.release();
    }
}

void smokerTobacco() {
    while (true) {
        paper.acquire();
        if (DEBUG) cout << "FumeurB à pris du PAPIER, plus qu'une ALUMETTE pour finir sa cigarette..." << endl;
        match.acquire();
        if (DEBUG) cout << "FumeurB a eu son ALLUMETTE, il fabrique une cigarette et attendra du TABAC.." << endl;
        agentSem.release();
    }
}

void smokerPaper() {
    while (true) {
        tobacco.acquire();
        if (DEBUG) cout << "FumeurC à pris du TABAC , plus qu'une ALUMETTE pour finir sa cigarette..." << endl;
        match.acquire();
        if (DEBUG) cout << "FumeurC a eu son ALLUMETTE, il fabrique une cigarette et attendra du TABAC." << endl;
        agentSem.release();
    }
}

int main()
{
    if (DEBUG) cout << "Fumeur A et C attendent du TABAC, Fumeur B attend du PAPIER." << endl;
    thread a(agent);
    thread s1(smokerMatches), s2(smokerTobacco), s3(smokerPaper);
    a.join();
    s1.join(); s2.join(); s3.join();
    return 0;
}