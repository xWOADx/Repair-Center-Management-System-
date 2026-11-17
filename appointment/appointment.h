#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <stdbool.h>
#include "../customer/customer.h"
#include "../technician/technician.h"
#include "../service/service.h"

typedef enum Month {
    JANUARY = 1,
    FEBRUARY,
    MARCH,
    APRIL,
    MAY,
    JUNE,
    JULY,
    AUGUST,
    SEPTEMBER,
    OCTOBER,
    NOVEMBER,
    DECEMBER
} Month;

typedef enum PaymentStatus {
    UNPAID,
    PAID
} PaymentStatus;

typedef struct Date {
    int year;
    int day;
    Month month;
} Date;

typedef struct Appointment {
    int id;
    Customer *customer;
    Technician *technician;
    Service *service;
    Date date;
    int hour;
    PaymentStatus status;
    struct Appointment *next;
} Appointment;

extern Appointment *appointment_head;

Appointment * find_appointment (int id);

bool make_appointment (int id, int customer_id, int technician_id, int service_id, Date date, int hour);

bool cancel_appointment (int id);

bool edit_appointment (int id, int technician_id, int service_id, Date date, int hour);

bool is_future_appointment (const Appointment *appointment);

void find_appointments_by_customer(int customer_id);
void find_appointments_by_technician(int technician_id);

bool process_payment(int id);
void print_receipt(const Appointment *appointment);
void print_appointments(void);
void sync_appointment_tail(void);

#endif
