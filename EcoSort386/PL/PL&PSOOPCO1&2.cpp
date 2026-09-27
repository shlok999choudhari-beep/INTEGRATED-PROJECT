#include <iostream>
#include <string>
using namespace std;

const int MAX = 100;

// ---------- PSOOP: Base Class ----------
class Waste
{
protected:
    int id;
    string name;

public:
    Waste(int i, string n)
    {
        id = i;
        name = n;
    }

    virtual string category() = 0;
    virtual int priority() = 0;

    int getId()
    {
        return id;
    }

    string getCategory()
    {
        return category();
    }

    void display()
    {
        cout << "ID: " << id
             << " | Name: " << name
             << " | Category: " << category()
             << " | Priority: " << priority() << endl;
    }

    virtual ~Waste() {}
};

// ---------- PSOOP: Inheritance + Polymorphism ----------
class Reusable : public Waste
{
public:
    Reusable(int i, string n) : Waste(i, n) {}

    string category() { return "Reusable"; }
    int priority() { return 1; }
};

class Recyclable : public Waste
{
public:
    Recyclable(int i, string n) : Waste(i, n) {}

    string category() { return "Recyclable"; }
    int priority() { return 2; }
};

class Hazardous : public Waste
{
public:
    Hazardous(int i, string n) : Waste(i, n) {}

    string category() { return "Hazardous"; }
    int priority() { return 3; }
};

// ---------- PL CO2: Singly Linked List ----------
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
                temp = temp->next;

            temp->next = newNode;
        }
    }

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

    Waste* search(int id)
    {
        Node *temp = head;

        while (temp != NULL)
        {
            if (temp->data->getId() == id)
                return temp->data;

            temp = temp->next;
        }

        return NULL;
    }

    Node* getHead()
    {
        return head;
    }
};

// ---------- PL CO3: Queue ----------
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
            return NULL;

        return items[front++];
    }
};

// ---------- PL CO3: Stack ----------
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
            items[i]->display();
    }
};

// ---------- EcoSort Manager ----------
class EcoSort
{
private:
    WasteList collectionList;
    Queue collectionQueue;

    Stack reusableStack;
    Stack recyclableStack;
    Stack hazardousStack;

public:

    // PSOOP CO2: modular object creation
    Waste* createWaste(int id, string name, int choice)
    {
        if (choice == 1)
            return new Reusable(id, name);

        if (choice == 2)
            return new Recyclable(id, name);

        if (choice == 3)
            return new Hazardous(id, name);

        return NULL;
    }

    // Add collection request to linked list
    void addWaste()
    {
        int id, choice;
        string name;

        cout << "\nEnter Waste ID: ";
        cin >> id;

        cout << "Enter Waste Name: ";
        cin >> name;

        cout << "\n1. Reusable";
        cout << "\n2. Recyclable";
        cout << "\n3. Hazardous";
        cout << "\nEnter Category: ";
        cin >> choice;

        Waste *w = createWaste(id, name, choice);

        if (w == NULL)
        {
            cout << "Invalid category.\n";
            return;
        }

        collectionList.insert(w);

        cout << "\nCollection request added.\n";
        cout << "Priority: " << w->priority() << endl;
    }

    void showCollectionRequests()
    {
        collectionList.display();
    }

    void searchWaste()
    {
        int id;

        cout << "\nEnter ID to search: ";
        cin >> id;

        Waste *result = collectionList.search(id);

        if (result != NULL)
            result->display();
        else
            cout << "Waste not found.\n";
    }

    // Move linked-list requests into FIFO queue
    void createQueue()
    {
        Node *temp = collectionList.getHead();

        while (temp != NULL)
        {
            collectionQueue.enqueue(temp->data);
            temp = temp->next;
        }

        cout << "\nRequests moved to Queue (FIFO).\n";
    }

    // Classify queue items into category stacks
    void processWaste()
    {
        Waste *w;

        while (!collectionQueue.empty())
        {
            w = collectionQueue.dequeue();

            if (w->getCategory() == "Reusable")
                reusableStack.push(w);
            else if (w->getCategory() == "Recyclable")
                recyclableStack.push(w);
            else
                hazardousStack.push(w);
        }

        cout << "\nWaste successfully segregated.\n";
    }

    void displaySortedWaste()
    {
        reusableStack.display("REUSABLE WASTE");
        recyclableStack.display("RECYCLABLE WASTE");
        hazardousStack.display("HAZARDOUS WASTE");
    }
};

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
        cout << "\n7. Exit";

        cout << "\n\nEnter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1: system.addWaste(); break;
        case 2: system.showCollectionRequests(); break;
        case 3: system.searchWaste(); break;
        case 4: system.createQueue(); break;
        case 5: system.processWaste(); break;
        case 6: system.displaySortedWaste(); break;
        case 7: cout << "\nThank you for using EcoSort 386.\n"; break;
        default: cout << "\nInvalid choice.\n";
        }

    } while (choice != 7);

    return 0;
}
