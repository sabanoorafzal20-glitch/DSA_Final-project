// ===========================================================
//  Waste Classification & Request Management
//  Data structures: HASH TABLE, QUEUE, MAX HEAP
// ============================================================
#ifndef RIDA_H
#define RIDA_H
#include <iostream>
#include "common.h"
using namespace std;

// ---------- 1. HASH TABLE (separate chaining): keyword -> Category ----------
class HashTable {
    struct Node {
        string key; Category val; Node* next;
        Node(string k, Category v) : key(k), val(v), next(NULL) {}
    };
    static const int SIZE = 53;
    Node* table[SIZE];
    int hash(const string& k) const {
        unsigned long h = 5381;
        for (size_t i = 0; i < k.size(); i++) h = h * 33 + k[i];
        return h % SIZE;
    }
public:
    HashTable() { for (int i = 0; i < SIZE; i++) table[i] = NULL; }
    ~HashTable() {
        for (int i = 0; i < SIZE; i++) {
            Node* c = table[i];
            while (c) { Node* t = c; c = c->next; delete t; }
        }
    }
    void insert(const string& key, Category v) {
        int idx = hash(key);
        Node* n = new Node(key, v);
        n->next = table[idx];
        table[idx] = n;
    }
    bool search(const string& key, Category& out) const {
        for (Node* c = table[hash(key)]; c; c = c->next)
            if (c->key == key) { out = c->val; return true; }
        return false;
    }
};

// ---------- 2. QUEUE (linked list): incoming requests, FIFO ----------
class RequestQueue {
    struct Node { Request data; Node* next; Node(Request r) : data(r), next(NULL) {} };
    Node* front, * rear;
    int count;
public:
    RequestQueue() : front(NULL), rear(NULL), count(0) {}
    ~RequestQueue() { Request r; while (dequeue(r)); }
    void enqueue(Request r) {
        Node* n = new Node(r);
        if (!rear) front = rear = n; else { rear->next = n; rear = n; }
        count++;
    }
    bool dequeue(Request& out) {
        if (!front) return false;
        Node* t = front; out = t->data;
        front = front->next;
        if (!front) rear = NULL;
        delete t; count--;
        return true;
    }
    bool empty() const { return count == 0; }
    int size() const { return count; }
};

// ---------- 3. MAX HEAP: most urgent request on top ----------
// higher priority first; ties -> older request (smaller id) first
class MaxHeap {
    Request a[100];
    int n;
    bool better(const Request& x, const Request& y) const {
        if (x.priority != y.priority) return x.priority > y.priority;
        return x.id < y.id;
    }
    void up(int i) {
        while (i > 0 && better(a[i], a[(i - 1) / 2])) {
            Request t = a[i]; a[i] = a[(i - 1) / 2]; a[(i - 1) / 2] = t;
            i = (i - 1) / 2;
        }
    }
    void down(int i) {
        while (true) {
            int l = 2 * i + 1, r = 2 * i + 2, b = i;
            if (l < n && better(a[l], a[b])) b = l;
            if (r < n && better(a[r], a[b])) b = r;
            if (b == i) break;
            Request t = a[i]; a[i] = a[b]; a[b] = t;
            i = b;
        }
    }
public:
    MaxHeap() : n(0) {}
    bool push(Request r) { if (n >= 100) return false; a[n] = r; up(n++); return true; }
    bool pop(Request& out) {
        if (n == 0) return false;
        out = a[0]; a[0] = a[--n]; down(0);
        return true;
    }
    bool empty() const { return n == 0; }
    int size() const { return n; }
    void display() const {
        for (int i = 0; i < n; i++)
            cout << "   #" << a[i].id << "  " << a[i].item << "  [" << a[i].category
            << ", priority " << a[i].priority << ", loc " << a[i].location << "]\n";
    }
};

