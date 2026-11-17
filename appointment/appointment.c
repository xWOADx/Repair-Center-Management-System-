#include "appointment.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Appointment *appointment_head = NULL;
static Appointment *appointment_tail = NULL;

Appointment * find_appointment (const int id) {
    Appointment *temp = appointment_head;

    while (temp != NULL) {
        if (temp->id == id)
            return temp;

        temp = temp->next;
    }

    return NULL;
}

static bool isValidDate (const Date date, const int hour) {
    if (date.year < 1 || date.year > 9999)
        return false;

    if (date.month < JANUARY || date.month > DECEMBER)
        return false;

    if (hour < 0 || hour > 23)
        return false;

    static const int days_in_month[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    int max_day = days_in_month[(int)date.month - 1];

    if (date.month == FEBRUARY) {
        bool leap_year =
            date.year % 400 == 0 ||
            (date.year % 4 == 0 && date.year % 100 != 0);

        if (leap_year)
            max_day = 29;
    }

    return date.day >= 1 && date.day <= max_day;
}

bool is_future_appointment (const Appointment *appointment) {
    if (appointment == NULL)
        return false;

    time_t now = time(NULL);

    if (now == (time_t)-1)
        return true;

    struct tm *current = localtime(&now);

    if (current == NULL)
        return true;

    Date date = appointment->date;

    int year = current->tm_year + 1900;
    int month = current->tm_mon + 1;
    int day = current->tm_mday;
    int hour = current->tm_hour;

    if (date.year != year)
        return date.year > year;

    if ((int)date.month != month)
        return (int)date.month > month;

    if (date.day != day)
        return date.day > day;

    return appointment->hour > hour;
}

bool make_appointment (const int id, const int customer_id, const int technician_id, const int service_id, const Date date, const int hour) {
    if (find_appointment(id) != NULL || !isValidDate(date, hour))
        return false;

    Customer *customer = find_customer(customer_id);
    Technician *technician = find_technician(technician_id);
    Service *service = find_service(service_id);

    if (customer == NULL || technician == NULL || service == NULL)
        return false;

    if (!technician->active || !service->active)
        return false;

    Appointment *temp = appointment_head;

    while (temp != NULL) {
        if (temp->technician == technician &&
            temp->date.year == date.year &&
            temp->date.month == date.month &&
            temp->date.day == date.day &&
            temp->hour == hour)
            return false;

        temp = temp->next;
    }

    Appointment *appointment = malloc(sizeof(Appointment));

    if (appointment == NULL)
        return false;

    appointment->id = id;
    appointment->customer = customer;
    appointment->technician = technician;
    appointment->service = service;
    appointment->date = date;
    appointment->hour = hour;
    appointment->status = UNPAID;
    appointment->next = NULL;

    if (appointment_head == NULL) {
        appointment_head = appointment_tail = appointment;
    } else {
        appointment_tail->next = appointment;
        appointment_tail = appointment;
    }

    return true;
}

bool cancel_appointment (const int id) {
    Appointment *previous = NULL;
    Appointment *current = appointment_head;

    while (current != NULL && current->id != id) {
        previous = current;
        current = current->next;
    }

    if (current == NULL || current->status == PAID)
        return false;

    if (previous == NULL)
        appointment_head = current->next;
    else
        previous->next = current->next;

    if (appointment_tail == current)
        appointment_tail = previous;

    free(current);
    return true;
}

bool edit_appointment (const int id, const int technician_id, const int service_id, const Date date, const int hour) {
    Appointment *appointment = find_appointment(id);

    if (appointment == NULL || appointment->status == PAID)
        return false;

    if (!isValidDate(date, hour))
        return false;

    Technician *technician = find_technician(technician_id);
    Service *service = find_service(service_id);

    if (technician == NULL || service == NULL)
        return false;

    if (!technician->active || !service->active)
        return false;

    Appointment *temp = appointment_head;

    while (temp != NULL) {
        if (temp != appointment &&
            temp->technician == technician &&
            temp->date.year == date.year &&
            temp->date.month == date.month &&
            temp->date.day == date.day &&
            temp->hour == hour)
            return false;

        temp = temp->next;
    }

    appointment->technician = technician;
    appointment->service = service;
    appointment->date = date;
    appointment->hour = hour;

    return true;
}

void find_appointments_by_customer (const int customer_id) {
    Customer *customer = find_customer(customer_id);

    if (customer == NULL) {
        printf("Customer not found.\n");
        return;
    }

    Appointment *temp = appointment_head;
    int count = 0;

    printf("\nAppointments for Customer: %s (ID: %d)\n",
           customer->name, customer->id);

    printf("%-8s %-22s %-22s %-12s %-12s %-8s %-10s\n",
           "App ID", "Technician", "Service", "Price (SAR)",
           "Date", "Hour", "Status");

    printf("------------------------------------------------------------------------------------------------------\n");

    while (temp != NULL) {
        if (temp->customer == customer) {
            printf("%-8d %-22.22s %-22.22s %-12.2f "
                   "%04d-%02d-%02d   %02d:00    %-10s\n",
                   temp->id,
                   temp->technician->name,
                   temp->service->name,
                   temp->service->price,
                   temp->date.year,
                   (int)temp->date.month,
                   temp->date.day,
                   temp->hour,
                   temp->status == PAID ? "Paid" : "Unpaid");

            count++;
        }

        temp = temp->next;
    }

    if (count == 0)
        printf("No appointments found for this customer.\n");
}

void find_appointments_by_technician (const int technician_id) {
    Technician *technician = find_technician(technician_id);

    if (technician == NULL) {
        printf("Technician not found.\n");
        return;
    }

    Appointment *temp = appointment_head;
    int count = 0;

    printf("\nAppointments for Technician: %s (ID: %d)\n",
           technician->name, technician->id);

    printf("%-8s %-22s %-22s %-12s %-12s %-8s %-10s\n",
           "App ID", "Customer", "Service", "Price (SAR)",
           "Date", "Hour", "Status");

    printf("------------------------------------------------------------------------------------------------------\n");

    while (temp != NULL) {
        if (temp->technician == technician) {
            printf("%-8d %-22.22s %-22.22s %-12.2f "
                   "%04d-%02d-%02d   %02d:00    %-10s\n",
                   temp->id,
                   temp->customer->name,
                   temp->service->name,
                   temp->service->price,
                   temp->date.year,
                   (int)temp->date.month,
                   temp->date.day,
                   temp->hour,
                   temp->status == PAID ? "Paid" : "Unpaid");

            count++;
        }

        temp = temp->next;
    }

    if (count == 0)
        printf("No appointments found for this technician.\n");
}

bool process_payment (const int id) {
    Appointment *appointment = find_appointment(id);

    if (appointment == NULL || appointment->status == PAID)
        return false;

    appointment->status = PAID;

    print_receipt(appointment);
    return true;
}

void print_receipt (const Appointment *appointment) {
    if (appointment == NULL)
        return;

    printf("\n==================================================\n");
    printf("              REPAIR CENTER RECEIPT\n");
    printf("==================================================\n");

    printf("%-22s %d\n",
           "Appointment ID:", appointment->id);

    printf("%-22s %d\n",
           "Customer ID:", appointment->customer->id);

    printf("%-22s %s\n",
           "Customer Name:", appointment->customer->name);

    printf("%-22s %s\n",
           "Technician:", appointment->technician->name);

    printf("%-22s %s\n",
           "Service:", appointment->service->name);

    printf("%-22s %04d-%02d-%02d\n",
           "Date:",
           appointment->date.year,
           (int)appointment->date.month,
           appointment->date.day);

    printf("%-22s %02d:00\n",
           "Time:", appointment->hour);

    printf("--------------------------------------------------\n");

    printf("%-22s %.2f SAR\n",
           "Total Amount:", appointment->service->price);

    printf("%-22s %s\n",
           "Payment Status:",
           appointment->status == PAID ? "PAID" : "UNPAID");

    printf("==================================================\n");
    printf("              Thank you for visiting!\n");
    printf("==================================================\n");
}

void print_appointments (void) {
    if (appointment_head == NULL) {
        printf("\nNo appointments found.\n");
        return;
    }

    printf("\n================================ ALL APPOINTMENTS ================================\n");

    printf("%-8s %-18s %-18s %-20s %-12s %-12s %-7s %-10s\n",
           "App ID", "Customer", "Technician", "Service",
           "Price (SAR)", "Date", "Hour", "Status");

    printf("----------------------------------------------------------------------------------------------------------------\n");

    Appointment *temp = appointment_head;

    while (temp != NULL) {
        printf("%-8d %-18.18s %-18.18s %-20.20s %-12.2f "
               "%04d-%02d-%02d   %02d:00   %-10s\n",
               temp->id,
               temp->customer->name,
               temp->technician->name,
               temp->service->name,
               temp->service->price,
               temp->date.year,
               (int)temp->date.month,
               temp->date.day,
               temp->hour,
               temp->status == PAID ? "PAID" : "UNPAID");

        temp = temp->next;
    }

    printf("----------------------------------------------------------------------------------------------------------------\n");
}

void sync_appointment_tail (void) {
    appointment_tail = appointment_head;

    if (appointment_tail == NULL)
        return;

    while (appointment_tail->next != NULL)
        appointment_tail = appointment_tail->next;
}
