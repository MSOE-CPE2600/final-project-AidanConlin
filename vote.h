/**
 * @file vote.h
 * @brief Provides a struct, definitions and include staments
 * to be shared between the log and terminal executables.
 * 
 * Name: Aidan Conlin
 * Class: CPE 2600
 * Section: 121
 */
#ifndef VOTE_H
#define VOTE_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define CSV_FILE "candidates.csv"
#define MAX_NAME_LENGTH 50
#define SHM_KEY 12345

typedef struct {
    char voter_name[MAX_NAME_LENGTH];
    char candidate_name[MAX_NAME_LENGTH];
    int signal_flag; // 1 for new vote, 0 for no new vote
} vote_data_t;

#endif
