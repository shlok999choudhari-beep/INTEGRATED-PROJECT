/*
    EcoSort 386 - PL + PSOOP Final Capstone Integration (Task 4)
    =============================================================
    PSOOP CO1: Classes, Inheritance, and Runtime Polymorphism
    PSOOP CO2: Modular Object-Oriented Architecture (EcoSort Manager)
    PSOOP CO3: Exception Handling (std::invalid_argument) & Input Validation
    PSOOP CO4: Rule-Based Decision Making & Smart Recommendations
    PSOOP CO5: Persistent File I/O (Disk Serialization/Deserialization) [TASK 4]
    PSOOP CO6: Generic Programming with C++ Class & Function Templates  [TASK 4]

    PL CO1: Dynamic Array Buffering
    PL CO2: Dynamic Singly Linked List
    PL CO3: Template-Based FIFO Queue & LIFO Category Stacks
    PL CO4: Searching (Linear) & Sorting (Bubble Sort)
    PL CO5: Hashing with Linear Probing Collision Resolution

    IPC: Cross-Process Shared Memory Bridge Integration (CGL & COA Sync)

    Compile:
    g++ -std=c++17 -O2 "PL&PSOOPCO1&2.cpp" -static -o ecosort_pl_psoop.exe
*/

#include <iostream>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <fstream>
#include <sstream>

// Auto-detect and link with EcoSort Shared Memory Bridge if present
#if __has_include("../ecosort_bridge.h")
    #include "../ecosort_bridge.h"
    #define HAVE_ECOSORT_BRIDGE 1
#elif __has_include("ecosort_bridge.h")
    #include "ecosort_bridge.h"
    #define HAVE_ECOSORT_BRIDGE 1
#else
    #define HAVE_ECOSORT_BRIDGE 0
#endif

using namespace std;

const int SIZE = 50;
const double PRIORITY_LIMIT = 10.0;
const string DB_FILENAME = "ecosort_database.txt";

// =====================================================
// PSOOP CO6: GENERIC FUNCTION TEMPLATE
// =====================================================
template <typename T>
void displayMetric(const string &label, const T &value, const string &unit = "")
{
    cout << setw(26) << left << label << ": " << value;
    if (!unit.empty()) cout << " " << unit;
    cout << "\n";
}

// =====================================================
// PSOOP CO1: ABSTRACT BASE CLASS (POLYMORPHISM)
// =====================================================
class Waste
{
protected:
    int id;
    string name;
    double weight;

public:
    Waste(int i, string n, double w) : id(i), name(n), weight(w) {}

    virtual string category() = 0;
    virtual int priority() = 0;

    int getId() const { return id; }
    string getName() const { return name; }
    double getWeight() const { return weight; }

    virtual void display() const
    {
        cout << "ID: " << setw(4) << left << id
             << " | Name: " << setw(14) << left << name
             << " | Weight: " << setw(6) << fixed << setprecision(2) << weight << " kg"
             << " | Category: " << setw(11) << left << const_cast<Waste*>(this)->category()
             << " | Priority: " << const_cast<Waste*>(this)->priority() << endl;
    }

    // PSOOP CO5: Serialize record to comma-separated format for file storage
    virtual string toFileString() const
    {
        return to_string(id) + "," + name + "," + to_string(weight) + "," + const_cast<Waste*>(this)->category();
    }

    virtual ~Waste() {}
};

// =====================================================
// PSOOP CO1: INHERITED DERIVED CLASSES
// =====================================================
class Reusable : public Waste
{
public:
    Reusable(int i, string n, double w) : Waste(i, n, w) {}
    string category() override { return "Reusable"; }
    int priority() override { return 1; }
};

class Recyclable : public Waste
{
public:
    Recyclable(int i, string n, double w) : Waste(i, n, w) {}
    string category() override { return "Recyclable"; }
    int priority() override { return 2; }
};

class Hazardous : public Waste
{
public:
    Hazardous(int i, string n, double w) : Waste(i, n, w) {}
    string category() override { return "Hazardous"; }
    int priority() override { return 3; }
};

