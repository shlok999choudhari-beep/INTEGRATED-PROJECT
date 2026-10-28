/*
    EcoSort 386 - Simple PL + PSOOP Integrated Project

    PSOOP CO1: Classes, inheritance and polymorphism
    PSOOP CO2: Use classes to keep the program organized
    PSOOP CO3: Handle wrong input and duplicate IDs
    PSOOP CO4: Make a decision using waste category and weight

    PL CO2: Singly linked list
    PL CO3: Queue and stacks
    PL CO4: Searching and sorting

    Compile:
    g++ -std=c++17 EcoSort.cpp -o EcoSort.exe
*/

#include <iostream>
#include <string>
#include <iomanip>
#include <stdexcept>

using namespace std;

const int SIZE = 50;
const double PRIORITY_LIMIT = 10.0;

// =====================================================
// PSOOP CO1: BASE CLASS
// =====================================================

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

    int getId() { return id; }
    string getName() { return name; }
    double getWeight() { return weight; }

    virtual void display()
    {
        cout << "ID: " << id
             << " | Name: " << name
             << " | Weight: " << weight << " kg"
             << " | Category: " << category()
             << " | Priority: " << priority() << endl;
    }

    virtual ~Waste() {}
};

// =====================================================
// PSOOP CO1: INHERITANCE
// =====================================================

class Reusable : public Waste
{
public:
    Reusable(int i, string n, double w) : Waste(i, n, w) {}
    string category() { return "Reusable"; }
    int priority() { return 1; }
};

class Recyclable : public Waste
{
public:
    Recyclable(int i, string n, double w) : Waste(i, n, w) {}
    string category() { return "Recyclable"; }
    int priority() { return 2; }
};

class Hazardous : public Waste
{
public:
    Hazardous(int i, string n, double w) : Waste(i, n, w) {}
    string category() { return "Hazardous"; }
    int priority() { return 3; }
};

// =====================================================
// PL CO2: SINGLY LINKED LIST
// =====================================================

struct Node
{
    Waste *data;
    Node *next;

    Node(Waste *w)
    {
        data = w;
        next = NULL;
    }
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

