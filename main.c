#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

#define SERVER_IP "89.252.138.99"
#define SERVER_PORT 3306
#define USERNAME "hidirekt_ipek"
#define PASSWORD "o7E#=UQzzo6_"
#define DATABASE "hidirekt_ondergrup"
#define TABLE "machines"

MYSQL* db_connect() {
    MYSQL* conn = mysql_init(NULL);

    if (conn == NULL) {
        fprintf(stderr, "Hata: %s\n", mysql_error(conn));
        exit(1);
    }

    if (mysql_real_connect(conn, SERVER_IP, USERNAME, PASSWORD, DATABASE, SERVER_PORT, NULL, 0) == NULL) {
        fprintf(stderr, "Hata: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }

    return conn;
}

void db_disconnect(MYSQL* conn) {
    mysql_close(conn);
}

void create_record(MYSQL* conn, int MachineID, char* SetupDate, int acilStop1, int acilStop2) {
    char query[200];
    sprintf(query, "INSERT INTO %s (MachineID, SetupDate, acilStop1, acilStop2) VALUES (%d, '%s', %d, %d)",
            TABLE, MachineID, SetupDate, acilStop1, acilStop2);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Hata: %s\n", mysql_error(conn));
        exit(1);
    }
}

void read_record(MYSQL* conn, int MachineID) {
    char query[200];
    sprintf(query, "SELECT * FROM %s WHERE MachineID=%d", TABLE, MachineID);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Hata: %s\n", mysql_error(conn));
        exit(1);
    }

    MYSQL_RES* res = mysql_use_result(conn);
    MYSQL_ROW row;

    if ((row = mysql_fetch_row(res)) != NULL) {
        printf("MachineID: %s, SetupDate: %s, acilStop1: %s, acilStop2: %s\n", row[0], row[1], row[2], row[3]);
    } else {
        printf("Kayit bulunamadi!! MachineID=%d\n", MachineID);
    }

    mysql_free_result(res);
}

void update_record(MYSQL* conn, int MachineID, char* SetupDate, int acilStop1, int acilStop2) {
    char query[200];
    sprintf(query, "UPDATE %s SET SetupDate='%s', acilStop1=%d, acilStop2=%d WHERE MachineID=%d",
            TABLE, SetupDate, acilStop1, acilStop2, MachineID);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Hata: %s\n", mysql_error(conn));
        exit(1);
    }
}

void delete_record(MYSQL* conn, int MachineID) {
    char query[200];
    sprintf(query, "DELETE FROM %s WHERE MachineID=%d", TABLE, MachineID);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Hata: %s\n", mysql_error(conn));
        exit(1);
    }
}

int main() {
    MYSQL* conn = db_connect();

    create_record(conn, 45, "2022-01-01", 3434, 345345);
    read_record(conn, 123);
    update_record(conn, 2, "2022-01-02", 1, 0);
    delete_record(conn, 5);

    
    db_disconnect(conn);

    return 0;
}