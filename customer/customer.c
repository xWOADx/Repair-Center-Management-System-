#include "customer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Customer *customer_head = NULL;
static Customer *customer_tail = NULL;

Customer * find_customer (const int id) {
    Customer *temp = customer_head;

    while (temp != NULL) {
        if (temp->id == id) return temp;
        temp = temp->next;
    }
    return NULL;
}

bool add_customer (const int id, const char *name, const char *phone) {
    if (name == NULL || phone == NULL || name[0] == '\0' || phone[0] == '\0' || find_customer(id) != NULL)
        return false;

    if (strlen(name) >= sizeof(((Customer *)0)->name) || strlen(phone) >= sizeof(((Customer *)0)->phone))
        return false;

    Customer *item = malloc(sizeof(Customer));
    if (item == NULL)
        return false;

    item->id = id;
    strcpy(item->name, name);
    strcpy(item->phone, phone);
    item->next = NULL;

    if (customer_head == NULL) {
        customer_head = customer_tail = item;
    } else {
        customer_tail->next = item;
        customer_tail = item;
    }

    return true;
}

void print_customers (void) {
    if (customer_head == NULL) {
        printf("No Customers\n");
        return;
    }

    const Customer *temp = customer_head;

    printf("\n%-10s %-30s %-20s\n", "ID", "Name", "Phone");
    printf("--------------------------------------------------------------\n");

    while (temp != NULL) {
        printf("%-10d %-30s %-20s\n", temp->id, temp->name, temp->phone);
        temp = temp->next;
    }
}

void reset_customer_tail (void) {
    customer_tail = NULL;
}