    bool hasID(int id)
    {
        Node *temp = head;
        while (temp != NULL)
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

        if (head == NULL)
            head = newNode;
        else
        {
            Node *temp = head;
            while (temp->next != NULL)
                temp = temp->next;
            temp->next = newNode;
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

    void display()
    {
        Node *temp = head;

        cout << "\n===== COLLECTION REQUESTS =====\n";
        if (temp == NULL)
        {
            cout << "No records found.\n";
            return;
        }

        while (temp != NULL)
        {
            temp->data->display();
            temp = temp->next;
        }
    }

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

    double totalWeight()
    {
        double total = 0;
        Node *temp = head;
        while (temp != NULL)
        {
            total += temp->data->getWeight();
            temp = temp->next;
        }
        return total;
    }

    // Used to copy record pointers for sorting and queue creation.
    void copyTo(Waste *items[], int &total)
    {
        total = 0;
        Node *temp = head;
        while (temp != NULL && total < SIZE)
        {
            items[total] = temp->data;
            total++;
            temp = temp->next;
        }
    }

    Node* getHead() { return head; }

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

// =====================================================
// PL CO3: QUEUE (FIFO)
// =====================================================

class Queue
{
private:
    Waste *items[SIZE];
    int front, rear;

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
        if (rear == SIZE - 1)
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

// =====================================================
// PL CO3: STACKS
// =====================================================

class Stack
{
private:
    Waste *items[SIZE];
    int top;

public:
    Stack()
    {
        top = -1;
    }

    void push(Waste *w)
    {
        if (top == SIZE - 1)
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

// =====================================================
// PSOOP CO2: ECOSORT MANAGER
// =====================================================

class EcoSort
{
private:
    WasteList list;
    Queue queue;
    Stack reusableStack, recyclableStack, hazardousStack;

    Waste* makeWaste(int id, string name, double weight, int choice)
    {
        // PSOOP CO3: Validation and exception handling.
        if (id <= 0)
            throw invalid_argument("ID must be greater than zero.");

        if (name == "")
            throw invalid_argument("Name cannot be empty.");

        if (weight <= 0)
            throw invalid_argument("Weight must be greater than zero.");

        if (choice < 1 || choice > 3)
            throw invalid_argument("Choose category 1, 2 or 3.");

        if (list.hasID(id))
            throw invalid_argument("This ID already exists.");

        if (choice == 1)
            return new Reusable(id, name, weight);
        else if (choice == 2)
            return new Recyclable(id, name, weight);
        else
            return new Hazardous(id, name, weight);
    }

public:
    void addWaste()
    {
        int id, choice;
        string name;
        double weight;

        try
        {
            cout << "Enter Waste ID: ";
            if (!(cin >> id))
                throw invalid_argument("ID must be a number.");

            cout << "Enter Waste Name (one word): ";
            cin >> name;

            cout << "Enter Weight (kg): ";
            if (!(cin >> weight))
                throw invalid_argument("Weight must be a number.");

            cout << "\n1. Reusable\n2. Recyclable\n3. Hazardous\n";
            cout << "Choose Category: ";
            if (!(cin >> choice))
                throw invalid_argument("Category must be a number.");

            Waste *w = makeWaste(id, name, weight, choice);
            list.add(w);

            cout << "\nWaste added successfully.\n";

            // PSOOP CO4: Decision-making feature.
            recommend(w);
        }
        catch (const exception &e)
        {
            cout << "\nInput Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }

    void showWaste()
    {
        list.display();
    }

    void searchWaste()
    {
        int id;
        cout << "Enter ID to search: ";
        if (!(cin >> id))
        {
            cout << "Please enter a number.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            return;
        }

        Waste *w = list.search(id); // PL CO4: Linear search.

        if (w != NULL)
        {
            cout << "Waste found:\n";
            w->display();
        }
        else
            cout << "Waste not found.\n";
    }

    void sortWaste()
    {
        // PL CO4: Bubble Sort using a temporary array of pointers.
        // Main waste records remain stored in the linked list.
        Waste *items[SIZE];
        int total;
        list.copyTo(items, total);

        if (total == 0)
        {
            cout << "No records to sort.\n";
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

        cout << "\n===== WASTE SORTED BY WEIGHT =====\n";
        for (int i = 0; i < total; i++)
            items[i]->display();
    }

    void createQueue()
    {
        Node *temp = list.getHead();
        if (temp == NULL)
        {
            cout << "No records to add to queue.\n";
            return;
        }

        while (temp != NULL)
        {
            queue.enqueue(temp->data);
            temp = temp->next;
        }
        cout << "Waste records added to queue in FIFO order.\n";
    }

    void processQueue()
    {
        if (queue.empty())
        {
            cout << "Queue is empty. Create queue first.\n";
            return;
        }

        while (!queue.empty())
        {
            Waste *w = queue.dequeue();

            if (w->category() == "Reusable")
                reusableStack.push(w);
            else if (w->category() == "Recyclable")
                recyclableStack.push(w);
            else
                hazardousStack.push(w);

            cout << "Processed waste ID: " << w->getId() << endl;
        }

        cout << "Waste sorted into category stacks.\n";
    }

    void showStacks()
    {
        reusableStack.display("REUSABLE WASTE");
        recyclableStack.display("RECYCLABLE WASTE");
        hazardousStack.display("HAZARDOUS WASTE");
    }

    void showSummary()
    {
        cout << "\n===== ECOSORT SUMMARY =====\n";
        cout << "Total waste records: " << list.count() << endl;
        cout << "Total weight: " << list.totalWeight() << " kg\n";
        cout << "Priority limit: " << PRIORITY_LIMIT << " kg\n";
    }

    void recommend(Waste *w)
    {
        cout << "Recommendation: ";

        if (w->category() == "Hazardous")
            cout << "Send for special hazardous-waste handling.\n";
        else if (w->category() == "Reusable")
            cout << "Check if it can be reused or repaired.\n";
        else if (w->getWeight() >= PRIORITY_LIMIT)
            cout << "Send to priority recycling batch.\n";
        else
            cout << "Add to normal recycling batch.\n";
    }

    void showRecommendations()
    {
        Node *temp = list.getHead();
        if (temp == NULL)
        {
            cout << "No records available.\n";
            return;
        }

        cout << "\n===== SMART RECOMMENDATIONS =====\n";
        while (temp != NULL)
        {
            cout << "Waste ID " << temp->data->getId() << ": ";
            recommend(temp->data);
            temp = temp->next;
        }
    }

    void testDecisions()
    {
        // PSOOP CO4: Three example inputs for decision-making.
        cout << "\n===== DECISION TEST CASES =====\n";
        cout << "Reusable, 3 kg: ";
        cout << "Check if it can be reused or repaired.\n";
        cout << "Recyclable, 12 kg: ";
        cout << "Send to priority recycling batch.\n";
        cout << "Hazardous, 2 kg: ";
        cout << "Send for special hazardous-waste handling.\n";
    }
};

// =====================================================
// MAIN MENU
// =====================================================

int main()
{
    EcoSort system;
    int choice;

    do
    {
        cout << "\n==============================\n";
        cout << "        ECOSORT 386\n";
        cout << "==============================\n";
        cout << "1. Add Waste\n";
        cout << "2. Display Waste Records\n";
        cout << "3. Search Waste (Linear Search)\n";
        cout << "4. Sort Waste by Weight\n";
        cout << "5. Create Collection Queue\n";
        cout << "6. Process Queue and Segregate Waste\n";
        cout << "7. Display Waste Stacks\n";
        cout << "8. Show Smart Recommendations\n";
        cout << "9. Run Decision Test Cases\n";
        cout << "10. Show Summary\n";
        cout << "11. Exit\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice))
        {
            cout << "Invalid input. Enter a menu number.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            choice = 0;
            continue;
        }

        switch (choice)
        {
            case 1: system.addWaste(); break;
            case 2: system.showWaste(); break;
            case 3: system.searchWaste(); break;
            case 4: system.sortWaste(); break;
            case 5: system.createQueue(); break;
            case 6: system.processQueue(); break;
            case 7: system.showStacks(); break;
            case 8: system.showRecommendations(); break;
            case 9: system.testDecisions(); break;
            case 10: system.showSummary(); break;
            case 11: cout << "Thank you for using EcoSort 386.\n"; break;
            default: cout << "Invalid choice. Select 1 to 11.\n";
        }
    } while (choice != 11);

    return 0;
}