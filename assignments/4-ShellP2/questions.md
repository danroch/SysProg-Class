1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

   > **Answer**: Calling execvp directly would overwrite the current process image with the new one which isn't intended behavior. Using fork/execvp allows us to run both the shell and the chld process concurrently, and also provides us with error handling in the event of the forked process failing

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

   > **Answer**: There is a condition for this where the system call returns -1 and no child process is created. My implemntation just prints a system generated error describing the fork failure via perror("fork").

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

   > **Answer**: The execvp() function just searches for the command in the directories listed in the PATH environment variable. Unless it contains some absolute path this is how the executable is found.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

   > **Answer**: wait() is needed to make the parent process pause execution until the child process terminates. If we didn't call it, the child process could become a zombie process, ultimately just wasting resources, not terminating until the parent calls wait() or exits.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

   > **Answer**: WEXITSTATUS() extracts the child's exit code if it exited normally. This is important to determine if the child process was a success or a failure. It can be useful in our shell scenario since it can be used to store the last command's exist status, similarly to how $? does the same thing.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

   > **Answer**: I used a function called parse_command for clarity, but this handled quoted arguments by adding a state variable to indicate if the current character pointer being used to go through the buffer was inside or outside of quotes. This was necessary because quotes are handled in a special manner in a shell. Without accounting for whether or not I was in quotes, all spaces would be stripped from text within quotes which wouldn't be intended behavior

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

   > **Answer**: The biggest change was accounting for quotes, and to do this I no longer used strtok and took a more manual approach to processing the buffer. The challenge was not being able to rely on something like strtok.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

  > **Answer**: Signals are a mechanism for asynchronous communication between proceses and the operating system. They can be used to notify a process that an event has occurred, such as an interrupt, termination request, etc. In terms of how they differ from other forms of IPC (pipes, shared memory, sockets, etc), they are asynchronous, and act as notifications as opposed to sending out drawn out payloads or streams.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

  > **Answer**: Using the three signals given, SIGKILL forces a process to terminate immediately and it's removed from memory. This is used for when a process becomes unresponsive or SIGTERM fails. SIGTERM is a more graceful and proper method of terminating a process which allows the process to perform cleanup before exiting. It is used to allow a program to save its state before shutting down. SIGINT is sent when the user presses Ctrl+C in the terminal and allows a process to exit unless it ignores the signal. The usecase here is just stopping interactive programs like command line applications.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

  > **Answer**: When a process recieves SIGSTOP it is paused immediately byt the kernel and remains in this stopped state until it receives a SIGCONT signal which resumes the execution. It can't be caught or ignored like SIGINT because it is used to kill unresponsive tasks. When the task itself is unresponsive, there is not reason for it to handle a signal, so the handling is done by the kernel entirely.
