#ifndef COMMON_H
#define COMMON_H
#include <string>
using namespace std;


inline bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
inline string toLower(string s) {
    for (size_t i = 0; i < s.size(); i++)
        if (s[i] >= 'A' && s[i] <= 'Z') s[i] = s[i] + ('a' - 'A');
    return s;
}

const int INF = 1000000000;

struct Category {
    string name;
    int priority;       // higher = more urgent
};

struct Request {
    int id;
    string item;
    string category;
    int priority;
    int location;       // graph vertex number (1..N), 0 = depot
};

#endif