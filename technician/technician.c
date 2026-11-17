#include "technician.h"
#include "../appointment/appointment.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Technician *technician_head = NULL;
static Technician *technician_tail = NULL;

Technician * find_technician (const int id) {
    Technician *temp = technician_head;

    while (temp != NULL) {
        if (temp->id == id)
            return temp;

        temp = temp->next;
    }

    return NULL;
}

bool add_technician (const int id, const char *name, const TechInfoType info_type, const char *info) {
    if (name == NULL || info == NULL ||
        name[0] == '\0' || info[0] == '\0' ||
        find_technician(id) != NULL)
        return false;

    if (strlen(name) >= sizeof(((Technician *)0)->name))
        return false;

    switch (info_type) {
        case TECH_PHONE:
            if (strlen(info) >= sizeof(((Technician *)0)->info.phone))
                return false;
            break;

        case TECH_NATIONAL_ID:
            if (strlen(info) >= sizeof(((Technician *)0)->info.national_id))
                return false;
            break;

        case TECH_ADDRESS:
            if (strlen(info) >= sizeof(((Technician *)0)->info.address))
                return false;
            break;

        default:
            return false;
    }

    Technician *item = malloc(sizeof(Technician));

    if (item == NULL)
        return false;

    item->id = id;
    item->info_type = info_type;
    item->active = true;
    item->next = NULL;

    strcpy(item->name, name);

    switch (info_type) {
        case TECH_PHONE:
            strcpy(item->info.phone, info);
            break;

        case TECH_NATIONAL_ID:
            strcpy(item->info.national_id, info);
            break;

        case TECH_ADDRESS:
            strcpy(item->info.address, info);
            break;

        default:
            free(item);
            return false;
    }

    if (technician_head == NULL) {
        technician_head = technician_tail = item;
    } else {
        technician_tail->next = item;
        technician_tail = item;
    }

    return true;
}

bool delete_technician (const int id) {
    Technician *technician = find_technician(id);

    if (technician == NULL || !technician->active)
        return false;

    Appointment *temp = appointment_head;

    while (temp != NULL) {
        if (temp->technician == technician &&
            temp->status == PAID &&
            is_future_appointment(temp))
            return false;

        temp = temp->next;
    }

    temp = appointment_head;

    while (temp != NULL) {
        Appointment *next = temp->next;

        if (temp->technician == technician &&
            temp->status == UNPAID) {
            if (!cancel_appointment(temp->id))
                return false;
        }

        temp = next;
    }

    technician->active = false;

    return true;
}

void print_technicians (void) {
    const Technician *temp = technician_head;
    int count = 0;

    printf("\n%-10s %-30s %-20s %s\n",
           "ID", "Name", "Info Type", "Information");

    printf("--------------------------------------------------------------------------------\n");

    while (temp != NULL) {
        if (temp->active) {
            const char *s;
            const char *t;

            switch (temp->info_type) {
                case TECH_PHONE:
                    s = temp->info.phone;
                    t = "Phone";
                    break;

                case TECH_NATIONAL_ID:
                    s = temp->info.national_id;
                    t = "National ID";
                    break;

                case TECH_ADDRESS:
                    s = temp->info.address;
                    t = "Address";
                    break;

                default:
                    s = "Unknown";
                    t = "Unknown";
                    break;
            }

            printf("%-10d %-30.30s %-20s %s\n",
                   temp->id, temp->name, t, s);

            count++;
        }

        temp = temp->next;
    }

    if (count == 0)
        printf("No active technicians found.\n");
}

void reset_technician_tail (void) {
    technician_tail = NULL;
}
