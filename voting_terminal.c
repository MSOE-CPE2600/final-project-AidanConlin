/**
 * @file voting_terminal.h
 * @brief Executable file designed to act as a "terminal" for voting,
 * sending a signal conveying the desired vote. 
 * 
 * Name: Aidan Conlin
 * Class: CPE 2600
 * Section: 121
 * Compile: gcc -o voting_terminal voting_terminal.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "vote.h"

vote_data_t *shared_data = NULL; // Shared memory pointer

/**
 * @brief Establishes a shared connection with active voting terminals.
 */
void setup_shared_memory() {
    int shm_id = shmget(SHM_KEY, sizeof(vote_data_t), 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        exit(1);
    }
    shared_data = (vote_data_t *)shmat(shm_id, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }
}

/**
 * @brief Utilizes the confirmed vote from the user and sends a signal with the
 * data provided by the user to the voter log.
 * 
 * @param log_pid The process ID of the voter log process that will receive the signal.
 * @param voter The name of the voter as a string, provided by the user.
 * @param candidate The name of the candidate being voted for, provided by the user.
 */
void send_vote(pid_t log_pid, const char *voter, const char *candidate) {
    strncpy(shared_data->voter_name, voter, MAX_NAME_LENGTH);
    strncpy(shared_data->candidate_name, candidate, MAX_NAME_LENGTH);
    shared_data->signal_flag = 1; // Mark new vote
    kill(log_pid, SIGUSR1); // Notify voter_log
    printf("Vote sent: %s -> %s\n", voter, candidate);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <voter_log_pid>\n", argv[0]);
        return 1;
    }

    pid_t log_pid = atoi(argv[1]);
    setup_shared_memory();

    
    char voter[MAX_NAME_LENGTH];
    char candidate[MAX_NAME_LENGTH];
    char choice;

    // Prompt name
    printf("Enter your name: ");
    fgets(voter, MAX_NAME_LENGTH, stdin);
    voter[strcspn(voter, "\n")] = '\0';

    while (1) {
        // Prompt user
        printf("Enter the candidate's name: ");
        fgets(candidate, MAX_NAME_LENGTH, stdin);
        candidate[strcspn(candidate, "\n")] = '\0';

        // Confirm the vote
        printf("You entered:\n");
        printf("Voter: %s\n", voter);
        printf("Candidate: %s\n", candidate);
        printf("Would you like to change your vote? (y/n): ");
        choice = getchar();
        // Clear the input buffer
        while (getchar() != '\n');

        // Assumes non 'y' responses as a "no"
        if (choice != 'y' && choice != 'Y') {
            // Exit the loop if the vote is confirmed
            break;
        }

        printf("Let's update your vote.\n");
    }

    // Send the confirmed vote to the log
    send_vote(log_pid, voter, candidate);
    

    if (shmdt(shared_data) < 0) {
        perror("shmdt failed");
    }
    return 0;
}