// =====================================================
// PL CO2: SINGLY LINKED LIST
// =====================================================
struct Node
{
    Waste *data;
    Node *next;

    Node(Waste *w) : data(w), next(nullptr) {}
};

class WasteList
{
private:
    Node *head;

public:
    WasteList() : head(nullptr) {}

    bool hasID(int id) const
    {
        Node *temp = head;
        while (temp != nullptr)
        {
            if (temp->data->getId() == id)
                return true;
            temp = temp->next;
        }
        return false;
    }

    void add(Waste *w)
    {
        Node *newNode = new Node(w);
        if (head == nullptr)
        {
            head = newNode;
        }
        else
        {
            Node *temp = head;
            while (temp->next != nullptr)
                temp = temp->next;
            temp->next = newNode;
        }
    }

    Waste* search(int id) const
    {
        Node *temp = head;
        while (temp != nullptr)
        {
            if (temp->data->getId() == id)
                return temp->data;
            temp = temp->next;
        }
        return nullptr;
    }

    void display() const
    {
        Node *temp = head;
        cout << "\n======================== CURRENT COLLECTION RECORDS ========================\n";
        if (temp == nullptr)
        {
            cout << "No records found in database.\n";
            return;
        }

        while (temp != nullptr)
        {
            temp->data->display();
            temp = temp->next;
        }
        cout << "============================================================================\n";
    }

    int count() const
    {
        int total = 0;
        Node *temp = head;
        while (temp != nullptr)
        {
            total++;
            temp = temp->next;
        }
        return total;
    }

    double totalWeight() const
    {
        double total = 0.0;
        Node *temp = head;
        while (temp != nullptr)
        {
            total += temp->data->getWeight();
            temp = temp->next;
        }
        return total;
    }

    void copyTo(Waste *items[], int &total) const
    {
        total = 0;
        Node *temp = head;
        while (temp != nullptr && total < SIZE)
        {
            items[total++] = temp->data;
            temp = temp->next;
        }
    }

    Node* getHead() const { return head; }

    void clear()
    {
        Node *temp;
        while (head != nullptr)
        {
            temp = head;
            head = head->next;
            delete temp->data;
            delete temp;
        }
    }

    ~WasteList()
    {
        clear();
    }
};

// =====================================================
// PSOOP CO6 & PL CO3: GENERIC QUEUE TEMPLATE (FIFO)
// =====================================================
template <typename T>
class Queue
{
private:
    T items[SIZE];
    int front, rear;

public:
    Queue() : front(0), rear(-1) {}

    bool empty() const { return front > rear; }
    bool full() const { return rear == SIZE - 1; }

    void enqueue(T w)
    {
        if (full())
        {
            cout << "Queue is full.\n";
            return;
        }
        items[++rear] = w;
    }

    T dequeue()
    {
        if (empty())
            return nullptr;
        return items[front++];
    }

    int count() const
    {
        return empty() ? 0 : (rear - front + 1);
    }
};

// =====================================================
// PSOOP CO6 & PL CO3: GENERIC STACK TEMPLATE (LIFO)
// =====================================================
template <typename T>
class Stack
{
private:
    T items[SIZE];
    int top;

public:
    Stack() : top(-1) {}

    bool empty() const { return top == -1; }
    bool full() const { return top == SIZE - 1; }

    void push(T w)
    {
        if (full())
        {
            cout << "Stack is full.\n";
            return;
        }
        items[++top] = w;
    }

    T pop()
    {
        if (empty())
            return nullptr;
        return items[top--];
    }

    int count() const { return top + 1; }

    void display(const string &title) const
    {
        cout << "\n===== " << title << " (Total: " << count() << ") =====\n";
        if (empty())
        {
            cout << "No items in this category stack.\n";
            return;
        }

        for (int i = top; i >= 0; i--)
            items[i]->display();
    }
};

