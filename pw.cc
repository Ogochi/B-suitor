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
set<pair<int, int>> S[MAX], N[MAX]; // set<waga - nr sasiada>
set<int> T[MAX]; // nr sasiada

void readGraphAndPrepare(char* fileName) {
  std::ifstream infile(fileName);

  while (infile.peek() == '#')
    infile.ignore(std::numeric_limits<std::streamsize>::max(), infile.widen('\n'));

  int from, to, w;
  bool added[MAX] = {false};
  while (infile >> from >> to >> w) {
    if (!added[from])
      Q->push(from);
    if (!added[to])
      Q->push(to);
    added[from] = true;
    added[to] = true;

    N[from].insert({w, to});
    N[to].insert({w, from});
  }
}

inline int sLast(int x, int method) {
  if (x == 0)
    cout << "slast: " << bvalue(method, x) << S[x].size() << "\n";
  if (bvalue(method, x) == S[x].size())
    return S[x].begin()->second;
  else
    return -1;
}

inline int wSLast(int x, int method) {
  if (bvalue(method, x) == S[x].size())
    return S[x].begin()->first;
  else
    return -1;
}

auto findMax(int curr, int method) {
  auto i = N[curr].rbegin();
  while (i != N[curr].rend()) {
    if (T[curr].find(i->second) == T[curr].end())
      if (i->first > wSLast(i->second, method) ||
          (wSLast(i->second, method) == i->first && i->second > sLast(i->second, method)))
        return i;
    i++;
  }
  return N[curr].rend();
}

int main(int argc, char** argv) {
  //std::ios_base::sync_with_stdio(0);
  int method = atoi(argv[3]); // bedziemy iterowac
  auto t1 = std::chrono::high_resolution_clock::now();
  readGraphAndPrepare(argv[2]);

  while (!Q->empty()) {
    while (!Q->empty()) {
      int curr = Q->front();
      cout << "Tera: " << curr << "\n";
      Q->pop();

      while (T[curr].size() < bvalue(method, curr)) {
        auto x = findMax(curr, method);
        if (x == N[curr].rend())
          break;
        cout << "\tZnalazlem: " << x->second << "\n";

        int y = sLast(x->second, method);
        S[x->second].insert({x->first, curr});
        T[curr].insert(x->second);

        if (y != -1) {
          S[x->second].erase(S[x->second].begin());
          T[y].erase(x->second);
          cout << "alert!";
          R->push(y);
        }
      }
    }
    cout << "a " << R->size() << "\n";
    delete(Q);
    Q = R;
    R = new queue<int>();
  }

  int sum = 0;
  for (int i = 0; i <= 5; i++) {
    cout << i << ":\n\tS: ";
    for (auto j : S[i]) {
      cout << "(" << j.first << ", " << j.second << ")";
      sum += j.first;
    }
    cout << "\n\tT: ";
    for (auto j : T[i]) {
      cout << j << " ";
    }
    cout << "\n";
  }
  cout << "sum: " << sum / 2 << "\n";
  cout << "time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::high_resolution_clock::now() - t1).count() / (double)1000000000 << "\n";
}
