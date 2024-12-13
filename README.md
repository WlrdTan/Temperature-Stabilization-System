# Temperature-Stabilization-System
Created a central server to gather and calculate temperature data from four client processes using TCP-based communication channels.

<img width="1633" alt="4" src="https://github.com/user-attachments/assets/026b799c-9fe5-47db-9899-89a6ea536ed0">

This program simulates a temperature stabilization system using a multi-process architecture with inter-process communication via TCP sockets. The setup includes:

A central process (server) that gathers temperatures from four external processes (clients).
Four client processes, each sending their current temperature to the server.
The system continues to exchange temperatures until the values stabilize, based on predefined formulas.
