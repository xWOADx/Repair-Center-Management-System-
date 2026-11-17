#ifndef CUSTOMER_H
#define CUSTOMER_H
#include <stdbool.h>

typedef struct Customer {
    int id;
    char name[50];
    char phone[20];
    struct Customer *next;
} Customer;

extern Customer *customer_head;

bool add_customer (int id, const char *name, const char *phone);
Customer * find_customer (int id);
void print_customers (void);
void reset_customer_tail(void);


#endif