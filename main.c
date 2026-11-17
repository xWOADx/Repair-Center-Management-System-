#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "customer/customer.h"
#include "technician/technician.h"
#include "service/service.h"
#include "appointment/appointment.h"

static void print_menu (void);

static void load_data (void);
static void save_data (void);
static void free_all_data (void);

static void handle_add_customer (void);
static void handle_add_technician (void);
static void handle_add_service (void);

static void handle_make_appointment (void);
static void handle_cancel_appointment (void);
static void handle_edit_appointment (void);

static void handle_delete_service (void);
static void handle_delete_technician (void);

static void handle_find_appointments_by_customer (void);
static void handle_find_appointments_by_technician (void);

static void handle_process_payment (void);
static void handle_print_all_records (void);

static int readInt (int *n, const char *prompt, const char *error);
static int readLine (char *s, const int maxSize, const char *prompt);

int main (void) {
    int choice;

    load_data();

    while (1) {
        print_menu();

        if (!readInt(&choice, "", "[!] Invalid choice. Enter an integer.")) {
            save_data();
            free_all_data();
            return EXIT_SUCCESS;
        }

        switch (choice) {
            case 1:
                load_data();
                break;

            case 2:
                save_data();
                free_all_data();
                return EXIT_SUCCESS;

            case 3:
                handle_add_customer();
                break;

            case 4:
                handle_add_technician();
                break;

            case 5:
                handle_add_service();
                break;

            case 6:
                handle_make_appointment();
                break;

            case 7:
                handle_cancel_appointment();
                break;

            case 8:
                handle_edit_appointment();
                break;

            case 9:
                handle_delete_service();
                break;

            case 10:
                handle_delete_technician();
                break;

            case 11:
                handle_find_appointments_by_customer();
                break;

            case 12:
                handle_find_appointments_by_technician();
                break;

            case 13:
                handle_process_payment();
                break;

            case 14:
                handle_print_all_records();
                break;

            default:
                printf("[!] Invalid choice. Choose a number from 1 to 14.\n");
                break;
        }
    }
}

static void print_menu (void) {
    printf("\n==================================================\n");
    printf("    MOBILE PHONE & TABLET REPAIR CENTER SYSTEM\n");
    printf("==================================================\n");
    printf("  1. Load Data\n");
    printf("  2. Save and Exit\n");
    printf("  3. Add Customer\n");
    printf("  4. Add Technician\n");
    printf("  5. Add Service\n");
    printf("  6. Make Appointment\n");
    printf("  7. Cancel Appointment\n");
    printf("  8. Edit Appointment\n");
    printf("  9. Delete Service\n");
    printf(" 10. Delete Technician\n");
    printf(" 11. Find Appointments by Customer\n");
    printf(" 12. Find Appointments by Technician\n");
    printf(" 13. Process Payment & Print Receipt\n");
    printf(" 14. Print All Records\n");
    printf("==================================================\n");
    printf("Enter your choice (1-14): ");
}

