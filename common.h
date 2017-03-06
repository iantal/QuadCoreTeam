//
// Created by Stefan Avram on 06/03/2017.
//

#ifndef QUADCORETEAM_COMMON_H
#define QUADCORETEAM_COMMON_H

#endif //QUADCORETEAM_COMMON_H


#define BUFFER_SIZE 4096
#define PORT 5000


typedef enum {
    AUTH_REQ,
    DISCONNECT_REQ,
    USERNAME,
    SUCCESS,
    ERROR,
    MESSAGE

} message_type;

typedef struct {
    message_type type;
    char username[21];
    unsigned int length;
    char *body;
} message;
