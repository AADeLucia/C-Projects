Project 4: Shell
====================

Description
--------------------
The goal was to create a shell that can be used like a normal shell, but with a
few added features:

1. If '> <filename>' is in the command then the output will be redirected to
   the specified file
2. If '&' is present then the shell will not wait for the process to finish
   before re-prompting

Programming Features of Note
----------------------------
1. execvp
2. forking and utilizing the child process
3. Manipulation of file descriptors, mainly 'stdout'

Credit
-------------------
Project assigned and created by Dr. Myers at Rollins College