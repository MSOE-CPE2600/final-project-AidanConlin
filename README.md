# final-project-AidanConlin
final-project-AidanConlin created by GitHub Classroom

Voting Machine Project

This project implements a voting machine system using shared memory and signals for inter-process communication.

The provided .csv file represents the dedicated file that cotains the data for these candidates.
The file contains an example of the needed format: (candidate),(number of votes)
Ex:
    Alice,0
    Bob,0

The system comprises two main components:

1. Voting Terminal (voting_terminal.c): Allows users to cast votes.

* Allows users to enter their name and the candidate they wish to vote for.
* Provides an option to confirm or change their vote before it is recorded.
* Sends the confirmed vote to the voter log.

2. Voter Log (voter_log.c): Records the votes in a file (candidates.csv).

* Receives votes from the voting terminal using signals.
* Updates the candidates.csv file with the vote.
* Adds new candidates to the file if they are not already present.
* Displays the current vote count for all candidates after each vote.

The system is designed to handle votes dynamically, ensuring proper user interaction, shared memory management, and signal-based communication between the processes.