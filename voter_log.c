/**
 * @file voter_log.h
 * @brief Executable file with the purpose of acting as a central point
 * to obtain and handle votes from the terminal executable.
 * 
 * Name: Aidan Conlin
 * Class: CPE 2600
 * Section: 121
 * Compile: gcc -o voter_log voter_log.c
 */
#include "vote.h"

vote_data_t *shared_data = NULL; // Shared memory pointer
int running = 1; // Flag to keep the program running
char line[256];

/**
 * @brief Establishes a shared connection with active voting terminals.
 */
void setup_shared_memory() {
    int shm_id = shmget(SHM_KEY, sizeof(vote_data_t), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        exit(1);
    }
    shared_data = (vote_data_t *)shmat(shm_id, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }
    shared_data->signal_flag = 0; // Initialize flag
}

/**
 * @brief Takes in votes from the terminal and records them to the dedicated .csv file.
 * 
 * Responds to signal data from the terminals after confirming their votes.
 * After confirming the vote, it updates the file and prints out the current votes per candidate.
 */
void handle_vote() {
    if (shared_data->signal_flag) {
        printf("New vote received: %s voted for %s\n", 
            shared_data->voter_name, shared_data->candidate_name);
        shared_data->signal_flag = 0; // Reset flag

        // Update candidates.csv
        FILE *file = fopen(CSV_FILE, "r+");
        if (!file) {
            perror("Error opening candidates.csv");
            return;
        }

        int found = 0;

        // Create a temporary file to store updated contents
        FILE *temp_file = fopen("candidates_temp.csv", "w");
        if (!temp_file) {
            perror("Error creating temporary file");
            fclose(file);
            return;
        }

        // Read the original file line by line and update the candidate's votes
        while (fgets(line, sizeof(line), file)) {
            char candidate_name[MAX_NAME_LENGTH];
            int vote_count;

            if (sscanf(line, "%[^,],%d", candidate_name, &vote_count) == 2) {
                if (strcmp(candidate_name, shared_data->candidate_name) == 0) {
                    vote_count++; // Increment vote count
                    found = 1;
                }
                fprintf(temp_file, "%s,%d\n", candidate_name, vote_count);
            } else {
                // Preserve any malformed or unexpected lines
                fprintf(temp_file, "%s", line);
            }
        }

        // If the candidate wasn't found, add them to the file
        if (!found) {
            fprintf(temp_file, "%s,1\n", shared_data->candidate_name);
        }

        // Close the files before replacing
        fclose(file);  
        fclose(temp_file);  

        // Replace the original file with the updated file
        if (remove(CSV_FILE) != 0 || rename("candidates_temp.csv", CSV_FILE) != 0) {
            perror("Error updating candidates.csv");
        } else {
            printf("Vote successfully recorded in candidates.csv\n");
            // Reopen the candidates.csv file for reading and display the current votes
            file = fopen(CSV_FILE, "r");
            if (!file) {
                perror("Error reopening candidates.csv");
                return;
            }

            printf("\nCurrent Votes:\n");
            printf("---------------------\n");
            while (fgets(line, sizeof(line), file)) {
                char candidate_name[MAX_NAME_LENGTH];
                int vote_count;
                if (sscanf(line, "%[^,],%d", candidate_name, &vote_count) == 2) {
                    printf("%-15s %d\n", candidate_name, vote_count);
                }
            }
            printf("---------------------\n");
            // Close after printing the vote list
            fclose(file);  
        }
    }
}

/**
 * @brief Seperates the shared memory between the two executables upon being given the signal.
 */
void handle_sigint() {
    printf("\nQuitting voter log...\n");
    if (shmdt(shared_data) < 0) {
        perror("shmdt failed");
    }
    exit(0);
}


int main() {
    setup_shared_memory();
    struct sigaction sa;
    sa.sa_handler = handle_vote;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        perror("Failed signal action");
        exit(1);
    }

    printf("Voter log is running. Press Ctrl+C to quit. PID: %d\n", getpid());

    FILE *file = fopen(CSV_FILE, "r+");
    printf("\nCurrent Votes:\n");
    printf("---------------------\n");
    while (fgets(line, sizeof(line), file)) {
        char candidate_name[MAX_NAME_LENGTH];
        int vote_count;
        if (sscanf(line, "%[^,],%d", candidate_name, &vote_count) == 2) {
            printf("%-15s %d\n", candidate_name, vote_count);
        }
    }
    printf("---------------------\n");
    // Close after printing the vote list
    fclose(file);  

    signal(SIGINT, handle_sigint);
    while (1) {
        pause(); // Wait for signals
    }
    
    return 0;
}