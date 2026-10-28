#include <iostream>
#include <string>
#include <stdexcept>
#include "../ecosort_bridge.h"

using namespace std;

const int MAX = 20;

// ======================================================
// PSOOP : BASE CLASS
// ======================================================

class Waste
{
protected:
    int id;
    string name;
    double weight;
    bool queued;
    bool processed;

public:
    Waste(int i, string n, double w)
    {
        id = i;
        name = n;
        weight = w;
        queued = false;
        processed = false;
    }

    virtual string category() = 0;
    virtual int priority() = 0;

    int getId()
    {
        return id;
    }

    double getWeight()
    {
        return weight;
    }

    bool isQueued() const { return queued; }
    bool isProcessed() const { return processed; }
    void markQueued() { queued = true; }
    void markProcessed() { processed = true; }

    string getCategory()
    {
        return category();
    }

    void display()
    {
        cout << "ID: " << id
             << " | Name: " << name
             << " | Weight: " << weight << " kg"
             << " | Category: " << category()
             << " | Priority: " << priority()
             << endl;
    }

    virtual ~Waste() {}
};

// ======================================================
// PSOOP : INHERITANCE + POLYMORPHISM
// ======================================================

class Reusable : public Waste
{
public:
    Reusable(int i, string n, double w)
        : Waste(i, n, w)
    {
    }

    string category()
    {
        return "Reusable";
    }

    int priority()
    {
        return 1;
    }
};

class Recyclable : public Waste
{
public:
    Recyclable(int i, string n, double w)
        : Waste(i, n, w)
    {
    }

    string category()
    {
        return "Recyclable";
    }

    int priority()
    {
        return 2;
    }
};

class Hazardous : public Waste
{
public:
    Hazardous(int i, string n, double w)
        : Waste(i, n, w)
    {
    }

    string category()
    {
        return "Hazardous";
    }

    int priority()
    {
        return 3;
    }
};

// ======================================================
// PL CO2 : SINGLY LINKED LIST
// Collection requests are stored dynamically
// ======================================================

struct Node
{
    Waste *data;
    Node *next;
};

class WasteList
{
private:
    Node *head;

public:
    WasteList()
    {
        head = NULL;
    }

