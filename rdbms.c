#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_TABLES 10
#define MAX_COLUMNS 10
#define MAX_RECORDS 100
#define MAX_COLUMN_NAME 50
#define MAX_RECORD_SIZE 100
#define PORT 8080

typedef struct {
    char name[MAX_COLUMN_NAME];
    char data[MAX_RECORDS][MAX_RECORD_SIZE];
} Column;

typedef struct {
    char name[MAX_COLUMN_NAME];
    int column_count;
    Column columns[MAX_COLUMNS];
    int record_count;
} Table;

typedef struct {
    int table_count;
    Table tables[MAX_TABLES];
} Database;

Database db;

void create_table(const char *table_name) {
    if (db.table_count >= MAX_TABLES) {
        printf("Maximum table limit reached\n");
        return;
    }
    Table *table = &db.tables[db.table_count++];
    strncpy(table->name, table_name, MAX_COLUMN_NAME);
    table->column_count = 0;
    table->record_count = 0;
    printf("Table '%s' created\n", table_name);
}

void add_column(const char *table_name, const char *column_name) {
    for (int i = 0; i < db.table_count; i++) {
        if (strcmp(db.tables[i].name, table_name) == 0) {
            if (db.tables[i].column_count >= MAX_COLUMNS) {
                printf("Maximum column limit reached for table '%s'\n", table_name);
                return;
            }
            Column *column = &db.tables[i].columns[db.tables[i].column_count++];
            strncpy(column->name, column_name, MAX_COLUMN_NAME);
            printf("Column '%s' added to table '%s'\n", column_name, table_name);
            return;
        }
    }
    printf("Table '%s' not found\n", table_name);
}

void insert_record(const char *table_name, char *values[]) {
    for (int i = 0; i < db.table_count; i++) {
        if (strcmp(db.tables[i].name, table_name) == 0) {
            if (db.tables[i].record_count >= MAX_RECORDS) {
                printf("Maximum record limit reached for table '%s'\n", table_name);
                return;
            }
            for (int j = 0; j < db.tables[i].column_count; j++) {
                strncpy(db.tables[i].columns[j].data[db.tables[i].record_count], values[j], MAX_RECORD_SIZE);
            }
            db.tables[i].record_count++;
            printf("Record inserted into table '%s'\n", table_name);
            return;
        }
    }
    printf("Table '%s' not found\n", table_name);
}

void query_table(const char *table_name, char *response) {
    for (int i = 0; i < db.table_count; i++) {
        if (strcmp(db.tables[i].name, table_name) == 0) {
            strcat(response, "Table: ");
            strcat(response, table_name);
            strcat(response, "\n");
            for (int j = 0; j < db.tables[i].column_count; j++) {
                strcat(response, db.tables[i].columns[j].name);
                strcat(response, "\t");
            }
            strcat(response, "\n");
            for (int k = 0; k < db.tables[i].record_count; k++) {
                for (int j = 0; j < db.tables[i].column_count; j++) {
                    strcat(response, db.tables[i].columns[j].data[k]);
                    strcat(response, "\t");
                }
                strcat(response, "\n");
            }
            return;
        }
    }
    strcat(response, "Table not found\n");
}

void handle_query(const char *query, char *response) {
    char command[MAX_COLUMN_NAME];
    sscanf(query, "%s", command);

    if (strcmp(command, "CREATE_TABLE") == 0) {
        char table_name[MAX_COLUMN_NAME];
        sscanf(query, "%*s %s", table_name);
        create_table(table_name);
        strcpy(response, "Table created\n");
    } else if (strcmp(command, "ADD_COLUMN") == 0) {
        char table_name[MAX_COLUMN_NAME], column_name[MAX_COLUMN_NAME];
        sscanf(query, "%*s %s %s", table_name, column_name);
        add_column(table_name, column_name);
        strcpy(response, "Column added\n");
    } else if (strcmp(command, "INSERT") == 0) {
        char table_name[MAX_COLUMN_NAME], values[MAX_COLUMNS][MAX_RECORD_SIZE];
        sscanf(query, "%*s %s %s %s %s", table_name, values[0], values[1], values[2]);
        char *values_ptrs[MAX_COLUMNS] = { values[0], values[1], values[2] };
        insert_record(table_name, values_ptrs);
        strcpy(response, "Record inserted\n");
    } else if (strcmp(command, "QUERY") == 0) {
        char table_name[MAX_COLUMN_NAME];
        sscanf(query, "%*s %s", table_name);
        query_table(table_name, response);
    } else {
        strcpy(response, "Unknown command\n");
    }
}

void start_server() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char response[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        read(new_socket, buffer, 1024);
        printf("Received query: %s\n", buffer);
        handle_query(buffer, response);
        send(new_socket, response, strlen(response), 0);
        printf("Response sent: %s\n", response);
        memset(buffer, 0, sizeof(buffer));
        memset(response, 0, sizeof(response));
        close(new_socket);
    }
}

int main() {
    db.table_count = 0;
    start_server();
    return 0;
}

