#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>

#define ToMilliseconds std::chrono::duration_cast<std::chrono::milliseconds>
typedef std::chrono::high_resolution_clock CoolClock;
using namespace std;

class Random {
    int coef;
public:
    Random(double q = 0.5) {
        if ((q < 0) || (q > 1))
            throw - 1;
        coef = RAND_MAX * q;
    }
    static void Rand_Init(int seed = -1) {
        if (seed == -1)
            srand(time(NULL));
        else
            srand(seed);
    }
    inline bool toss() noexcept {
        return rand() <= coef;
    }
    static int give(unsigned i) noexcept {
        return rand() % i;
    }
};

class disjoint_set1{
    vector<int> p;
    long unsigned int used;
    int setCount;
public:
    disjoint_set1(unsigned n):p(n, -1), used(0), setCount(0){ }
    void MakeSet(int x){
        p[x] = x;
        ++used;
        ++setCount;
    }
    int Find(int x) noexcept{
        do{
            x = p[x];
        }while(x != p[x]);
        return x;
    }
    void Union(int x, int y) noexcept{
        if ( (x = Find(x)) == (y = Find(y)) )
            return;
        p[x] = y;
        --setCount;
    }

    inline bool check(){ return (used == p.size()) && (setCount == 1);}
    inline int get(int x){ return p[x]; }
    void add(int x, int y) noexcept{
        if(p[x] == -1)
            MakeSet(x);
        if(p[y] == -1)
            MakeSet(y);
        Union(x, y);
    }
};

class disjoint_set{
    vector<int> p;
    vector<int> rank;
    long unsigned int used;
    int setCount;
public:
    disjoint_set(unsigned n):p(n, -1), rank(n), used(0), setCount(0){ }
    void MakeSet(int x){
        p[x] = x;
        rank[x] = 0;
        ++used;
        ++setCount;
    }
    int Find(int x) noexcept{
        return ( x == p[x] ? x : p[x] = Find(p[x]) );
    }
    void Union(int x, int y) noexcept{
        if ( (x = Find(x)) == (y = Find(y)) )
            return;

        if ( rank[x] <  rank[y] )
            p[x] = y;
        else {
            p[y] = x;
            if ( rank[x] == rank[y] )
                ++rank[x];
        }
        --setCount;
    }

    inline bool check(){ return (used == p.size()) && (setCount == 1);}
    inline int get(int x){ return p[x]; }
    void add(int x, int y) noexcept{
        if(p[x] == -1)
            MakeSet(x);
        if(p[y] == -1)
            MakeSet(y);
        Union(x, y);
    }
};

struct Edge{int x, y;};

void generate_graph(vector<vector<Edge>> *g, int n, double prob, unsigned w)
{   disjoint_set set(n);
    Random r(prob);

    for(int i = 1; i < n; ++i) // Generate random edges - O(n^2)
      for(int j = 0; j < i; ++j)
        if(r.toss()){
            (*g)[Random::give(w)].push_back(Edge{j, i});
            set.add(j, i);
        }



    int p = -1;         // Binding all together
    for(int i = 0; i < n; ++i)
    {   int cur = set.get(i);
        if(cur == -1)// alone
        {   (*g)[Random::give(w)].push_back(Edge{i, (i+1)%n});
            set.add(i, (i+1)%n);
            p = set.Find(i); // save group
        }else
        {   if(p == -1)
                p = set.Find(cur);
            else if(p != (cur = set.Find(i))) // find another group
            {   (*g)[Random::give(w)].push_back(Edge{p, cur});
                set.add(p, cur);
            }
        }
    }
}

void Kruskal(vector<vector<Edge>> *sg, int n)
{   disjoint_set set(n);
    //disjoint_set1 set(n);

    for(auto& v: *sg)
        for(auto& e: v){
            if((set.get(e.x) != -1) && (set.get(e.y) != -1)
                    && (set.Find(e.x) == set.Find(e.y)))
                continue;

            set.add(e.x, e.y);
            if(set.check())
                return;
        }
}

int main()
{   Random::Rand_Init();
    ofstream file;
    file.open("results.txt", std::ios::app);
    uint32_t maxWeight = 10;
    vector<vector<Edge>> sortedEdges(maxWeight);

for(int i = 13; i < 14; ++i) // n = 2^17  - Crash
{   int n = 1 << i; double p = 0.2;//3.0 * log(n)/n; // m ~ n(n-1)/2 * p
    file << "n = 2^" << i << " p = " << p << endl;

    for(int j = 0; j < 1; ++j) // Number of experiments
    {   for(auto& v: sortedEdges) // Reserve memory
            v.reserve(long(n) * p * (n / 2 / maxWeight));
        //  // Generation
        auto t1 = CoolClock::now();
        generate_graph(&sortedEdges, n, p, maxWeight);
        auto t2 = CoolClock::now();
        file << ToMilliseconds(t2 - t1).count()
            << " ms for generation" << endl;
        //  // Kraskal algorithm
        t1 = CoolClock::now();
        Kruskal(&sortedEdges, n);
        t2 = CoolClock::now();
        file << ToMilliseconds(t2 - t1).count()
             << " ms for Kruskal" << endl;

        file << endl;
        for(auto& v: sortedEdges) // Clear memory
            v.clear();
    }
}   //*/
    file.close();
    return 0;
}
