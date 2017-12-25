#include <thread>
#include <iostream>
#include <atomic>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>

using std::cout;
using std::map;
using std::set;
using std::pair;
using std::vector;
using std::atomic;

vector<int> *Q = new vector<int>(), *R = new vector<int>();
vector<int> mapping; // new node nr -> node nr
struct setComp {
  bool operator() (const pair<int, int>& a, const pair<int, int>& b) {
    if (a.first == b.first)
      return mapping[a.second] < mapping[b.second];
    else
      return a.first < b.first;
  }
};
vector<set<pair<int, int>, setComp>> N; // set<waga - nr sasiada>
set<pair<int, int>, setComp> *S; // set<waga - nr sasiada>
set<pair<int, int>>::reverse_iterator *lastProcessed;

void readGraphAndPrepare(char* fileName) {
  map<int, int> helper; // node nr - new node nr
  int newNodeNr = 0;
  std::ifstream infile(fileName);

  while(infile.peek() == '#')
    infile.ignore(std::numeric_limits<std::streamsize>::max(), infile.widen('\n'));

  int from, to, w;
  while (infile >> from >> to >> w) {
    auto itFrom = helper.find(from), itTo = helper.find(to);
    if (itFrom == helper.end()) {
      N.push_back(set<pair<int, int>, setComp>());
      mapping.push_back(from);
      itFrom = helper.insert({from, newNodeNr++}).first;
    }
    if (itTo == helper.end()) {
      N.push_back(set<pair<int, int>, setComp>());
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
  auto i = lastProcessed[curr];
  while (i != N[curr].rend()) {
    if (S[i->second].find({i->first, curr}) == S[i->second].end() && bvalue(method, mapping[i->second]) != 0) // TODO usunac != 0
      if (i->first > wSLast(i->second, method) ||
          (wSLast(i->second, method) == i->first && mapping[curr] > mapping[sLast(i->second, method)])) {
        lastProcessed[curr] = ++i;
        return --i;
      }
    i++;
  }
  lastProcessed[curr] = N[curr].rend();
  return N[curr].rend();
}

int sum() {
  int sum = 0;
  for (unsigned int i = 0; i < N.size(); i++)
    for (auto j : S[i])
      sum += j.first;
  return sum;
}

atomic<unsigned int> *T; // ilosc sasiadow
atomic<bool> lockR;
atomic<bool> *spinLock;
atomic<int> nodesQueue;
bool *inR;

void processNode(int method, bool isFirstRound) {
  while (true) {
    unsigned int curr;
    bool canProcess = false;

    if (isFirstRound) {
      curr = nodesQueue.fetch_add(1);
      if (curr < N.size()) {
        canProcess = true;
      }
    } else {
      curr = nodesQueue.fetch_add(1);
      if (curr < Q->size()) {
        curr = (*Q)[curr];
        canProcess = true;
      }
    }

    if (!canProcess)
      break;

    while (T[curr] < bvalue(method, mapping[curr])) {
      auto x = findMax(curr, method);
      if (x == N[curr].rend())
        break;

      bool expected;
      do {
        expected = true;
        spinLock[x->second].compare_exchange_weak(expected, false);
      } while (expected == false);

      int y = sLast(x->second, method);
      // is still eligible?
      if (x->first > wSLast(x->second, method) ||
          (wSLast(x->second, method) == x->first && mapping[curr] > mapping[y])) {
        T[curr]++;
        if (y != -1)
          T[y]--;
        S[x->second].insert({x->first, curr});
        if (y != -1)
          S[x->second].erase(S[x->second].begin());
        spinLock[x->second] = true;


        if (y != -1) {
          do {
            expected = true;
            lockR.compare_exchange_weak(expected, false);
          } while (expected == false);
          if (!inR[y]) {
            R->push_back(y);
            inR[y] = true;
          }
          lockR = true;
        }
      }
        else
      spinLock[x->second] = true;
     }
  }
}

int main(int argc, char** argv) {
  auto t1 = std::chrono::high_resolution_clock::now();
  std::ios_base::sync_with_stdio(0);

  int blimit = atoi(argv[3]);
  int threadsLimit = atoi(argv[1]);
  readGraphAndPrepare(argv[2]);

  lockR = true;
  lastProcessed = new set<pair<int, int>>::reverse_iterator[N.size()];
  T = new atomic<unsigned int>[N.size()];
  spinLock = new atomic<bool>[N.size()];
  for (unsigned int i = 0; i < N.size(); i++)
    spinLock[i] = true;

  for (int method = 0; method <= blimit; method++) {
    S = new set<pair<int, int>, setComp>[N.size()];
    for (unsigned int i = 0; i < N.size(); i++)
      T[i] = 0;
    for (unsigned int i = 0; i < N.size(); i++)
      lastProcessed[i] = N[i].rbegin();
    Q->push_back(-1);
    bool firstRound = true;

    while (!Q->empty()) {
      inR = new bool[N.size()]{0};
      int howManyThreadsToMake;
      nodesQueue = 0;

      if (firstRound)
        howManyThreadsToMake = std::min((int)N.size() - 1, threadsLimit - 1);
      else
        howManyThreadsToMake = std::min((int)Q->size(), threadsLimit - 1);

      std::thread threads[howManyThreadsToMake];
      for (int i = 0; i < howManyThreadsToMake; i++)
        threads[i] = std::thread{ [method, firstRound]{processNode(method, firstRound); }};

      processNode(method, firstRound);
      firstRound = false;

      for (int i = 0; i < howManyThreadsToMake; i++)
        threads[i].join();

      delete Q;
      Q = R;
      R = new vector<int>();
    }
    cout << sum() / 2 << "\n";
    delete [] inR;
    delete [] S;
  }
  delete [] T;
  delete [] spinLock;
  delete [] lastProcessed;
  delete Q;
  delete R;

  cout << "time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::high_resolution_clock::now() - t1).count() / (double)1000000000 << "\n";
}
