#ifndef SERVICE_H
#define SERVICE_H

#include <stdbool.h>

#define MAX_SERVICES 10

typedef struct Service {
    int id;
    char name[50];
    float price;
    bool active;
} Service;

extern Service services[MAX_SERVICES];
extern int service_count;

Service * find_service (int id);
bool add_service (int id, const char *name, float price);
bool delete_service (int id);
void print_services (void);

#endif
