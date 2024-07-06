#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Define the max number of tables and columns
#define MAX_TABLES 10
#define MAX_COLUMNS 10
#define MAX_RECORDS 100
#define MAX_COLUMN_NAME 50
#define MAX_RECORD_SIZE 100


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
		printf("Max table limit reached\n");
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
				printf("Max column limit reached for table '%s'\n",table_name);
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

// Function to insert a record into a table
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

// Function to query records from a table
void query_table(const char *table_name) {
    for (int i = 0; i < db.table_count; i++) {
        if (strcmp(db.tables[i].name, table_name) == 0) {
            printf("Table: %s\n", table_name);
            for (int j = 0; j < db.tables[i].column_count; j++) {
                printf("%s\t", db.tables[i].columns[j].name);
            }
            printf("\n");
            for (int k = 0; k < db.tables[i].record_count; k++) {
                for (int j = 0; j < db.tables[i].column_count; j++) {
                    printf("%s\t", db.tables[i].columns[j].data[k]);
                }
                printf("\n");
            }
            return;
        }
    }
    printf("Table '%s' not found\n", table_name);
}

int main() {
	db.table_count = 0;

	create_table("users");

	add_column("users", "id");
	add_column("users", "name");
    add_column("users", "email");

    // Insert records into the table
    char *record1[] = {"1", "John Doe", "john@example.com"};
    insert_record("users", record1);

    char *record2[] = {"2", "Jane Smith", "jane@example.com"};
    insert_record("users", record2);

    // Query the table
    query_table("users");

    return 0;
}
