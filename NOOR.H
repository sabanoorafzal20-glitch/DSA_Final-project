// ============================================================
//  PART 2 - MEMBER 2
//  Truck Management & Route Planning
//  Data structures: CIRCULAR LINKED LIST, GRAPH (+ Dijkstra)
// ============================================================
#ifndef PART2_ROUTING_H
#define PART2_ROUTING_H
#include <iostream>
#include "common.h"
using namespace std;

// ---------- 4. CIRCULAR LINKED LIST: trucks, round-robin ----------
class TruckRing {
    struct Node { int id; string name; int trips; Node* next; };
    Node* tail;      // tail->next = head
    Node* cur;       // next truck to be assigned
public:
    TruckRing() : tail(NULL), cur(NULL) {}
    ~TruckRing() {
        if (!tail) return;
        Node* c = tail->next; tail->next = NULL;
        while (c) { Node* t = c; c = c->next; delete t; }
    }
    void addTruck(int id, const string& name) {
        Node* n = new Node{ id, name, 0, NULL };
        if (!tail) { tail = n; n->next = n; cur = n; }
        else { n->next = tail->next; tail->next = n; tail = n; }
    }
    void nextTruck(int& id, string& name) {      // rotate
        id = cur->id; name = cur->name; cur->trips++;
        cur = cur->next;
    }
    void showUsage() const {
        if (!tail) return;
        Node* c = tail->next;
        do {
            cout << "   Truck " << c->id << " (" << c->name << ") - trips: " << c->trips << "\n";
            c = c->next;
        } while (c != tail->next);
    }
};

// ---------- 5. GRAPH (adjacency list) + Dijkstra ----------
class CityGraph {
    struct Edge { int to, w; Edge* next; };
    static const int MAXV = 20;
    Edge* adj[MAXV];
    string names[MAXV];
    int V;
public:
    CityGraph() : V(0) { for (int i = 0; i < MAXV; i++) adj[i] = NULL; }
    ~CityGraph() {
        for (int i = 0; i < V; i++) {
            Edge* e = adj[i];
            while (e) { Edge* t = e; e = e->next; delete t; }
        }
    }
    int addLocation(const string& name) { names[V] = name; return V++; }
    void addRoad(int u, int v, int w) {
        adj[u] = new Edge{ v, w, adj[u] };
        adj[v] = new Edge{ u, w, adj[v] };
    }
    int size() const { return V; }
    string name(int i) const { return names[i]; }

    void dijkstra(int src, int dist[], int prev[]) const {
        bool done[MAXV];
        for (int i = 0; i < V; i++) { dist[i] = INF; prev[i] = -1; done[i] = false; }
        dist[src] = 0;
        for (int k = 0; k < V; k++) {
            int u = -1;
            for (int i = 0; i < V; i++)
                if (!done[i] && (u == -1 || dist[i] < dist[u])) u = i;
            if (u == -1 || dist[u] == INF) break;
            done[u] = true;
            for (Edge* e = adj[u]; e; e = e->next)
                if (dist[u] + e->w < dist[e->to]) { dist[e->to] = dist[u] + e->w; prev[e->to] = u; }
        }
    }
    int distance(int a, int b) const {
        int dist[MAXV], prev[MAXV];
        dijkstra(a, dist, prev);
        return dist[b];
    }
    void printPath(int src, int dst) const {
        int dist[MAXV], prev[MAXV], path[MAXV], len = 0;
        dijkstra(src, dist, prev);
        for (int v = dst; v != -1; v = prev[v]) path[len++] = v;
        for (int i = len - 1; i >= 0; i--) cout << names[path[i]] << (i ? " -> " : "");
        cout << "  (" << dist[dst] << " km)";
    }
    void display() const {
        for (int i = 0; i < V; i++) {
            cout << "   [" << i << "] " << names[i] << " : ";
            for (Edge* e = adj[i]; e; e = e->next) cout << names[e->to] << "(" << e->w << "km) ";
            cout << "\n";
        }
    }
};

// ---------- Route planner: trucks + city together ----------
class RoutePlanner {
    CityGraph city;
    TruckRing trucks;
    void buildCity() {
        int depot = city.addLocation("Depot");
        int a = city.addLocation("Gulberg");
        int b = city.addLocation("Model Town");
        int c = city.addLocation("Satyana Road");
        int d = city.addLocation("Jinnah Colony");
        int e = city.addLocation("D Ground");
        int f = city.addLocation("Samanabad");
        city.addRoad(depot, a, 4);  city.addRoad(depot, b, 7);
        city.addRoad(a, b, 3);      city.addRoad(a, c, 6);
        city.addRoad(b, d, 5);      city.addRoad(c, e, 4);
        city.addRoad(d, e, 2);      city.addRoad(d, f, 6);
        city.addRoad(e, f, 3);      city.addRoad(c, f, 9);
    }
public:
    RoutePlanner() {
        buildCity();
        trucks.addTruck(1, "Green-1");
        trucks.addTruck(2, "Green-2");
        trucks.addTruck(3, "Green-3");
    }
    int locationCount() const { return city.size(); }
    bool validLocation(int loc) const { return loc >= 1 && loc < city.size(); }
    void showMap() const { cout << " City map:\n"; city.display(); }
    void showTrucks() const { cout << " Truck usage:\n"; trucks.showUsage(); }

    // Assign batch to next truck (round-robin) and plan a route:
    // nearest-neighbour using Dijkstra shortest paths, depot -> stops -> depot
    void dispatch(Request batch[], int n) {
        int tid; string tname; trucks.nextTruck(tid, tname);
        cout << "\n Truck " << tid << " (" << tname << ") assigned " << n << " request(s):\n";
        for (int i = 0; i < n; i++)
            cout << "   #" << batch[i].id << " " << batch[i].item << " [" << batch[i].category
            << "] @ " << city.name(batch[i].location) << "\n";

        bool visited[20] = { false };
        int pos = 0, total = 0;
        cout << "\n Planned route:\n";
        for (int step = 0; step < n; step++) {
            int bi = -1, bd = INF;
            for (int i = 0; i < n; i++) {
                if (visited[i]) continue;
                int d = city.distance(pos, batch[i].location);
                if (d < bd) { bd = d; bi = i; }
            }
            visited[bi] = true;
            cout << "   "; city.printPath(pos, batch[bi].location);
            cout << "   -> collect #" << batch[bi].id << "\n";
            total += bd; pos = batch[bi].location;
        }
        cout << "   "; city.printPath(pos, 0); cout << "   -> return\n";
        total += city.distance(pos, 0);
        cout << " Total distance: " << total << " km\n";
    }
};

#endif