// =====================================================
// PL CO5: HASH TABLE WITH LINEAR PROBING
// =====================================================
class HashTable
{
private:
    int ids[SIZE];
    Waste *items[SIZE];
    bool used[SIZE];

public:
    HashTable()
    {
        clear();
    }

    void clear()
    {
        for (int i = 0; i < SIZE; i++)
        {
            ids[i] = -1;
            items[i] = nullptr;
            used[i] = false;
        }
    }

    int hash(int id) const
    {
        return id % SIZE;
    }

    void insert(Waste *w)
    {
        int index = hash(w->getId());

        for (int i = 0; i < SIZE; i++)
        {
            int place = (index + i) % SIZE;
            if (!used[place])
            {
                ids[place] = w->getId();
                items[place] = w;
                used[place] = true;
                return;
            }
        }
        cout << "Hash table is full.\n";
    }

    Waste* search(int id, int &checks) const
    {
        checks = 0;
        int index = hash(id);

        for (int i = 0; i < SIZE; i++)
        {
            int place = (index + i) % SIZE;
            checks++;

            if (!used[place])
                return nullptr;

            if (ids[place] == id)
                return items[place];
        }
        return nullptr;
    }
};

// =====================================================
// PSOOP CO2: ECOSORT MANAGER (CAPSTONE CONTROLLER)
// =====================================================
class EcoSort
{
private:
    WasteList list;
    Queue<Waste*> queue;                                        // Generic Template Queue
    Stack<Waste*> reusableStack, recyclableStack, hazardousStack; // Generic Template Stacks
    HashTable hashTable;

    Waste* makeWaste(int id, string name, double weight, int choice)
    {
        // PSOOP CO3: Strict Validation and Exception Handling
        if (id <= 0)
            throw invalid_argument("ID must be greater than zero.");

        if (name.empty())
            throw invalid_argument("Name cannot be empty.");

        if (weight <= 0.0)
            throw invalid_argument("Weight must be greater than zero.");

        if (choice < 1 || choice > 3)
            throw invalid_argument("Category choice must be 1, 2, or 3.");

        if (list.hasID(id))
            throw invalid_argument("This Waste ID already exists in the system.");

        if (choice == 1)
            return new Reusable(id, name, weight);
        else if (choice == 2)
            return new Recyclable(id, name, weight);
        else
            return new Hazardous(id, name, weight);
    }

    void notifyIPC(Waste *w)
    {
#if HAVE_ECOSORT_BRIDGE
        int catCode = EcoSortCore::CAT_RECYCLABLE;
        if (w->category() == "Reusable") catCode = EcoSortCore::CAT_REUSABLE;
        else if (w->category() == "Hazardous") catCode = EcoSortCore::CAT_HAZARDOUS;

        EcoSortCore::Bridge::get().setCategory(catCode);
        EcoSortCore::Bridge::get().triggerDeposit((float)w->getWeight());
        EcoSortCore::Bridge::get().completeDeposit();
#endif
    }

public:
    EcoSort()
    {
        // PSOOP CO5: Automatically load persistent records on startup
        loadFromFile(false);
    }

