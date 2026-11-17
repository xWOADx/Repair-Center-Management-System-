#include "service.h"
#include "../appointment/appointment.h"
#include <stdio.h>
#include <string.h>

Service services[MAX_SERVICES];
int service_count = 0;

Service * find_service (const int id) {
    for (int i = 0; i < service_count; i++) {
        if (services[i].id == id)
            return &services[i];
    }

    return NULL;
}

bool add_service (const int id, const char *name, const float price) {
    if (name == NULL || name[0] == '\0' || price < 0 ||
        find_service(id) != NULL)
        return false;

    if (strlen(name) >= sizeof(((Service *)0)->name))
        return false;

    int index = -1;

    for (int i = 0; i < service_count; i++) {
        if (!services[i].active) {
            bool referenced = false;

            Appointment *temp = appointment_head;

            while (temp != NULL) {
                if (temp->service == &services[i]) {
                    referenced = true;
                    break;
                }

                temp = temp->next;
            }

            if (!referenced) {
                index = i;
                break;
            }
        }
    }

    if (index == -1) {
        if (service_count >= MAX_SERVICES)
            return false;

        index = service_count++;
    }

    Service *service = &services[index];

    service->id = id;
    strcpy(service->name, name);
    service->price = price;
    service->active = true;

    return true;
}

bool delete_service (const int id) {
    Service *service = find_service(id);

    if (service == NULL || !service->active)
        return false;

    Appointment *temp = appointment_head;

    while (temp != NULL) {
        if (temp->service == service &&
            temp->status == PAID &&
            is_future_appointment(temp))
            return false;

        temp = temp->next;
    }

    temp = appointment_head;

    while (temp != NULL) {
        Appointment *next = temp->next;

        if (temp->service == service &&
            temp->status == UNPAID) {
            if (!cancel_appointment(temp->id))
                return false;
        }

        temp = next;
    }

    service->active = false;

    return true;
}

void print_services (void) {
    int count = 0;

    for (int i = 0; i < service_count; i++) {
        if (services[i].active)
            count++;
    }

    if (count == 0) {
        printf("No active services.\n");
        return;
    }

    printf("\n%-10s %-30s %s\n", "ID", "Name", "Price (SAR)");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < service_count; i++) {
        if (services[i].active) {
            printf("%-10d %-30.30s %.2f\n",
                   services[i].id,
                   services[i].name,
                   services[i].price);
        }
    }
}
