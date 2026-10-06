// ============================================================
//  SMART PARKING RECORD SYSTEM  (DSA Mini Project)
//  Data structures used (all hand-built, no STL containers):
//    1. Singly Linked List -> vehicles currently parked
//    2. Stack (array)      -> pool of free slots (LIFO)
//    3. Queue (linked)     -> vehicles waiting when lot is full
//    4. Stack (linked)     -> history of exited vehicles (latest on top)
// ============================================================
#include <iostream>
#include <string>
#include <ctime>
#include <limits>
#include <cctype>
using namespace std;

const int  TOTAL_SLOTS  = 5;      // size of the parking lot
const long UNIT_SECONDS = 3600;   // 1 billing unit = 1 hour (set to 60 to demo quickly)

// ---------- helpers ----------
string typeName(int t) { return t == 1 ? "Bike" : (t == 2 ? "Car" : "Truck"); }
int    rateOf(int t)   { return t == 1 ? 10 : (t == 2 ? 20 : 40); }   // per unit
string fmtTime(time_t t) {
    char buf[30];
    strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", localtime(&t));
    return string(buf);
}
string upper(string s) {
    for (char &c : s) c = toupper((unsigned char)c);
    return s;
}

// ---------- records ----------
struct Vehicle {
    string plate;
    int    type;     // 1 = Bike, 2 = Car, 3 = Truck
    int    slot;
    time_t entry;
};

struct ExitRecord {
    string plate;
    int    type, slot, fee;
    time_t in, out;
};

// ============================================================
// 1. STACK OF FREE SLOTS (array based)
//    Slot 1 is pushed last, so it is popped first.
// ============================================================
class SlotStack {
    int arr[TOTAL_SLOTS];
    int top;
public:
    SlotStack() : top(-1) { for (int i = TOTAL_SLOTS; i >= 1; i--) push(i); }
    bool push(int s) { if (top == TOTAL_SLOTS - 1) return false; arr[++top] = s; return true; }
    int  pop()       { return top == -1 ? -1 : arr[top--]; }
    bool empty()     { return top == -1; }
    int  size()      { return top + 1; }
    void display() {
        if (empty()) { cout << "  (no free slots)\n"; return; }
        cout << "  Free slots (next to be allotted first): ";
        for (int i = top; i >= 0; i--) cout << arr[i] << " ";
        cout << "\n";
    }
};

// ============================================================
// 2. LINKED LIST OF PARKED VEHICLES
// ============================================================
struct ParkedNode {
    Vehicle     v;
    ParkedNode *next;
};

class ParkedList {
    ParkedNode *head;
public:
    ParkedList() : head(nullptr) {}
    ~ParkedList() { while (head) { ParkedNode *t = head; head = head->next; delete t; } }

    void insert(const Vehicle &v) {                 // insert at head: O(1)
        head = new ParkedNode{v, head};
    }
    ParkedNode *find(const string &plate) {         // search: O(n)
        for (ParkedNode *p = head; p; p = p->next)
            if (p->v.plate == plate) return p;
        return nullptr;
    }
    bool remove(const string &plate, Vehicle &out) { // delete node: O(n)
        ParkedNode *prev = nullptr, *cur = head;
        while (cur && cur->v.plate != plate) { prev = cur; cur = cur->next; }
        if (!cur) return false;
        out = cur->v;
        if (prev) prev->next = cur->next; else head = cur->next;
        delete cur;
        return true;
    }
    void display() {
        if (!head) { cout << "  (lot is empty)\n"; return; }
        cout << "  Slot | Plate        | Type  | Entry time\n";
        cout << "  -----+--------------+-------+--------------------\n";
        for (ParkedNode *p = head; p; p = p->next)
            cout << "  " << p->v.slot << "    | " << p->v.plate
                 << string(13 - p->v.plate.size(), ' ') << "| "
                 << typeName(p->v.type) << string(6 - typeName(p->v.type).size(), ' ')
                 << "| " << fmtTime(p->v.entry) << "\n";
    }
};

// ============================================================
// 3. WAITING QUEUE (linked, FIFO)
// ============================================================
struct WaitNode {
    string    plate;
    int       type;
    WaitNode *next;
};

class WaitQueue {
    WaitNode *front, *rear;
public:
    WaitQueue() : front(nullptr), rear(nullptr) {}
    ~WaitQueue() { while (front) { WaitNode *t = front; front = front->next; delete t; } }

    bool empty() { return front == nullptr; }
    void enqueue(const string &plate, int type) {
        WaitNode *n = new WaitNode{plate, type, nullptr};
        if (rear) rear->next = n; else front = n;
        rear = n;
    }
    bool dequeue(string &plate, int &type) {
        if (!front) return false;
        WaitNode *t = front;
        plate = t->plate; type = t->type;
        front = front->next;
        if (!front) rear = nullptr;
        delete t;
        return true;
    }
    bool contains(const string &plate) {
        for (WaitNode *p = front; p; p = p->next) if (p->plate == plate) return true;
        return false;
    }
    void display() {
        if (!front) { cout << "  (nobody waiting)\n"; return; }
        int i = 1;
        for (WaitNode *p = front; p; p = p->next, i++)
            cout << "  " << i << ". " << p->plate << " (" << typeName(p->type) << ")\n";
    }
};

// ============================================================
// 4. HISTORY STACK (linked, LIFO) - latest exit on top
// ============================================================
struct HistNode {
    ExitRecord r;
    HistNode  *next;
};

