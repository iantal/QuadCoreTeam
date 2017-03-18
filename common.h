//
// Created by Stefan Avram on 06/03/2017.
//

#ifndef QUADCORETEAM_COMMON_H
#define QUADCORETEAM_COMMON_H

#endif //QUADCORETEAM_COMMON_H

#define PACK_SIZE sizeof(message)
#define BUFFER_SIZE 4000
#define IP_ADDRESS "127.0.0.1"
#define PORT 5000



typedef enum {
    AUTH_REQ,
    DISCONNECT_REQ,
    SUCCESS,
    ERROR,
    MESSAGE

} message_type;

typedef struct {
    char type[4];
    char username[21];
    unsigned int length;
    char *body;
} message;