static void load_data (void) {
    FILE *file;
    char line[512];

    free_all_data();

    file = fopen("customers.txt", "r");

    if (file != NULL) {
        int id;
        char name[50];
        char phone[20];

        while (fgets(line, sizeof(line), file) != NULL) {
            if (sscanf(line, "%d|%49[^|]|%19[^\r\n]",
                       &id, name, phone) == 3) {
                add_customer(id, name, phone);
            }
        }

        fclose(file);
    }

    file = fopen("technicians.txt", "r");

    if (file != NULL) {
        int id, type, active;
        char name[50];
        char info[100];

        while (fgets(line, sizeof(line), file) != NULL) {
            if (sscanf(line, "%d|%49[^|]|%d|%99[^|]|%d",
                       &id, name, &type, info, &active) != 5)
                continue;

            if (type < TECH_PHONE || type > TECH_ADDRESS)
                continue;

            if (active != 0 && active != 1)
                continue;

            if (add_technician(id, name, (TechInfoType)type, info)) {
                Technician *technician = find_technician(id);
                technician->active = active != 0;
            }
        }

        fclose(file);
    }

    file = fopen("services.txt", "r");

    if (file != NULL) {
        int id, active;
        char name[50];
        float price;

        while (fgets(line, sizeof(line), file) != NULL) {
            if (sscanf(line, "%d|%49[^|]|%f|%d",
                       &id, name, &price, &active) != 4)
                continue;

            if (active != 0 && active != 1)
                continue;

            if (service_count >= MAX_SERVICES)
                break;

            if (find_service(id) != NULL)
                continue;

            Service *service = &services[service_count++];

            service->id = id;
            strcpy(service->name, name);
            service->price = price;
            service->active = active != 0;
        }

        fclose(file);
    }

    file = fopen("appointments.txt", "r");

    if (file != NULL) {
        int id, customer_id, technician_id, service_id;
        int year, month, day, hour, status;

        Appointment **tail = &appointment_head;

        while (fgets(line, sizeof(line), file) != NULL) {
            if (sscanf(line, "%d|%d|%d|%d|%d|%d|%d|%d|%d",
                       &id, &customer_id, &technician_id,
                       &service_id, &year, &month, &day,
                       &hour, &status) != 9)
                continue;

            if (find_appointment(id) != NULL)
                continue;

            Customer *customer = find_customer(customer_id);
            Technician *technician = find_technician(technician_id);
            Service *service = find_service(service_id);

            if (customer == NULL || technician == NULL || service == NULL)
                continue;

            if (year < 1 || year > 9999 ||
                month < 1 || month > 12 ||
                day < 1 || day > 31 ||
                hour < 0 || hour > 23 ||
                (status != UNPAID && status != PAID))
                continue;

            Appointment *appointment = malloc(sizeof(Appointment));

            if (appointment == NULL) {
                printf("[!] Memory allocation failed while loading appointments.\n");
                break;
            }

            appointment->id = id;
            appointment->customer = customer;
            appointment->technician = technician;
            appointment->service = service;

            appointment->date.year = year;
            appointment->date.month = (Month)month;
            appointment->date.day = day;

            appointment->hour = hour;
            appointment->status = (PaymentStatus)status;
            appointment->next = NULL;

            *tail = appointment;
            tail = &appointment->next;
        }

        fclose(file);
    }

    sync_appointment_tail();

    printf("Data loading completed.\n");
}

static void save_data (void) {
    FILE *file;

    file = fopen("customers.txt", "w");

    if (file == NULL) {
        printf("[!] Failed to open customers.txt.\n");
        return;
    }

    for (Customer *customer = customer_head;
         customer != NULL;
         customer = customer->next) {

        fprintf(file, "%d|%s|%s\n",
                customer->id,
                customer->name,
                customer->phone);
    }

    if (fclose(file) != 0) {
        printf("[!] Failed to save customers.txt.\n");
        return;
    }

    file = fopen("technicians.txt", "w");

    if (file == NULL) {
        printf("[!] Failed to open technicians.txt.\n");
        return;
    }

    for (Technician *technician = technician_head;
         technician != NULL;
         technician = technician->next) {

        const char *info;

        switch (technician->info_type) {
            case TECH_PHONE:
                info = technician->info.phone;
                break;

            case TECH_NATIONAL_ID:
                info = technician->info.national_id;
                break;

            case TECH_ADDRESS:
                info = technician->info.address;
                break;

            default:
                info = "";
                break;
        }

        fprintf(file, "%d|%s|%d|%s|%d\n",
                technician->id,
                technician->name,
                (int)technician->info_type,
                info,
                technician->active ? 1 : 0);
    }

    if (fclose(file) != 0) {
        printf("[!] Failed to save technicians.txt.\n");
        return;
    }

    file = fopen("services.txt", "w");

    if (file == NULL) {
        printf("[!] Failed to open services.txt.\n");
        return;
    }

    for (int i = 0; i < service_count; i++) {
        fprintf(file, "%d|%s|%.2f|%d\n",
                services[i].id,
                services[i].name,
                services[i].price,
                services[i].active ? 1 : 0);
    }

    if (fclose(file) != 0) {
        printf("[!] Failed to save services.txt.\n");
        return;
    }

    file = fopen("appointments.txt", "w");

    if (file == NULL) {
        printf("[!] Failed to open appointments.txt.\n");
        return;
    }

    for (Appointment *appointment = appointment_head;
         appointment != NULL;
         appointment = appointment->next) {

        fprintf(file, "%d|%d|%d|%d|%d|%d|%d|%d|%d\n",
                appointment->id,
                appointment->customer->id,
                appointment->technician->id,
                appointment->service->id,
                appointment->date.year,
                (int)appointment->date.month,
                appointment->date.day,
                appointment->hour,
                (int)appointment->status);
    }

    if (fclose(file) != 0) {
        printf("[!] Failed to save appointments.txt.\n");
        return;
    }

    printf("Data saved successfully.\n");
}

