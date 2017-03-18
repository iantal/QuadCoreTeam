//
// Created by Stefan Avram on 18/03/2017.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

char *serialize(message msg) {
    char *buff = calloc(sizeof(message) + msg.length, sizeof(char));
    sprintf(buff, "%s%s%03d%s", msg.type, msg.username, msg.length, msg.body);
    return buff;
}

message deserialize(char *buff) {
    char *p = buff;
    message msg;
    memcpy(msg.type, buff, TYPE_LEN);
    msg.type[TYPE_LEN] = '\0';
    p = p + TYPE_LEN;
    memcpy(msg.username, p, USER_LEN);
    msg.username[USER_LEN] = '\0';
    p = p + USER_LEN;
    msg.length = (unsigned int) atoi(p);
    msg.body = calloc(msg.length, sizeof(char));
    return msg;
    // strncpy(message_received.type, read_size, sizeof(message));
}