class HistoryStack {
    HistNode *top;
public:
    HistoryStack() : top(nullptr) {}
    ~HistoryStack() { while (top) { HistNode *t = top; top = top->next; delete t; } }

    void push(const ExitRecord &r) { top = new HistNode{r, top}; }
    void display() {
        if (!top) { cout << "  (no exits recorded yet)\n"; return; }
        int total = 0;
        for (HistNode *p = top; p; p = p->next) {
            cout << "  " << p->r.plate << " | " << typeName(p->r.type)
                 << " | Slot " << p->r.slot
                 << " | In: " << fmtTime(p->r.in)
                 << " | Out: " << fmtTime(p->r.out)
                 << " | Fee: Rs." << p->r.fee << "\n";
            total += p->r.fee;
        }
        cout << "  Total collection so far: Rs." << total << "\n";
    }
};

// ============================================================
// PARKING SYSTEM - ties everything together
// ============================================================
class ParkingSystem {
    SlotStack    freeSlots;
    ParkedList   parked;
    WaitQueue    waiting;
    HistoryStack history;

    void parkVehicle(const string &plate, int type, int slot) {
        Vehicle v{plate, type, slot, time(nullptr)};
        parked.insert(v);
        cout << "  Parked " << plate << " at slot " << slot
             << " | Entry: " << fmtTime(v.entry) << "\n";
    }

public:
    void vehicleArrives(string plate, int type) {
        plate = upper(plate);
        if (parked.find(plate) || waiting.contains(plate)) {
            cout << "  This vehicle is already in the system.\n";
            return;
        }
        if (freeSlots.empty()) {
            waiting.enqueue(plate, type);
            cout << "  Lot FULL. " << plate << " added to the waiting queue.\n";
            return;
        }
        parkVehicle(plate, type, freeSlots.pop());
    }

    void vehicleLeaves(string plate) {
        plate = upper(plate);
        Vehicle v;
        if (!parked.remove(plate, v)) {
            cout << "  Vehicle not found in the lot.\n";
            return;
        }
        time_t out = time(nullptr);
        long secs  = (long)difftime(out, v.entry);
        long units = (secs + UNIT_SECONDS - 1) / UNIT_SECONDS;   // round up
        if (units < 1) units = 1;                                // minimum 1 unit
        int fee = (int)(units * rateOf(v.type));

        history.push(ExitRecord{v.plate, v.type, v.slot, fee, v.entry, out});
        freeSlots.push(v.slot);

        cout << "  ---------- BILL ----------\n"
             << "  Plate    : " << v.plate << " (" << typeName(v.type) << ")\n"
             << "  Slot     : " << v.slot << "\n"
             << "  In       : " << fmtTime(v.entry) << "\n"
             << "  Out      : " << fmtTime(out) << "\n"
             << "  Units    : " << units << " x Rs." << rateOf(v.type) << "\n"
             << "  TOTAL    : Rs." << fee << "\n"
             << "  --------------------------\n";

        // Auto-allot the freed slot to the first waiting vehicle
        string wPlate; int wType;
        if (waiting.dequeue(wPlate, wType)) {
            cout << "  Waiting vehicle " << wPlate << " gets the free slot automatically.\n";
            parkVehicle(wPlate, wType, freeSlots.pop());
        }
    }

    void search(string plate) {
        plate = upper(plate);
        ParkedNode *n = parked.find(plate);
        if (n) cout << "  " << plate << " is parked at slot " << n->v.slot
                    << " since " << fmtTime(n->v.entry) << "\n";
        else if (waiting.contains(plate)) cout << "  " << plate << " is in the waiting queue.\n";
        else cout << "  Vehicle not found.\n";
    }

    void showParked()  { parked.display(); }
    void showWaiting() { waiting.display(); }
    void showHistory() { history.display(); }
    void showFree()    { cout << "  Free: " << freeSlots.size() << " / " << TOTAL_SLOTS << "\n";
                         freeSlots.display(); }
};

// ---------- input helpers ----------
int readInt() {
    int x;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Enter a number: ";
    }
    return x;
}

int readType() {
    cout << "  Type (1 = Bike, 2 = Car, 3 = Truck): ";
    int t = readInt();
    while (t < 1 || t > 3) { cout << "  Enter 1, 2 or 3: "; t = readInt(); }
    return t;
}

int main() {
    ParkingSystem ps;
    int choice;
    do {
        cout << "\n===== SMART PARKING RECORD SYSTEM =====\n"
             << " 1. Vehicle arrives (entry)\n"
             << " 2. Vehicle leaves (exit + auto bill)\n"
             << " 3. Show parked vehicles\n"
             << " 4. Show free slots\n"
             << " 5. Show waiting queue\n"
             << " 6. Show exit history & collection\n"
             << " 7. Search a vehicle\n"
             << " 0. Exit\n"
             << " Choice: ";
        choice = readInt();
        string plate;
        switch (choice) {
            case 1: cout << "  Plate number: "; cin >> plate; ps.vehicleArrives(plate, readType()); break;
            case 2: cout << "  Plate number: "; cin >> plate; ps.vehicleLeaves(plate); break;
            case 3: ps.showParked();  break;
            case 4: ps.showFree();    break;
            case 5: ps.showWaiting(); break;
            case 6: ps.showHistory(); break;
            case 7: cout << "  Plate number: "; cin >> plate; ps.search(plate); break;
            case 0: cout << "  Goodbye!\n"; break;
            default: cout << "  Invalid choice.\n";
        }
    } while (choice != 0);
    return 0;
}