static void free_all_data (void) {
    while (appointment_head != NULL) {
        Appointment *temp = appointment_head;
        appointment_head = appointment_head->next;
        free(temp);
    }

    while (customer_head != NULL) {
        Customer *temp = customer_head;
        customer_head = customer_head->next;
        free(temp);
    }

    while (technician_head != NULL) {
        Technician *temp = technician_head;
        technician_head = technician_head->next;
        free(temp);
    }

    service_count = 0;

    reset_customer_tail();
    reset_technician_tail();
    sync_appointment_tail();
}

static int readInt (int *n, const char *prompt, const char *error) {
    int c;

    while (1) {
        printf("%s ", prompt);

        const int result = scanf("%d", n);
        if (result == EOF) return 0;

        while ((c = getchar()) != '\n' && c != EOF) {}

        if (result == 1) return 1;

        printf("%s\n", error);

        if (c == EOF) return 0;
    }
}

static int readLine (char *s, const int maxSize, const char *prompt) {
    while (1) {
        printf("%s ", prompt);

        if (fgets(s, maxSize, stdin) == NULL)
            return 0;

        s[strcspn(s, "\n")] = '\0';

        if (s[0] != '\0')
            return 1;

        printf("Empty is not allowed\n");
    }
}

static void handle_add_customer (void) {
    int id;
    char name[50];
    char phone[20];

    if (!readInt(&id, "Enter Customer ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (!readLine(name, sizeof(name), "Enter Customer Name:"))
        return;

    if (!readLine(phone, sizeof(phone), "Enter Phone Number:"))
        return;

    if (add_customer(id, name, phone))
        printf("Customer added successfully.\n");
    else
        printf("Failed to add customer.\n");

}

static void handle_add_technician (void) {
    int id;
    int type;
    char name[50];
    char info[100];

    if (!readInt(&id, "Enter Technician ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (!readLine(name, sizeof(name), "Enter Technician Name:"))
        return;

    printf("\nTechnician Information Type:\n");
    printf("0. Phone Number\n");
    printf("1. National ID\n");
    printf("2. Address\n");

    do {
        if (!readInt(&type, "Enter Information Type (0-2):",
                     "[!] Invalid input. Enter an integer."))
            return;

        if (type < TECH_PHONE || type > TECH_ADDRESS)
            printf("[!] Invalid type. Choose 0, 1, or 2.\n");

    } while (type < TECH_PHONE || type > TECH_ADDRESS);

    switch (type) {
        case TECH_PHONE:
            if (!readLine(info, 20, "Enter Phone Number:"))
                return;
            break;

        case TECH_NATIONAL_ID:
            if (!readLine(info, 20, "Enter National ID:"))
                return;
            break;

        case TECH_ADDRESS:
            if (!readLine(info, sizeof(info), "Enter Address:"))
                return;
            break;
    }

    if (add_technician(id, name, (TechInfoType)type, info))
        printf("Technician added successfully.\n");
    else
        printf("Failed to add technician.\n");
}

static void handle_add_service (void) {
    int id;
    char name[50];
    float price;

    if (!readInt(&id, "Enter Service ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (!readLine(name, sizeof(name), "Enter Service Name:"))
        return;

    while (1) {
        printf("Enter Service Price: ");

        int result = scanf("%f", &price);
        int c;

        if (result == EOF)
            return;

        while ((c = getchar()) != '\n' && c != EOF) {}

        if (result == 1 && price >= 0)
            break;

        printf("[!] Invalid price. Enter a non-negative number.\n");

        if (c == EOF)
            return;
    }

    if (add_service(id, name, price))
        printf("Service added successfully.\n");
    else
        printf("Failed to add service.\n");
}

static void handle_make_appointment (void) {
    int id;
    int customer_id;
    int technician_id;
    int service_id;
    int year, month, day, hour;
    Date date;

    if (!readInt(&id, "Enter Appointment ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (!readInt(&customer_id, "Enter Customer ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (!readInt(&technician_id, "Enter Technician ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (!readInt(&service_id, "Enter Service ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (!readInt(&year, "Enter Year:", "[!] Invalid year. Enter an integer."))
        return;

    if (!readInt(&month, "Enter Month (1-12):", "[!] Invalid month. Enter an integer."))
        return;

    if (!readInt(&day, "Enter Day:", "[!] Invalid day. Enter an integer."))
        return;

    if (!readInt(&hour, "Enter Hour (0-23):", "[!] Invalid hour. Enter an integer."))
        return;

    date.year = year;
    date.month = (Month)month;
    date.day = day;

    if (make_appointment(id, customer_id, technician_id, service_id, date, hour)) {
        printf("Appointment created successfully.\n");
    } else {
        printf("Failed to create appointment.\n");
    }
}

static void handle_cancel_appointment(void) {
    int id;

    if (!readInt(&id, "Enter Appointment ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (cancel_appointment(id)) {
        printf("Appointment cancelled successfully.\n");
    } else {
        printf("Failed to cancel appointment.\n");
    }
}

static void handle_edit_appointment (void) {
    int id;
    int technician_id;
    int service_id;
    int year, month, day, hour;
    Date date;

    if (!readInt(&id, "Enter Appointment ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (!readInt(&technician_id, "Enter New Technician ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (!readInt(&service_id, "Enter New Service ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (!readInt(&year, "Enter New Year:", "[!] Invalid year. Enter an integer."))
        return;

    if (!readInt(&month, "Enter New Month (1-12):", "[!] Invalid month. Enter an integer."))
        return;

    if (!readInt(&day, "Enter New Day:", "[!] Invalid day. Enter an integer."))
        return;

    if (!readInt(&hour, "Enter New Hour (0-23):", "[!] Invalid hour. Enter an integer."))
        return;

    date.year = year;
    date.month = (Month)month;
    date.day = day;

    if (edit_appointment(id, technician_id, service_id, date, hour)) {
        printf("Appointment updated successfully.\n");
    } else {
        printf("Failed to update appointment.\n");
    }
}

static void handle_delete_service (void) {
    int id;

    if (!readInt(&id, "Enter Service ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (delete_service(id)) {
        printf("Service deleted successfully.\n");
    } else {
        printf("Failed to delete service.\n");
    }
}

static void handle_delete_technician (void) {
    int id;

    if (!readInt(&id, "Enter Technician ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (delete_technician(id)) {
        printf("Technician deleted successfully.\n");
    } else {
        printf("Failed to delete technician.\n");
    }
}

static void handle_find_appointments_by_customer (void) {
    int id;

    if (!readInt(&id, "Enter customer ID:", "[!] Invalid ID. Enter an integer."))
        return;

    find_appointments_by_customer(id);
}

static void handle_find_appointments_by_technician (void) {
    int id;

    if (!readInt(&id, "Enter technician ID:", "[!] Invalid ID. Enter an integer."))
        return;

    find_appointments_by_technician(id);
}

static void handle_process_payment (void) {
    int id;

    if (!readInt(&id, "Enter Appointment ID:", "[!] Invalid ID. Enter an integer."))
        return;

    if (process_payment(id)) {
        printf("Payment processed successfully.\n");
    } else {
        printf("Failed to process payment.\n");
    }
}

static void handle_print_all_records (void) {
    printf("\n========== ALL RECORDS ==========\n");

    printf("\n========== CUSTOMERS ==========\n");
    print_customers();

    printf("\n========== TECHNICIANS ==========\n");
    print_technicians();

    printf("\n========== SERVICES ==========\n");
    print_services();

    printf("\n========== APPOINTMENTS ==========\n");
    print_appointments();
}
