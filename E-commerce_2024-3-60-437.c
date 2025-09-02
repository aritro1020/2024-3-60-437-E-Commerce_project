#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct Customer {
    int id;
    char name[50];
    struct Customer* next;
};

struct Product {
    int id;
    char name[50];
    float price;
    struct Product* left;
    struct Product* right;
};

struct Order {
    int order_id;
    int customer_id;
    int product_id;
    struct Order* next;
};

struct UndoStack {
    struct Order* order;
    struct UndoStack* next;
};

void addCustomer(struct Customer** head)
{
    struct Customer* newCustomer = (struct Customer*)malloc(sizeof(struct Customer));
    printf("Enter customer ID: ");
    scanf("%d", &newCustomer->id);
    printf("Enter customer name: ");
    scanf(" %[^\n]", newCustomer->name);
    newCustomer->next = *head;
    *head = newCustomer;
}

void displayCustomers(struct Customer* head)
{
    if (head==NULL)
    {
        printf("No customers found!\n");
        return;
    }
    struct Customer* temp = head;
    printf("Customers List:\n");
    while (temp != NULL)
    {
        printf("ID: %d, Name: %s\n", temp->id, temp->name);
        temp = temp->next;
    }
}

struct Product* createProduct(int id, char* name, float price)
{
    struct Product* newProduct = (struct Product*)malloc(sizeof(struct Product));
    newProduct->id = id;
    strcpy(newProduct->name, name);
    newProduct->price = price;
    newProduct->left = newProduct->right = NULL;
    return newProduct;
}

struct Product* insertProduct(struct Product* root) {
    int productID;
    struct Product* newProduct;
    printf("Enter product ID: ");
    scanf("%d", &productID);
    struct Product* temp = root;
    while (temp != NULL) {
        if (productID == temp->id) {
            printf("Product with ID %d already exists.\n", productID);
            return root;
        } else if (productID < temp->id) {
            temp = temp->left;
        } else {
            temp = temp->right;
        }
    }
    newProduct = (struct Product*)malloc(sizeof(struct Product));
    newProduct->id = productID;
    printf("");
    printf("Enter product name: ");
    scanf(" %[^\n]", newProduct->name);
    printf("Enter product price: ");
    scanf("%f", &newProduct->price);
    newProduct->left = newProduct->right = NULL;
    if (root == NULL)
    {
        return newProduct;
    }
    temp = root;
    struct Product* prev = NULL;
    while (temp != NULL)
    {
        prev = temp;
        if (newProduct->id < temp->id)
        {
            temp = temp->left;
        }
        else if (newProduct->id > temp->id)
        {
            temp = temp->right;
        }
    }
    if (newProduct->id < prev->id) {
        prev->left = newProduct;
    } else {
        prev->right = newProduct;
    }
    return root;
}

void displayProducts(struct Product* root)
{
    if (root == NULL)
    {
        return;
    }
    displayProducts(root->left);
        printf("Product ID: %d, Name: %s, Price: %.2f\n", root->id, root->name, root->price);
        displayProducts(root->right);

}

struct Order* createOrder(int order_id, int customer_id, int product_id)
{
    struct Order* newOrder = (struct Order*)malloc(sizeof(struct Order));
    newOrder->order_id = order_id;
    newOrder->customer_id = customer_id;
    newOrder->product_id = product_id;
    newOrder->next = NULL;
    return newOrder;
}

