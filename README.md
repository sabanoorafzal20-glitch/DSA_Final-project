# EcoSort

A small waste management system we built for our Data Structures course. You type in something like "old car battery", it figures out what kind of waste that is, puts the pickup in line, and then plans a route for a truck to collect it.

The whole thing is C++ and every data structure is written by hand, so there's no `vector`, `queue` or `priority_queue` anywhere. That was the point of the project. We also made a browser version so it's easier to show people (see below).

## What it does

- Works out the waste category from a short description, with a confidence score
- Takes pickup requests in the order they arrive
- Collects the most urgent ones first, so a syringe doesn't wait behind a newspaper
- Hands out trucks in turn so no single truck does all the work
- Plans the shortest route from the depot, through each stop, and back

## Which data structure does what

| Structure | What we used it for | File |
|---|---|---|
| Hash table | Looking up a word to find its category | `part1_requests.h` |
| Queue | Holding new requests until they're processed | `part1_requests.h` |
| Max heap | Keeping the most urgent request on top | `part1_requests.h` |
| Circular linked list | Rotating through the trucks | `part2_routing.h` |
| Graph + Dijkstra | The city map and shortest paths | `part2_routing.h` |

## Files

```
EcoSort/
├── main.cpp           menu and program flow
├── common.h           shared types and small helper functions
├── part1_requests.h   hash table, queue, heap, classifier
├── part2_routing.h    circular list, graph, route planner
├── web/
│   └── ecosort.html   browser version
└── README.md
```

## Running it

You just need g++ (or any compiler that handles C++11).

```bash
g++ -std=c++11 main.cpp -o ecosort
./ecosort
```

On Windows, run `ecosort.exe` instead.

For the web version, double-click `web/ecosort.html`. It opens in your browser and doesn't need a server. It's a JavaScript rewrite of the same logic, so the C++ program is still the "real" one.

## Using it

The menu looks like this:

```
1. Classify a waste item
2. Add collection request
3. Process queue -> priority heap
4. Dispatch truck & plan route
5. Show pending requests/trucks
6. Show city map
0. Exit
```

The usual order is: add a few requests with option 2, move them into the scheduler with 3, then dispatch a truck with 4. Option 5 shows what's still waiting and how many trips each truck has done.

Here's a run with four requests (banana peel, old car battery, used syringes, newspaper):

```
 Truck 1 (Green-1) assigned 4 request(s):
   #2 old car battery [Hazardous] @ Jinnah Colony
   #3 used syringes [Medical] @ Satyana Road
   #1 banana peel [Organic] @ Gulberg
   #4 newspaper [Recyclable] @ Samanabad

 Planned route:
   Depot -> Gulberg  (4 km)   -> collect #1
   Gulberg -> Satyana Road  (6 km)   -> collect #3
   Satyana Road -> D Ground -> Jinnah Colony  (6 km)   -> collect #2
   Jinnah Colony -> D Ground -> Samanabad  (5 km)   -> collect #4
   Samanabad -> D Ground -> Jinnah Colony -> Model Town -> Depot  (17 km)   -> return
 Total distance: 38 km
```

Notice the truck doesn't visit stops in priority order. The most urgent requests get picked for the trip, but once they're on the truck, the route just goes to whichever stop is closest next.

## How it works

**Classifying.** The description is split into words, lowercased, and each word is looked up in the hash table. If a word ends in "s" and isn't found, we try again without the "s", which is how "syringes" still matches "syringe". Whichever category gets the most hits wins, and ties go to the one with the higher priority. Confidence is `60 + 40 × (winning hits / total hits)`, so one clear match gives 100% and a mixed description gives less. If nothing matches, it falls back to "General" at 40%.

| Category | Priority | A few keywords |
|---|---|---|
| Hazardous | 5 | battery, paint, chemical, acid |
| Medical | 4 | syringe, mask, bandage, gloves |
| Organic | 3 | banana, peel, food, vegetable |
| E-Waste | 2 | phone, charger, laptop, cable |
| Recyclable | 1 | bottle, plastic, paper, glass |
| General | 1 | everything else |

**Scheduling.** The heap sorts by priority. If two requests have the same priority, the older one goes first.

**Routing.** Dijkstra gives us the shortest distance between any two places. The truck starts at the depot, always drives to the nearest stop it hasn't visited yet, and finally returns to the depot. This is a quick approach but not always the shortest possible tour. A different stop order could occasionally save a few km.

**The map.** Seven places, numbered 0 to 6 (0 is the depot): Gulberg, Model Town, Satyana Road, Jinnah Colony, D Ground and Samanabad are 1 to 6. Pickup locations use those numbers.

## Limits

- Up to 100 requests in the scheduler at once
- Up to 20 places on the map
- Each truck takes at most 4 requests per trip
- The keyword list is small and hard-coded, so unfamiliar items end up as "General"

## What we'd do with more time

- Read the map and keywords from a file instead of hard-coding them
- Use a heap inside Dijkstra instead of scanning for the smallest distance
- Try a smarter way to order the stops
- Save requests so they survive closing the program
- Give trucks a capacity limit

## Project Members

- **Member 1:** Saba Noor Afzal - 25F-0074
- **Member 2:** Rida Khalid - 25F-0013


