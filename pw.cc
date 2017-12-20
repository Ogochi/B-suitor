#include <thread>
#include <iostream>
#include <atomic>
#include <fstream>
#include <map>
#include <set>
#include <queue>
#include <vector>
#include <string>
#include <chrono>
#define MAX 1000000

using std::cout;
using std::map;
using std::set;
using std::queue;
using std::pair;
using std::vector;

unsigned int bvalue(unsigned int method, unsigned long node_id) {
    switch (method) {
    case 0: return 1;
    default: switch (node_id) {
        case 0: return 2;
        case 1: return 2;
        default: return 1;
        }
    }
}

queue<int> *Q = new queue<int>(), *R = new queue<int>();
vector<set<pair<int, int>>> N; // set<waga - nr sasiada>
vector<int> mapping; // new node nr -> node nr
set<pair<int, int>> *S; // set<waga - nr sasiada>
set<int> *T; // nr sasiada
//set<pair<int, int>>::reverse_iterator lastProcessed[MAX];

void readGraphAndPrepare(char* fileName) {
  map<int, int> helper; // node nr - new node nr
  int newNodeNr = 0;
  std::ifstream infile(fileName);

  while (infile.peek() == '#')
    infile.ignore(std::numeric_limits<std::streamsize>::max(), infile.widen('\n'));

  int from, to, w;
  while (infile >> from >> to >> w) {
    auto itFrom = helper.find(from), itTo = helper.find(to);
    if (itFrom == helper.end()) {
      N.push_back(set<pair<int, int>>());
      mapping.push_back(from);
      itFrom = helper.insert({from, newNodeNr++}).first;
    }
    if (itTo == helper.end()) {
      N.push_back(set<pair<int, int>>());
      mapping.push_back(to);
      itTo = helper.insert({to, newNodeNr++}).first;
    }

    N[itFrom->second].insert({w, itTo->second});
    N[itTo->second].insert({w, itFrom->second});
  }
}

inline int sLast(int x, int method) {
  if (bvalue(method, mapping[x]) == S[x].size())
    return S[x].begin()->second;
  else
    return -1;
}

inline int wSLast(int x, int method) {
  if (bvalue(method, mapping[x]) == S[x].size())
    return S[x].begin()->first;
  else
    return -1;
}

auto findMax(int curr, int method) {
  auto i = N[curr].rbegin();
  while (i != N[curr].rend()) {
    if (T[curr].find(i->second) == T[curr].end())
      if (i->first > wSLast(i->second, method) ||
          (wSLast(i->second, method) == i->first && mapping[i->second] > mapping[sLast(i->second, method)]))
        return i;
    i++;
  }
  return N[curr].rend();
}

int sum() {
  int sum = 0;
  for (int i = 0; i <= 5; i++) {
    //cout << i << ":\n\tS: ";
    for (auto j : S[i]) {
      //cout << "(" << j.first << ", " << j.second << ")";
      sum += j.first;
    }/*
    cout << "\n\tT: ";
    for (auto j : T[i]) {
      cout << j << " ";
    }
    cout << "\n";*/
  }
  return sum;
}

int main(int argc, char** argv) {
  auto t1 = std::chrono::high_resolution_clock::now();
  //std::ios_base::sync_with_stdio(0); TODO
  int blimit = atoi(argv[3]); // bedziemy iterowac
  readGraphAndPrepare(argv[2]);
  bool firstRound;
  for (int method = 0; method <= blimit; method++) {
    S = new set<pair<int, int>>[N.size()];
    T = new set<int>[N.size()];
    firstRound = true;
    Q->push(0);

    while (!Q->empty()) {
      if (firstRound) {
        for (unsigned int curr = 0; curr < N.size(); curr++) {
          //cout << "Tera: " << curr << "\n";
          while (T[curr].size() < bvalue(method, mapping[curr])) {
            auto x = findMax(curr, method);
            if (x == N[curr].rend())
              break;
            //cout << "\tZnalazlem: " << x->second << "\n";

            int y = sLast(x->second, method);
            S[x->second].insert({x->first, curr});
            T[curr].insert(x->second);

            if (y != -1) {
              S[x->second].erase(S[x->second].begin());
              T[y].erase(x->second);
              //cout << "alert!";
              R->push(y);
            }
          }
        }
        firstRound = false;
      } else {
        while (!Q->empty()) {
          int curr = Q->front();
          //cout << "Tera: " << curr << "\n";
          Q->pop();

          while (T[curr].size() < bvalue(method, mapping[curr])) {
            auto x = findMax(curr, method);
            if (x == N[curr].rend())
              break;
            //cout << "\tZnalazlem: " << x->second << "\n";

            int y = sLast(x->second, method);
            S[x->second].insert({x->first, curr});
            T[curr].insert(x->second);

            if (y != -1) {
              S[x->second].erase(S[x->second].begin());
              T[y].erase(x->second);
              //cout << "alert!";
              R->push(y);
            }
          }
        }
      }

      delete Q;
      Q = R;
      R = new queue<int>();
    }
    cout << sum() / 2 << "\n";
    delete [] S;
    delete [] T;
  }

  cout << "time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::high_resolution_clock::now() - t1).count() / (double)1000000000 << "\n";
}
