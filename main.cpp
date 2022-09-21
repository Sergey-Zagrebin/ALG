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
protected:
    vector<int> p;
    long unsigned int used;
    int setCount;
public:
    disjoint_set1(unsigned n):p(n, -1), used(0), setCount(0){ }
    virtual void MakeSet(int x) noexcept{
        p[x] = x;
        ++used;
        ++setCount;
    }
    virtual int Find(int x) noexcept{
        do{
            x = p[x];
        }while(x != p[x]);
        return x;
    }
    virtual void Union(int x, int y) noexcept{
        if ( (x = Find(x)) == (y = Find(y)) )
            return;
        p[x] = y;
        --setCount;
    }

    inline bool check() noexcept{ return (used == p.size()) && (setCount == 1);}
    inline int get(int x) noexcept{ return p[x]; }
    void add(int x, int y) noexcept{
        if(p[x] == -1)
            MakeSet(x);
        if(p[y] == -1)
            MakeSet(y);
        Union(x, y);
    }
};

class disjoint_set : public disjoint_set1{
    vector<int> rank;
public:
    disjoint_set(unsigned n):disjoint_set1(n), rank(n){ }
    void MakeSet(int x) noexcept{
        disjoint_set1::MakeSet(x);
        rank[x] = 0;
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

void Kruskal_slow(vector<vector<Edge>> *sg, int n)
{   disjoint_set1 set(n);

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
    string s; uint32_t maxWeight;
    int n1, n2, c, choice; double p; //3.0 * log(n)/n; // m ~ n(n-1)/2 * p
    cout << "Result file name: ";
    cin >> s;
    cout << "Max edge weight: ";
    cin >> maxWeight;
    cout << "Probability of each edge: ";
    cin >> p;
    cout << "From (<2^17) n = 2^";
    cin >> n1;
    cout << "Up to (<2^17) n = 2^";
    cin >> n2;
    cout << "Number of experiments for each n: ";
    cin >> c;
    cout << "Improved algorithm or not? (0 - no, 1 - yes): ";
    cin >> choice;
    cout << "Running..." << endl;
    file.open(s, std::ios::app);
    vector<vector<Edge>> sortedEdges(maxWeight);

for(int i = n1; i <= n2; ++i) // n = 2^17  - Crash
{   int n = 1 << i;
    file << "n = 2^" << i << " p = " << p << " Max weight = " << maxWeight << endl;
    cout << "Launch with n = 2^" << i;
    for(int j = 0; j < c; ++j) // Number of experiments
    {   for(auto& v: sortedEdges) // Reserve memory
            v.reserve(long(n) * p * (n / 2 / maxWeight));
        //  // Generation
        auto t1 = CoolClock::now();
        generate_graph(&sortedEdges, n, p, maxWeight);
        auto t2 = CoolClock::now();
        file << ToMilliseconds(t2 - t1).count()
            << " ms for generation" << endl;
        //  // Kraskal algorithm
        if(choice){
            t1 = CoolClock::now();
            Kruskal(&sortedEdges, n);
        }else{
            t1 = CoolClock::now();
            Kruskal_slow(&sortedEdges, n);
        }
        t2 = CoolClock::now();
        file << ToMilliseconds(t2 - t1).count()
             << " ms for Kruskal" << (choice ? " " : " not ") << "optimized" << endl;

        file << endl;
        for(auto& v: sortedEdges) // Clear memory
            v.clear();
    }
    cout << " completed" << endl;
}   //*/
    cout << "Program finished" << endl;
    file.close();
    return 0;
}
