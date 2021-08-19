#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <unistd.h>

struct timeRule {
        int st_hour, st_min;
        int end_hour, end_min;
};

struct tm* getTime() {
        auto cur = time(NULL);
        return localtime(&cur);
}

void killUsr(std::string usrName) {
        std::string cmd("pkill -KILL -u " + usrName);
        system(cmd.c_str());
}

void changePasswd(std::string usrName, std::string newPasswd) {
        std::string cmd("echo -e \"");
        cmd += newPasswd;
        cmd += "\\n";
        cmd += newPasswd;
        cmd += "\" | passwd ";
        cmd += usrName;
        
        auto curT = getTime();
        
        printf("[%d-%d-%d  %d:%d:%d]  Running: %s\n",
               curT->tm_year + 1900, curT->tm_mon + 1, curT->tm_mday,
               curT->tm_hour, curT->tm_min, curT->tm_sec,
               cmd.c_str());
               
        system(cmd.c_str());
}

std::vector<timeRule> getRestrictions(std::string fileName) {
        std::ifstream in(fileName);
        int cnt;
        in >> cnt;
        std::vector<timeRule> rules(cnt);
        timeRule tmp;
        
        for (int i = 0; i < cnt; i++) {
                in >> tmp.st_hour >> tmp.st_min
                   >> tmp.end_hour >> tmp.end_min;
                rules[i] = tmp;
        }
        
        in.close();
        
        return rules;
}

// and their passwords
std::vector<std::pair<std::string, std::string> > getRestrictedUsrs(std::string
                                                                    fileName) {
        std::ifstream in(fileName);
        int cnt;
        in >> cnt;
        std::vector<std::pair<std::string, std::string> > usrs(cnt);
        std::string tmpU, tmpP;
        
        for (int i = 0; i < cnt; i++) {
                in >> tmpU >> tmpP;
                usrs[i] = std::make_pair(tmpU, tmpP);
        }
        
        in.close();
        
        return usrs;
}

std::vector<std::string> getPasswd(std::string fileName, int usrCnt) {
        std::ifstream in(fileName);
        std::vector<std::string> passwds(usrCnt);
        std::string tmp;
        
        for (int i = 0; i < usrCnt; i++) {
                in >> tmp;
                passwds[i] = tmp;
                
                if (in.eof()) {
                        break;
                }
        }
        
        in.close();
        
        return passwds;
}

unsigned int seed = 0;

int getRand() {
        srand(seed + time(NULL));
        return seed = rand();
}

std::vector<std::string> genRandPasswd(int usrCnt) {
        std::vector<std::string> passwds(usrCnt);
        
        for (int i = 0; i < usrCnt; i++) {
                std::string passwd;
                passwd += 'A' + getRand() % 26;
                passwd += 'a' + getRand() % 26;
                passwd += 'A' + getRand() % 26;
                passwd += 'a' + getRand() % 26;
                passwd += '0' + getRand() % 10;
                passwd += '0' + getRand() % 10;
                passwd += '0' + getRand() % 10;
                passwd += '0' + getRand() % 10;
                passwds[i] = passwd;
        }
        
        return passwds;
}

void updatePasswd(std::string fileName, std::vector<std::string> passwds) {
        std::ofstream out(fileName);
        
        for (auto passwd : passwds) {
                out << passwd << std::endl;
        }
        
        out.close();
}

int main() {

        fclose(stdout);
        
        while (1) {
                stdout = fopen("/home/peisongxiao/.uai/log", "a");
                
                auto rules = getRestrictions("/home/peisongxiao/.uai/restrictions");
                
                bool restrict = false;
                auto curT = getTime();
                
                printf("[%d-%d-%d  %d:%d:%d]  Checking restrictions...\n",
                       curT->tm_year + 1900, curT->tm_mon + 1, curT->tm_mday,
                       curT->tm_hour, curT->tm_min, curT->tm_sec);
                       
                for (auto cur : rules) {
                        if (curT->tm_hour >= cur.st_hour && curT->tm_hour <= cur.end_hour &&
                            curT->tm_min >= cur.st_min && curT->tm_min <= cur.end_min) {
                                restrict = true;
                                break;
                        }
                }
                
                if (!restrict) {
                
                        auto usrs = getRestrictedUsrs("/home/peisongxiao/.uai/usrs");
                        auto passwds = getPasswd("/home/peisongxiao/.uai/passwd", usrs.size());
                        
                        printf("[%d-%d-%d  %d:%d:%d]  Not restricted  Users: %ld\n",
                               curT->tm_year + 1900, curT->tm_mon + 1, curT->tm_mday,
                               curT->tm_hour, curT->tm_min, curT->tm_sec,
                               usrs.size());
                               
                        bool change = false;
                        
                        for (size_t i = 0; i < usrs.size(); i++)
                                if (usrs[i].second.compare(passwds[i]) != 0) {
                                        change = true;
                                        break;
                                }
                                
                        if (change) {
                                for (size_t i = 0; i < usrs.size(); i++) {
                                        changePasswd(usrs[i].first, usrs[i].second);
                                        passwds[i] = usrs[i].second;
                                }
                                
                                printf("[%d-%d-%d  %d:%d:%d]  Changed passwords to old\n",
                                       curT->tm_year + 1900, curT->tm_mon + 1, curT->tm_mday,
                                       curT->tm_hour, curT->tm_min, curT->tm_sec);
                                       
                                updatePasswd("/home/peisongxiao/.uai/passwd", passwds);
                        }
                        
                        sleep(60);
                        
                        fclose(stdout);
                        
                        continue;
                }
                
                printf("[%d-%d-%d  %d:%d:%d]  Restricted\n",
                       curT->tm_year + 1900, curT->tm_mon + 1, curT->tm_mday,
                       curT->tm_hour, curT->tm_min, curT->tm_sec);
                       
                auto usrs = getRestrictedUsrs("/home/peisongxiao/.uai/usrs");
                
                for (auto usr : usrs) {
                        killUsr(usr.first);
                }
                
                printf("[%d-%d-%d  %d:%d:%d]  Killed all users\n",
                       curT->tm_year + 1900, curT->tm_mon + 1, curT->tm_mday,
                       curT->tm_hour, curT->tm_min, curT->tm_sec);
                       
                auto passwds = getPasswd("/home/peisongxiao/.uai/passwd", usrs.size());
                
                bool change = false;
                
                for (size_t i = 0; i < usrs.size(); i++)
                        if (usrs[i].second.compare(passwds[i]) == 0) {
                                change = true;
                                break;
                        }
                        
                if (!change) {
                        sleep(60);
                        
                        fclose(stdout);
                        
                        continue;
                }
                
                passwds.clear();
                passwds = genRandPasswd(usrs.size());
                
                for (size_t i = 0; i < usrs.size(); i++) {
                        changePasswd(usrs[i].first, passwds[i]);
                }
                
                updatePasswd("/home/peisongxiao/.uai/passwd", passwds);
                
                printf("[%d-%d-%d  %d:%d:%d]  Changed passwords of restricted\n",
                       curT->tm_year + 1900, curT->tm_mon + 1, curT->tm_mday,
                       curT->tm_hour, curT->tm_min, curT->tm_sec);
                       
                sleep(60);
                
                fclose(stdout);
        }
        
        return 0;
}