void pushUndoStack(struct UndoStack** stack, struct Order* order)
{
    struct UndoStack* newStackNode = (struct UndoStack*)malloc(sizeof(struct UndoStack));
    newStackNode->order = order;
    newStackNode->next = *stack;
    *stack = newStackNode;
}
void enqueueOrder(struct Order** front, struct Order** rear, struct Customer* customers, struct Product* products, struct UndoStack** undoStack) {
    struct Order* newOrder = (struct Order*)malloc(sizeof(struct Order));
    int customerID, productID;
    printf("Enter order ID: ");
    scanf("%d", &newOrder->order_id);
    printf("Enter customer ID for the order: ");
    scanf("%d", &customerID);
    struct Customer* tempCustomer = customers;
    int customerFound = 0;
    while (tempCustomer != NULL) {
        if (tempCustomer->id == customerID) {
            customerFound = 1;
            break;
        }
        tempCustomer = tempCustomer->next;
    }
    if (!customerFound) {
        printf("Invalid Customer ID. Order not placed.\n");
        free(newOrder);
        return;
    }
    printf("Enter product ID for the order: ");
    scanf("%d", &productID);
    struct Product* tempProduct = products;
    int productFound = 0;
    while (tempProduct != NULL) {
        if (tempProduct->id == productID) {
            productFound = 1;
            break;
        }
        if (productID < tempProduct->id) {
            tempProduct = tempProduct->left;
        } else {
            tempProduct = tempProduct->right;
        }
    }
    if (!productFound) {
        printf("Invalid Product ID. Order not placed.\n");
        free(newOrder);
        return;
    }
    newOrder->customer_id = customerID;
    newOrder->product_id = productID;
    newOrder->next = NULL;
    if (*rear == NULL)
    {
        *front = *rear = newOrder;
    }
    else
    {
        (*rear)->next = newOrder;
        *rear = newOrder;
    }
    printf("Order placed successfully.\n");
    pushUndoStack(undoStack, newOrder);
}
void processOrder(struct Order** front, struct Order** rear) {
    if (*front == NULL) {
        printf("No orders to process.\n");
        return;
    }
    struct Order* temp = *front;
    printf("Processing Order ID: %d, Customer ID: %d, Product ID: %d\n", temp->order_id, temp->customer_id, temp->product_id);
    *front = (*front)->next;
    if (*front == NULL) {
        *rear = NULL;
    }
    free(temp);
}
void undoLastAction(struct UndoStack** stack, struct Order** front, struct Order** rear) {
    if (*stack == NULL) {
        printf("No actions to undo.\n");
        return;
    }
    struct UndoStack* temp = *stack;
    struct Order* undoneOrder = temp->order;
    printf("Undoing Order ID: %d\n", undoneOrder->order_id);
    *stack = (*stack)->next;
    free(temp);

    struct Order* prev = NULL;
    struct Order* current = *front;
    while (current != NULL && current != undoneOrder) {
        prev = current;
        current = current->next;
    }
    if (current == NULL) {
        printf("Order not found.\n");
        return;
    }
    if (prev == NULL) {
        *front = current->next;
    } else {
        prev->next = current->next;
    }
    if (current == *rear) {
        *rear = prev;
    }
    free(current);
}
void removeCustomer(struct Customer** customers) {
    int customerID;
    printf("Enter customer ID to remove: ");
    scanf("%d", &customerID);
    struct Customer* temp = *customers;
    struct Customer* prev = NULL;
    while (temp != NULL) {
        if (temp->id == customerID) {
            if (prev == NULL) {
                *customers = temp->next;
            } else {
                prev->next = temp->next;
            }
            printf("Customer removed: ID %d, Name: %s\n", temp->id, temp->name);
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("Customer ID %d not found.\n", customerID);
}

struct Product* findMin(struct Product* root) {
    while (root && root->left != NULL) {
        root = root->left;
    }
    return root;
}

struct Product* removeProduct(struct Product* root, int productID) {
    if (root == NULL) {
        printf("Product ID %d not found.\n", productID);
        return NULL;
    }

    if (productID < root->id) {
        root->left = removeProduct(root->left, productID);
    } else if (productID > root->id) {
        root->right = removeProduct(root->right, productID);
    } else {
        if (root->left == NULL && root->right == NULL) {
            printf("Product removed: ID %d, Name: %s\n", root->id, root->name);
            free(root);
            return NULL;
        } else if (root->left == NULL) {
            struct Product* temp = root->right;
            printf("Product removed: ID %d, Name: %s\n", root->id, root->name);
            free(root);
            return temp;
        } else if (root->right == NULL) {
            struct Product* temp = root->left;
            printf("Product removed: ID %d, Name: %s\n", root->id, root->name);
            free(root);
            return temp;
        } else {
            struct Product* successor = findMin(root->right);
            root->id = successor->id;
            strcpy(root->name, successor->name);
            root->price = successor->price;
            root->right = removeProduct(root->right, successor->id);
        }
    }
    return root;
}

int main() {
    struct Customer* customers = NULL;
    struct Product* products = NULL;
    struct Order* front = NULL;
    struct Order* rear = NULL;
    struct UndoStack* undoStack = NULL;
    int choice=-4;
    printf("E-Commerce Order Processing System by 2024-1-60-278, 2024-1-60-156 and 2024-3-60-437\n");
    while (choice != 10) {
        printf("1. Add Customer\n");
        printf("2. Display Customers\n");
        printf("3. Add Product\n");
        printf("4. Display Products\n");
        printf("5. Place Order\n");
        printf("6. Process Order\n");
        printf("7. Undo Last Order\n");
        printf("8. Remove Customer:\n");
        printf("9. Remove Product:\n");
        printf("10. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                addCustomer(&customers);
                break;
            case 2:
                displayCustomers(customers);
                break;
            case 3:
                products = insertProduct(products);
                break;
            case 4:
                if (products == NULL)
                {
                printf("No products found!\n");
                }
                else
                {
               displayProducts(products);
                }
                break;
            case 5:
                enqueueOrder(&front, &rear, customers, products, &undoStack);
                break;
            case 6:
                processOrder(&front, &rear);
                break;
            case 7:
                undoLastAction(&undoStack, &front, &rear);
                break;
            case 8 :
                removeCustomer(&customers);
                break;
            case 9:
                int productID;
                printf("Enter product ID to remove: ");
                scanf("%d", &productID);
                products = removeProduct(products, productID);
                break;
            case 10:
                printf("Exiting the system.\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}
