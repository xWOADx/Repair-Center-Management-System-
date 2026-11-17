#ifndef TECHNICIAN_H
#define TECHNICIAN_H

#include <stdbool.h>

typedef union TechInfo {
    char phone[20];
    char national_id[20];
    char address[100];
} TechInfo;

typedef enum TechInfoType {
    TECH_PHONE,
    TECH_NATIONAL_ID,
    TECH_ADDRESS
} TechInfoType;

typedef struct Technician {
    int id;
    char name[50];
    TechInfoType info_type;
    TechInfo info;
    bool active;
    struct Technician *next;
} Technician;

extern Technician *technician_head;

Technician * find_technician(int id);
bool add_technician (int id, const char *name, TechInfoType info_type, const char *info);
bool delete_technician (int id);
void print_technicians (void);
void reset_technician_tail(void);
#endif