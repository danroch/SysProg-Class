1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

The implementation ensures that all child processes complete before the shell continues accepting user input by calling waitpid() in the parent process for each child process (each command).

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

It is necessary to close unused pipe ends because file descriptors are a limited resource in the system, so neglecting to close the unused pipes will consume available descriptors. What could go wrong in this scenario is a new process being unable to access a descriptor and ultimately failing.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

cd is implemented as a built-in (executed by the parent process) because it changes the state of the shell. If it were used in execvp() and implemented as an external process, it would be executed by a forked child process, and the current working directory of that child would be changed rather than the shell the user was working on in the first place.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

To allow the shell implementation to accept an abritrary number of piped commands I would store commands dynamically using malloc() and realloc() as well as allocate pipe file descriptors dynamically instead of using a fixed array. The tradeoff is flexibility/scalability, efficient memory usage in exchange for having a more complex implementation and having a greater risk of memory leaks.
