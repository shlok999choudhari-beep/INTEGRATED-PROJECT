#include <iostream>
#include <string>
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

public:
    Waste(int i, string n, double w)
    {
        id = i;
        name = n;
        weight = w;
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

public:
    Queue()
    {
        front = 0;
        rear = -1;
    }

    bool empty()
    {
        return front > rear;
    }

    void enqueue(Waste *w)
    {
        if (rear == MAX - 1)
        {
            cout << "Queue is full.\n";
            return;
        }

        items[++rear] = w;
    }

    Waste* dequeue()
    {
        if (empty())
        {
            return NULL;
        }

        return items[front++];
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
        cin >> id;

        cout << "Enter Waste Name: ";
        cin >> name;

        cout << "Enter Waste Weight (kg): ";
        cin >> weight;

        if (weight <= 0)
        {
            cout << "Weight must be greater than 0.\n";
            return;
        }

        cout << "\n1. Reusable";
        cout << "\n2. Recyclable";
        cout << "\n3. Hazardous";

        cout << "\nEnter Category: ";
        cin >> choice;

        Waste *w = createWaste(id, name, weight, choice);

        if (w == NULL)
        {
            cout << "Invalid category.\n";
            return;
        }

        // STORE IN LINKED LIST
        collectionList.insert(w);

        // Sync with Cross-Process IPC (CGL Kiosk and COA Registers)
        int bridgeCat = (choice == 1 ? EcoSortCore::CAT_REUSABLE : (choice == 2 ? EcoSortCore::CAT_RECYCLABLE : EcoSortCore::CAT_HAZARDOUS));
        EcoSortCore::Bridge::get().setCategory(bridgeCat);
        EcoSortCore::Bridge::get().triggerDeposit((float)weight);

        cout << "\nCollection request added successfully.\n";
        cout << "Weight: " << w->getWeight() << " kg | Synced to CGL Kiosk Queue & COA Live Telemetry!\n";
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

    // MOVE LINKED LIST DATA TO QUEUE
    void createQueue()
    {
        Node *temp = collectionList.getHead();

        while (temp != NULL)
        {
            collectionQueue.enqueue(temp->data);
            temp = temp->next;
        }

        cout << "\nCollection requests moved to Queue (FIFO).\n";
    }

    // PROCESS QUEUE INTO CATEGORY STACKS
    void processWaste()
    {
        Waste *w;

        while (!collectionQueue.empty())
        {
            w = collectionQueue.dequeue();

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
        cout << "\n3. Search Waste";
        cout << "\n4. Create Collection Queue";
        cout << "\n5. Process and Segregate Waste";
        cout << "\n6. Display Sorted Waste";
        cout << "\n7. Show COA Input Data";
        cout << "\n8. Exit";

        cout << "\n\nEnter choice: ";
        cin >> choice;

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
            system.createQueue();
            break;
        case 5:
            system.processWaste();
            break;
        case 6:
            system.displaySortedWaste();
            break;
        case 7:
            system.showCOAData();
            break;
        case 8:
            cout << "\nThank you for using EcoSort 386.\n";
            break;
        default:
            cout << "\nInvalid choice.\n";
        }

    } while (choice != 8);

    return 0;
}
