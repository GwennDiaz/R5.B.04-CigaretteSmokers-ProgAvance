/**
 *
 * Ce fichier contient la version simple du Cigarette Smokers Problem avec solution pour empecher l'interblocage.
 *
 */

#include <iostream>
#include <thread>
#include <semaphore>
#include <mutex>

using namespace std;

const bool DEBUG = true;

counting_semaphore<> agentSem(1);
counting_semaphore<> tobacco(0), paper(0), match(0);
counting_semaphore<> tobaccoSem(0), paperSem(0), matchSem(0);
mutex mut;
bool isTobacco(false), isPaper(false), isMatch(false);

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
    agentSem.acquire();
    tobacco.release();
    match.release();
    if (DEBUG) cout << "L'agent distribue du TABAC et des ALLUMETTES" << endl;
}

//  Thread Agent (Producteur)

void agent() {
    while (true) {

        short action = rand() % 3;
        switch (action) {
        case 0:
            agentNoMatches();
            break;
        case 1:
            agentNoTobacco();
            break;
        case 2:
            agentNoPaper();
            break;
        }
    }
}


//  Acteurs version sans solution
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


//  Acteurs version avec solution
//  Thread Pushers (mediateurs)

void pusherMatches(){
    while (true) {
        match.acquire();
        mut.lock();
        if (isPaper) {
            isPaper = false;
            tobaccoSem.release();
            if (DEBUG) cout << "PusherA : \"Il y a des ALLUMETTES aussi, je donne tout a FumeurB.\"" << endl;
        } else if (isTobacco) {
            isTobacco = false;
            paperSem.release();
            if (DEBUG) cout << "PusherA : \"Il y a des ALLUMETTES aussi, je donne tout a FumeurC.\"" << endl;
        } else {
            isMatch = true;
            if (DEBUG) cout << "PusherA : \"Il y a des ALLUMETTES !\"" << endl;
        }
        mut.unlock();
    }
}

void pusherTobacco(){
    while (true) {
        tobacco.acquire();
        mut.lock();
        if (isPaper) {
            isPaper = false;
            matchSem.release();
            if (DEBUG) cout << "PusherB : \"Il y a du PAPIER aussi, je donne tout a FumeurA.\"" << endl;
        } else if (isMatch) {
            isMatch = false;
            paperSem.release();
            if (DEBUG) cout << "PusherB : \"Il y a du PAPIER aussi, je donne tout a FumeurC.\"" << endl;
        } else {
            isTobacco = true;
            if (DEBUG) cout << "PusherB : \"Il y a du PAPIER !\"" << endl;
        }
        mut.unlock();
    }
}

void pusherPaper(){
    while (true) {
        paper.acquire();
        mut.lock();
        if (isTobacco) {
            isTobacco = false;
            matchSem.release();
            if (DEBUG) cout << "PusherC : \"Il y a du TABAC aussi, je donne tout a FumeurA.\"" << endl;
        } else if (isMatch) {
            isMatch = false;
            tobaccoSem.release();
            if (DEBUG) cout << "PusherC : \"Il y a du TABAC aussi, je donne tout a FumeurB.\"" << endl;
        } else {
            isPaper = true;
            if (DEBUG) cout << "PusherC : \"Il y a du TABAC !\"" << endl;
        }
        mut.unlock();
    }
}

//  Thread Fumeurs (consommateurs)

void smoke() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void newSmokerMatches() {
    while (true) {
        matchSem.acquire();
        if (DEBUG) cout << "FumeurA a pris du PAPIER et du TABAC     et commence a fumer..." << endl;
        smoke();
        if (DEBUG) cout << "FumeurA a fini de fumer, il a toujours des ALLUMETTES..." << endl;
        agentSem.release();
    }
}

void newSmokerTobacco() {
    while (true) {
        tobaccoSem.acquire();
        if (DEBUG) cout << "FumeurB a pris du PAPIER et des ALUMETTES et commence a fumer..." << endl;
        smoke();
        if (DEBUG) cout << "FumeurB a fini de fumer, il a toujours du TABAC..." << endl;
        agentSem.release();
    }
}

void newSmokerPaper() {
    while (true) {
        paperSem.acquire();
        if (DEBUG) cout << "FumeurC a pris des ALLUMETTES et du TABAC et commence a fumer..." << endl;
        smoke();
        if (DEBUG) cout << "FumeurC a fini de fumer, il a toujours des PAPIER..." << endl;
        agentSem.release();
    }
}


//  Fonctions principales des différentes versions

int mainNoSolution()
{
    if (DEBUG) cout << "Fumeur A et C attendent du TABAC, Fumeur B attend du PAPIER." << endl;
    thread a(agent);
    thread s1(smokerMatches), s2(smokerTobacco), s3(smokerPaper);
    a.join();
    s1.join(); s2.join(); s3.join();
    return 0;
}

int mainSolution()
{
    if (DEBUG) cout << "Fumeur A a des ALLUMETTES, Fumeur B a du TABAC et Fumeur C a du PAPIER." << endl;
    thread a(agent);
    thread s1(newSmokerMatches), s2(newSmokerTobacco), s3(newSmokerPaper);
    thread p1(pusherMatches), p2(pusherTobacco), p3(pusherPaper);
    a.join();
    s1.join(); s2.join(); s3.join();
    p1.join(); p2.join(); p3.join();
    return 0;
}

int main () {
    cout << "Version avec (1) ou sans (2) solutions? (1 par défaut) : ";
    string input;
    cin >> input;
    if (input != "2") {
        mainSolution();
    } else {
        mainNoSolution();
    }
    return 0;
}