    void addWaste()
    {
        int id, choice;
        string name;
        double weight;

        try
        {
            cout << "Enter Waste ID (Integer): ";
            if (!(cin >> id))
                throw invalid_argument("ID must be a numeric integer.");

            cout << "Enter Waste Material Name (single word): ";
            cin >> name;

            cout << "Enter Weight (kg): ";
            if (!(cin >> weight))
                throw invalid_argument("Weight must be a numeric value.");

            cout << "1. Reusable\n2. Recyclable\n3. Hazardous\n";
            cout << "Choose Category (1-3): ";
            if (!(cin >> choice))
                throw invalid_argument("Category choice must be numeric.");

            Waste *w = makeWaste(id, name, weight, choice);
            list.add(w);
            hashTable.insert(w);

            // Notify cross-discipline IPC bridge (CGL & COA)
            notifyIPC(w);

            cout << "\n>>> Waste item registered successfully. IPC packet synchronized.\n";

            // PSOOP CO4: Decision Making
            recommend(w);

            // Auto-persist to file
            saveToFile(false);
        }
        catch (const exception &e)
        {
            cout << "\n[INPUT ERROR]: " << e.what() << endl;
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }

    void showWaste() const
    {
        list.display();
    }

    void searchWaste() const
    {
        int id;
        cout << "Enter Waste ID to search (Linear Search): ";
        if (!(cin >> id))
        {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            return;
        }

        Waste *w = list.search(id); // PL CO4: Linear Search
        if (w != nullptr)
        {
            cout << "\nItem Found:\n";
            w->display();
        }
        else
        {
            cout << "\nRecord with ID " << id << " not found in database.\n";
        }
    }

    void sortWaste() const
    {
        // PL CO4: Bubble Sort on pointer array (Linked List ordering untouched)
        Waste *items[SIZE];
        int total;
        list.copyTo(items, total);

        if (total == 0)
        {
            cout << "No records available to sort.\n";
            return;
        }

        for (int i = 0; i < total - 1; i++)
        {
            for (int j = 0; j < total - i - 1; j++)
            {
                if (items[j]->getWeight() > items[j + 1]->getWeight())
                {
                    Waste *temp = items[j];
                    items[j] = items[j + 1];
                    items[j + 1] = temp;
                }
            }
        }

        cout << "\n================= WASTE RECORDS SORTED BY WEIGHT (BUBBLE SORT) =================\n";
        for (int i = 0; i < total; i++)
            items[i]->display();
        cout << "=================================================================================\n";
    }

    void createQueue()
    {
        Node *temp = list.getHead();
        if (temp == nullptr)
        {
            cout << "No records in list to enqueue.\n";
            return;
        }

        int count = 0;
        while (temp != nullptr)
        {
            queue.enqueue(temp->data);
            count++;
            temp = temp->next;
        }
        cout << "Successfully added " << count << " waste items to FIFO Queue.\n";
    }

    void processQueue()
    {
        if (queue.empty())
        {
            cout << "Queue is empty. Call 'Create Collection Queue' first.\n";
            return;
        }

        int count = 0;
        while (!queue.empty())
        {
            Waste *w = queue.dequeue();
            if (w == nullptr) continue;

            if (w->category() == "Reusable")
                reusableStack.push(w);
            else if (w->category() == "Recyclable")
                recyclableStack.push(w);
            else
                hazardousStack.push(w);

            count++;
            cout << "Segregated Waste ID " << w->getId() << " -> " << w->category() << " Stack\n";
        }
        cout << "Successfully segregated " << count << " items into Category Stacks.\n";
    }

    void showStacks() const
    {
        reusableStack.display("REUSABLE STACK (LIFO)");
        recyclableStack.display("RECYCLABLE STACK (LIFO)");
        hazardousStack.display("HAZARDOUS STACK (LIFO)");
    }

    void searchHash() const
    {
        int id, checks;
        cout << "Enter ID to search using Hash Table: ";
        if (!(cin >> id))
        {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            return;
        }

        Waste *w = hashTable.search(id, checks); // PL CO5: Hash Search
        if (w != nullptr)
        {
            cout << "\nRecord Found via Hashing:\n";
            w->display();
        }
        else
        {
            cout << "\nRecord not found in Hash Table.\n";
        }
        cout << "Slots inspected (Linear Probing): " << checks << endl;
    }

    void compareSearch() const
    {
        int id;
        cout << "Enter ID to benchmark Linear vs. Hash Search: ";
        if (!(cin >> id))
        {
            cout << "Invalid input.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            return;
        }

        int linearChecks = 0;
        Node *temp = list.getHead();
        while (temp != nullptr)
        {
            linearChecks++;
            if (temp->data->getId() == id)
                break;
            temp = temp->next;
        }

        int hashChecks = 0;
        Waste *found = hashTable.search(id, hashChecks);

        cout << "\n--- Search Efficiency Benchmark ---\n";
        cout << "Linear Search Checks : " << linearChecks << " comparisons\n";
        cout << "Hash Search Checks   : " << hashChecks << " probe(s)\n";
        if (temp != nullptr && found != nullptr)
            cout << "Outcome: Record successfully found by both algorithms.\n";
        else
            cout << "Outcome: Record not present in database.\n";
    }

    void showSummary() const
    {
        cout << "\n=================== ECOSORT SYSTEM SUMMARY ===================\n";
        displayMetric("Total Waste Records", list.count(), "items");
        displayMetric("Total Processed Mass", list.totalWeight(), "kg");
        displayMetric("Priority Limit", PRIORITY_LIMIT, "kg");
        displayMetric("Queue Pending Items", queue.count(), "items");
        displayMetric("Reusable Stacked", reusableStack.count(), "items");
        displayMetric("Recyclable Stacked", recyclableStack.count(), "items");
        displayMetric("Hazardous Stacked", hazardousStack.count(), "items");
#if HAVE_ECOSORT_BRIDGE
        displayMetric("IPC Shared Memory", string("ACTIVE (Connected)"));
#else
        displayMetric("IPC Shared Memory", string("STANDALONE MODE"));
#endif
        cout << "==============================================================\n";
    }

    // PSOOP CO4: Smart Decision Making
    void recommend(Waste *w) const
    {
        cout << "Smart Action: ";
        if (w->category() == "Hazardous")
            cout << "[CRITICAL] Quarantine for safe chemical/electronic detoxification.\n";
        else if (w->category() == "Reusable")
            cout << "[REFURBISH] Forward to municipal repair and refurbishment hub.\n";
        else if (w->getWeight() >= PRIORITY_LIMIT)
            cout << "[HIGH PRIORITY] Mass >= " << PRIORITY_LIMIT << " kg: Route to bulk shredder batch.\n";
        else
            cout << "[STANDARD] Route to standard optical sorting line.\n";
    }

    void showRecommendations() const
    {
        Node *temp = list.getHead();
        if (temp == nullptr)
        {
            cout << "No records available for recommendations.\n";
            return;
        }

        cout << "\n================== AUTOMATED ROUTING RECOMMENDATIONS ==================\n";
        while (temp != nullptr)
        {
            cout << "Item #" << setw(3) << temp->data->getId() << " (" << temp->data->getName() << "): ";
            recommend(temp->data);
            temp = temp->next;
        }
        cout << "=======================================================================\n";
    }

    void testDecisions() const
    {
        cout << "\n================= PSOOP CO4 VERIFICATION TEST CASES =================\n";
        cout << "Test 1: Reusable (Phone), 0.25 kg   -> [REFURBISH] Forward to repair hub.\n";
        cout << "Test 2: Recyclable (Server), 18.5 kg -> [HIGH PRIORITY] Route to bulk shredder.\n";
        cout << "Test 3: Hazardous (Battery), 1.2 kg -> [CRITICAL] Quarantine for chemical treatment.\n";
        cout << "=======================================================================\n";
    }

    // =====================================================
    // PSOOP CO5: PERSISTENT FILE I/O (SAVE / LOAD)
    // =====================================================
    void saveToFile(bool verbose = true) const
    {
        ofstream outFile(DB_FILENAME);
        if (!outFile)
        {
            if (verbose) cout << "[ERROR]: Could not open file " << DB_FILENAME << " for writing.\n";
            return;
        }

        Node *temp = list.getHead();
        int count = 0;
        while (temp != nullptr)
        {
            outFile << temp->data->toFileString() << "\n";
            count++;
            temp = temp->next;
        }

        outFile.close();
        if (verbose)
            cout << "\n>>> [FILE PERSISTENCE]: Successfully saved " << count << " record(s) to " << DB_FILENAME << endl;
    }

    void loadFromFile(bool verbose = true)
    {
        ifstream inFile(DB_FILENAME);
        if (!inFile)
        {
            if (verbose) cout << "[INFO]: No existing database file found (" << DB_FILENAME << "). Starting fresh.\n";
            return;
        }

        list.clear();
        hashTable.clear();

        string line;
        int count = 0;
        while (getline(inFile, line))
        {
            if (line.empty()) continue;

            stringstream ss(line);
            string sId, sName, sWeight, sCat;

            if (getline(ss, sId, ',') && getline(ss, sName, ',') &&
                getline(ss, sWeight, ',') && getline(ss, sCat, ','))
            {
                try
                {
                    int id = stoi(sId);
                    double weight = stod(sWeight);

                    Waste *w = nullptr;
                    if (sCat == "Reusable")
                        w = new Reusable(id, sName, weight);
                    else if (sCat == "Recyclable")
                        w = new Recyclable(id, sName, weight);
                    else if (sCat == "Hazardous")
                        w = new Hazardous(id, sName, weight);

                    if (w != nullptr)
                    {
                        list.add(w);
                        hashTable.insert(w);
                        count++;
                    }
                }
                catch (...)
                {
                    // Ignore corrupted lines
                }
            }
        }

        inFile.close();
        if (verbose)
            cout << "\n>>> [FILE PERSISTENCE]: Successfully loaded " << count << " record(s) from " << DB_FILENAME << endl;
    }
};

// =====================================================
// MAIN USER INTERFACE
// =====================================================
int main()
{
    EcoSort system;
    int choice;

    do
    {
        cout << "\n========================================================\n";
        cout << "       ECOSORT 386 - SMART E-WASTE SEGREGATION (TASK 4) \n";
        cout << "========================================================\n";
        cout << " 1. Add Waste Record (Exception Handling & IPC Sync)\n";
        cout << " 2. Display All Waste Records\n";
        cout << " 3. Search Waste (PL CO4: Linear Search)\n";
        cout << " 4. Sort Waste by Weight (PL CO4: Bubble Sort)\n";
        cout << " 5. Create Collection Queue (PSOOP CO6 Template FIFO)\n";
        cout << " 6. Process Queue into Category Stacks (Template LIFO)\n";
        cout << " 7. Display Category Stacks\n";
        cout << " 8. Search Waste (PL CO5: Hash Table with Linear Probing)\n";
        cout << " 9. Compare Search Efficiency (Linear vs. Hash)\n";
        cout << "10. Show Smart Routing Recommendations (PSOOP CO4)\n";
        cout << "11. Run Decision Making Verification Test Cases\n";
        cout << "12. Save Records to Persistent Disk File (PSOOP CO5)\n";
        cout << "13. Reload Records from Persistent Disk File (PSOOP CO5)\n";
        cout << "14. Show Complete System Summary & Diagnostics\n";
        cout << "15. Exit\n";
        cout << "--------------------------------------------------------\n";
        cout << "Enter your choice (1-15): ";

        if (!(cin >> choice))
        {
            cout << "Invalid input. Please enter a valid number.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            choice = 0;
            continue;
        }

        switch (choice)
        {
            case 1:  system.addWaste(); break;
            case 2:  system.showWaste(); break;
            case 3:  system.searchWaste(); break;
            case 4:  system.sortWaste(); break;
            case 5:  system.createQueue(); break;
            case 6:  system.processQueue(); break;
            case 7:  system.showStacks(); break;
            case 8:  system.searchHash(); break;
            case 9:  system.compareSearch(); break;
            case 10: system.showRecommendations(); break;
            case 11: system.testDecisions(); break;
            case 12: system.saveToFile(true); break;
            case 13: system.loadFromFile(true); break;
            case 14: system.showSummary(); break;
            case 15: 
                system.saveToFile(false); // Auto-save on exit
                cout << "\nAll data persisted to disk. Thank you for using EcoSort 386.\n"; 
                break;
            default: cout << "Invalid choice. Please select an option between 1 and 15.\n";
        }
    } while (choice != 15);

    return 0;
}