    // INSERT WASTE INTO LINKED LIST
    void insert(Waste *w)
    {
        Node *newNode = new Node;
        newNode->data = w;
        newNode->next = NULL;

        if (head == NULL)
        {
            head = newNode;
        }
        else
        {
            Node *temp = head;
            while (temp->next != NULL)
            {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    // DISPLAY COLLECTION REQUESTS
    void display()
    {
        Node *temp = head;

        cout << "\n===== COLLECTION REQUESTS =====\n";

        if (temp == NULL)
        {
            cout << "No collection requests.\n";
            return;
        }

        while (temp != NULL)
        {
            temp->data->display();
            temp = temp->next;
        }
    }

    // SEARCH
    Waste* search(int id)
    {
        Node *temp = head;

        while (temp != NULL)
        {
            if (temp->data->getId() == id)
            {
                return temp->data;
            }
            temp = temp->next;
        }

        return NULL;
    }

    // CALCULATE TOTAL WEIGHT
    double totalWeight()
    {
        double total = 0;
        Node *temp = head;

        while (temp != NULL)
        {
            total = total + temp->data->getWeight();
            temp = temp->next;
        }

        return total;
    }

    // COUNT WASTE ITEMS
    int count()
    {
        int total = 0;
        Node *temp = head;

        while (temp != NULL)
        {
            total++;
            temp = temp->next;
        }

        return total;
    }

    void copyTo(Waste *items[], int &total)
    {
        total = 0;
        Node *temp = head;
        while (temp != NULL && total < MAX)
        {
            items[total++] = temp->data;
            temp = temp->next;
        }
    }

    // ACCESS HEAD FOR QUEUE
    Node* getHead()
    {
        return head;
    }

    // DYNAMIC MEMORY CLEANUP
    ~WasteList()
    {
        Node *temp;

        while (head != NULL)
        {
            temp = head;
            head = head->next;
            delete temp->data;
            delete temp;
        }
    }
};

// ======================================================
// PL CO3 : QUEUE
// FIFO COLLECTION PROCESSING
// ======================================================

class Queue
{
private:
    Waste *items[MAX];
    int front;
    int rear;
    int itemCount;

public:
    Queue()
    {
        front = 0;
        rear = -1;
        itemCount = 0;
    }

    bool empty()
    {
        return itemCount == 0;
    }

    bool enqueue(Waste *w)
    {
        if (itemCount == MAX)
        {
            cout << "Queue is full.\n";
            return false;
        }

        // Circular queue reuses slots freed by dequeue operations.
        rear = (rear + 1) % MAX;
        items[rear] = w;
        itemCount++;
        return true;
    }

    Waste* dequeue()
    {
        if (empty())
        {
            return NULL;
        }

        Waste *item = items[front];
        front = (front + 1) % MAX;
        itemCount--;
        return item;
    }
};

// ======================================================
// PL CO3 : STACK
// LIFO CATEGORY STORAGE
// ======================================================

class Stack
{
private:
    Waste *items[MAX];
    int top;

public:
    Stack()
    {
        top = -1;
    }

    void push(Waste *w)
    {
        if (top == MAX - 1)
        {
            cout << "Stack is full.\n";
            return;
        }

        items[++top] = w;
    }

    void display(string title)
    {
        cout << "\n===== " << title << " =====\n";

        if (top == -1)
        {
            cout << "No items.\n";
            return;
        }

        for (int i = top; i >= 0; i--)
        {
            items[i]->display();
        }
    }
};

// ======================================================
// PL CO5 : HASH TABLE WITH LINEAR PROBING
// ======================================================

class HashTable
{
private:
    int ids[MAX];
    Waste *items[MAX];
    bool used[MAX];

public:
    HashTable()
    {
        for (int i = 0; i < MAX; ++i)
        {
            ids[i] = -1;
            items[i] = NULL;
            used[i] = false;
        }
    }

    void insert(Waste *w)
    {
        int index = w->getId() % MAX;
        for (int i = 0; i < MAX; ++i)
        {
            int place = (index + i) % MAX;
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

    Waste* search(int id, int &checks)
    {
        checks = 0;
        int index = id % MAX;
        for (int i = 0; i < MAX; ++i)
        {
            int place = (index + i) % MAX;
            ++checks;
            if (!used[place]) return NULL;
            if (ids[place] == id) return items[place];
        }
        return NULL;
    }
};

// ======================================================
// ECOSORT MANAGER
// ======================================================

class EcoSort
{
private:
    WasteList collectionList;
    Queue collectionQueue;
    Stack reusableStack;
    Stack recyclableStack;
    Stack hazardousStack;
    HashTable hashTable;

public:
    // CREATE APPROPRIATE OBJECT
    Waste* createWaste(int id, string name, double weight, int choice)
    {
        if (choice == 1)
        {
            return new Reusable(id, name, weight);
        }

        if (choice == 2)
        {
            return new Recyclable(id, name, weight);
        }

        if (choice == 3)
        {
            return new Hazardous(id, name, weight);
        }

        return NULL;
    }

    // ADD COLLECTION REQUEST
    void addWaste()
    {
        int id;
        int choice;
        string name;
        double weight;

        cout << "\nEnter Waste ID: ";
        if (!(cin >> id) || id <= 0)
        {
            cout << "ID must be a positive number.\n";
            cin.clear(); cin.ignore(10000, '\n');
            return;
        }

        cout << "Enter Waste Name: ";
        cin >> name;

        if (name.empty())
        {
            cout << "Name cannot be empty.\n";
            return;
        }

        cout << "Enter Waste Weight (kg): ";
        if (!(cin >> weight))
        {
            cout << "Weight must be a number.\n";
            cin.clear(); cin.ignore(10000, '\n');
            return;
        }

        if (weight <= 0)
        {
            cout << "Weight must be greater than 0.\n";
            return;
        }

        if (collectionList.search(id) != NULL)
        {
            cout << "This Waste ID already exists. Please use a unique ID.\n";
            return;
        }

        cout << "\n1. Reusable";
        cout << "\n2. Recyclable";
        cout << "\n3. Hazardous";

        cout << "\nEnter Category: ";
        if (!(cin >> choice))
        {
            cout << "Category must be a number.\n";
            cin.clear(); cin.ignore(10000, '\n');
            return;
        }

        Waste *w = createWaste(id, name, weight, choice);

        if (w == NULL)
        {
            cout << "Invalid category.\n";
            return;
        }

        // STORE IN LINKED LIST
        collectionList.insert(w);
        hashTable.insert(w);

        // Sync with Cross-Process IPC (CGL Kiosk and COA Registers)
        int bridgeCat = (choice == 1 ? EcoSortCore::CAT_REUSABLE : (choice == 2 ? EcoSortCore::CAT_RECYCLABLE : EcoSortCore::CAT_HAZARDOUS));
        EcoSortCore::Bridge::get().setCategory(bridgeCat);
        EcoSortCore::Bridge::get().triggerDeposit((float)weight);

        cout << "\nCollection request added successfully.\n";
        cout << "Weight: " << w->getWeight() << " kg | Synced to CGL Kiosk Queue & COA Live Telemetry!\n";
        cout << "Recommendation: ";
        recommend(w);
    }

    // DISPLAY LINKED LIST
    void showCollectionRequests()
    {
        collectionList.display();
    }

    // SEARCH LINKED LIST
    void searchWaste()
    {
        int id;

        cout << "\nEnter ID to search: ";
        cin >> id;

        Waste *result = collectionList.search(id);

        if (result != NULL)
        {
            cout << "\nWaste found:\n";
            result->display();
        }
        else
        {
            cout << "\nWaste not found.\n";
        }
    }

    void sortWaste()
    {
        Waste *items[MAX];
        int total;
        collectionList.copyTo(items, total);
        if (total == 0) { cout << "No records to sort.\n"; return; }

        for (int i = 0; i < total - 1; ++i)
            for (int j = 0; j < total - i - 1; ++j)
                if (items[j]->getWeight() > items[j + 1]->getWeight())
                {
                    Waste *temp = items[j];
                    items[j] = items[j + 1];
                    items[j + 1] = temp;
                }

        cout << "\n===== WASTE SORTED BY WEIGHT =====\n";
        for (int i = 0; i < total; ++i) items[i]->display();
    }

    void searchHash()
    {
        int id, checks;
        cout << "Enter ID to search using hash table: ";
        if (!(cin >> id)) { cout << "Please enter a number.\n"; cin.clear(); cin.ignore(10000, '\n'); return; }
        Waste *w = hashTable.search(id, checks);
        if (w != NULL) { cout << "Waste found using hash table:\n"; w->display(); }
        else cout << "Waste not found in hash table.\n";
        cout << "Hash slots checked: " << checks << endl;
    }

    void compareSearch()
    {
        int id, linearChecks = 0, hashChecks;
        cout << "Enter ID to compare searches: ";
        if (!(cin >> id)) { cout << "Please enter a number.\n"; cin.clear(); cin.ignore(10000, '\n'); return; }
        Node *temp = collectionList.getHead();
        while (temp != NULL) { ++linearChecks; if (temp->data->getId() == id) break; temp = temp->next; }
        Waste *found = hashTable.search(id, hashChecks);
        cout << "Linear search checks: " << linearChecks << "\nHash search checks: " << hashChecks << endl;
        cout << ((temp != NULL && found != NULL) ? "Both methods found the record.\n" : "Record was not found by both methods.\n");
    }

    void recommend(Waste *w)
    {
        if (w->getCategory() == "Hazardous") cout << "Send for special hazardous-waste handling.\n";
        else if (w->getCategory() == "Reusable") cout << "Check if it can be reused or repaired.\n";
        else if (w->getWeight() >= 10.0) cout << "Send to priority recycling batch.\n";
        else cout << "Add to normal recycling batch.\n";
    }

    void showRecommendations()
    {
        Node *temp = collectionList.getHead();
        if (temp == NULL) { cout << "No records available.\n"; return; }
        cout << "\n===== SMART RECOMMENDATIONS =====\n";
        while (temp != NULL) { cout << "Waste ID " << temp->data->getId() << ": "; recommend(temp->data); temp = temp->next; }
    }

    void testDecisions()
    {
        cout << "\n===== DECISION TEST CASES =====\n"
             << "Reusable, 3 kg: Check if it can be reused or repaired.\n"
             << "Recyclable, 12 kg: Send to priority recycling batch.\n"
             << "Hazardous, 2 kg: Send for special hazardous-waste handling.\n";
    }

    // MOVE LINKED LIST DATA TO QUEUE
    void createQueue()
    {
        Node *temp = collectionList.getHead();

        int added = 0;
        while (temp != NULL)
        {
            Waste *item = temp->data;
            if (!item->isQueued() && !item->isProcessed())
            {
                if (collectionQueue.enqueue(item))
                {
                    item->markQueued();
                    added++;
                }
            }
            temp = temp->next;
        }

        cout << "\n" << added << " new collection request(s) added to Queue (FIFO).\n";
    }

    // PROCESS QUEUE INTO CATEGORY STACKS
    void processWaste()
    {
        Waste *w;

        while (!collectionQueue.empty())
        {
            w = collectionQueue.dequeue();
            if (w == NULL) continue;

            int catId = (w->getCategory() == "Reusable" ? EcoSortCore::CAT_REUSABLE : (w->getCategory() == "Recyclable" ? EcoSortCore::CAT_RECYCLABLE : EcoSortCore::CAT_HAZARDOUS));
            EcoSortCore::Bridge::get().setCategory(catId);
            EcoSortCore::Bridge::get().completeDeposit();

            if (w->getCategory() == "Reusable")
            {
                reusableStack.push(w);
            }
            else if (w->getCategory() == "Recyclable")
            {
                recyclableStack.push(w);
            }
            else
            {
                hazardousStack.push(w);
            }
            w->markProcessed();
        }

        cout << "\nWaste successfully segregated into category Stacks.\n";
    }

    // DISPLAY STACKS
    void displaySortedWaste()
    {
        reusableStack.display("REUSABLE WASTE");
        recyclableStack.display("RECYCLABLE WASTE");
        hazardousStack.display("HAZARDOUS WASTE");
    }

    // ==================================================
    // COA INPUT DATA
    // ==================================================
    void showCOAData()
    {
        double total = collectionList.totalWeight();
        int count = collectionList.count();

        cout << "\n====================================";
        cout << "\n          COA INPUT DATA";
        cout << "\n====================================";
        cout << "\nTotal E-Waste Weight : " << total << " kg";
        cout << "\nNumber of Waste Types: " << count;
        cout << "\nRecycling Rate       : Rs 50/kg";
        cout << "\nPriority Limit       : 10 kg";
        cout << "\n====================================\n";
    }
};

// ======================================================
// MAIN
// ======================================================

int main()
{
    EcoSort system;
    int choice;

    do
    {
        cout << "\n\n================================";
        cout << "\n          ECOSORT 386";
        cout << "\n================================";

        cout << "\n1. Add Waste Collection Request";
        cout << "\n2. Display Collection Requests";
        cout << "\n3. Search Waste (Linear Search)";
        cout << "\n4. Sort Waste by Weight";
        cout << "\n5. Create Collection Queue";
        cout << "\n6. Process Queue and Segregate Waste";
        cout << "\n7. Display Waste Stacks";
        cout << "\n8. Search Waste (Hashing)";
        cout << "\n9. Compare Search Methods";
        cout << "\n10. Show Smart Recommendations";
        cout << "\n11. Run Decision Test Cases";
        cout << "\n12. Show Summary / COA Input Data";
        cout << "\n13. Exit";

        cout << "\n\nEnter choice: ";
        if (!(cin >> choice))
        {
            cout << "\nInvalid input. Enter a menu number.\n";
            cin.clear(); cin.ignore(10000, '\n');
            choice = 0;
            continue;
        }

        switch (choice)
        {
        case 1:
            system.addWaste();
            break;
        case 2:
            system.showCollectionRequests();
            break;
        case 3:
            system.searchWaste();
            break;
        case 4:
            system.sortWaste();
            break;
        case 5:
            system.createQueue();
            break;
        case 6:
            system.processWaste();
            break;
        case 7:
            system.displaySortedWaste();
            break;
        case 8:
            system.searchHash();
            break;
        case 9:
            system.compareSearch();
            break;
        case 10:
            system.showRecommendations();
            break;
        case 11:
            system.testDecisions();
            break;
        case 12:
            system.showCOAData();
            break;
        case 13:
            cout << "\nThank you for using EcoSort 386.\n";
            break;
        default:
            cout << "\nInvalid choice.\n";
        }

    } while (choice != 13);

    return 0;
}
