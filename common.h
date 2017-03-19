//
// Created by Stefan Avram on 06/03/2017.
//

#ifndef QUADCORETEAM_COMMON_H
#define QUADCORETEAM_COMMON_H

#endif //QUADCORETEAM_COMMON_H

#define PACK_SIZE sizeof(message)
#define IP_ADDRESS "127.0.0.1"
#define PORT 5001

#define TYPE_LEN 3
#define USER_LEN 4
#define LENGTH_LEN 3
#define TOTAL_HEADER_LEN 10

typedef struct {
    char type[4];
    char username[5];
    unsigned char length;
    char *body;
} message;

char *serialize(message msg);

message deserialize(char *buff);

message create_message(char *type, char *username, char *body);