// ---------- Classifier ("AI" - rule based scoring on the hash table) ----------
class WasteClassifier {
    HashTable kb;
    void load() {
        Category haz = { "Hazardous", 5 }, med = { "Medical", 4 }, org = { "Organic", 3 },
            ew = { "E-Waste", 2 }, rec = { "Recyclable", 1 };
        const char* hz[] = { "battery","paint","chemical","pesticide","acid","bulb","thinner" };
        const char* md[] = { "syringe","mask","bandage","medicine","gloves","needle" };
        const char* og[] = { "banana","apple","peel","food","vegetable","leaves","bread","rice","meat" };
        const char* e[] = { "phone","charger","laptop","cable","keyboard","mouse","circuit","tv" };
        const char* rc[] = { "bottle","plastic","paper","glass","can","cardboard","newspaper","tin","carton" };
        for (int i = 0; i < 7; i++) kb.insert(hz[i], haz);
        for (int i = 0; i < 6; i++) kb.insert(md[i], med);
        for (int i = 0; i < 9; i++) kb.insert(og[i], org);
        for (int i = 0; i < 8; i++) kb.insert(e[i], ew);
        for (int i = 0; i < 9; i++) kb.insert(rc[i], rec);
    }
public:
    WasteClassifier() { load(); }

    // Scores every word of the description; best category wins.
    Category classify(const string& desc, int& confidence) {
        string d = toLower(desc), word;
        Category best = { "General", 1 }, cats[8];
        int scores[8] = { 0 }, nc = 0, total = 0;
        for (size_t i = 0; i <= d.size(); i++) {
            if (i < d.size() && isalpha(d[i])) { word += d[i]; continue; }
            if (word.empty()) continue;
            Category c;
            bool found = kb.search(word, c) ||
                (word.size() > 1 && word[word.size() - 1] == 's' &&
                    kb.search(word.substr(0, word.size() - 1), c));   // plural
            if (found) {
                int k = 0;
                while (k < nc && cats[k].name != c.name) k++;
                if (k == nc) cats[nc++] = c;
                scores[k]++; total++;
            }
            word.clear();
        }
        if (nc == 0) { confidence = 40; return best; }
        int bestScore = 0;
        for (int k = 0; k < nc; k++)
            if (scores[k] > bestScore ||
                (scores[k] == bestScore && cats[k].priority > best.priority)) {
                bestScore = scores[k]; best = cats[k];
            }
        confidence = 60 + 40 * bestScore / total;
        return best;
    }
};

// ---------- Request manager: Queue -> Heap pipeline ----------
class RequestManager {
    RequestQueue incoming;
    MaxHeap scheduler;
    WasteClassifier classifier;
    int nextId;
public:
    RequestManager() : nextId(1) {}

    void classifyOnly(const string& item) {
        int conf; Category c = classifier.classify(item, conf);
        cout << " -> Category: " << c.name << "  (confidence " << conf
            << "%, priority " << c.priority << ")\n";
    }
    // classify + enqueue; returns the created request
    Request addRequest(const string& item, int location) {
        int conf; Category c = classifier.classify(item, conf);
        Request r = { nextId++, item, c.name, c.priority, location };
        incoming.enqueue(r);
        cout << " Request #" << r.id << " classified as " << c.name << " (" << conf
            << "%) and queued.\n";
        return r;
    }
    void processQueue() {                     // Queue -> Heap
        int moved = 0; Request r;
        while (incoming.dequeue(r)) { scheduler.push(r); moved++; }
        cout << " " << moved << " request(s) moved from queue to priority scheduler.\n";
    }
    // Pops up to 'cap' most urgent requests; returns how many were popped
    int popUrgent(Request out[], int cap) {
        int n = 0;
        while (n < cap && scheduler.pop(out[n])) n++;
        return n;
    }
    bool hasScheduled() const { return !scheduler.empty(); }
    void showPending() const {
        cout << " Queue size: " << incoming.size()
            << "\n Scheduler (" << scheduler.size() << "):\n";
        scheduler.display();
    }
};

#endif