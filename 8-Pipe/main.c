#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../CmnBase/cmnbase.h"

#define BUF_SIZE 10

typedef struct {
    const char *roleDesc;           // Description of the child process role
    void (*parentHandler)(pid_t);   // Parent process handler
    void (*childHandler)(void);     // Child process handler
} ForkHandler;

bool initializePipe(int pfd[2]);
bool spawnChildProcesses();
pid_t forkWithHandlers(
    const char *roleDesc,
    void (*parentHandler)(pid_t),
    void (*childHandler)(void)
);
void childHandlerLs();
void childHandlerWc();
void executeCommandWithPipe(
    int pfd[2],
    int redirectFd,
    int pipeEndToUse,
    const char *cmdPath,
    const char *arg0,           
    const char *arg1,
    const char *roleDesc
);

void cleanup();
void waitForChildren();

int pfd[2]; // Pipe file descriptors
pid_t cid[2]; // Child process IDs
ForkHandler handlers[] = { // Handlers for child processes
    {
        .roleDesc = "ls -l Command",
        .parentHandler = NULL,
        .childHandler = childHandlerLs
    },
    {
        .roleDesc = "wc -l Command",
        .parentHandler = NULL,
        .childHandler = childHandlerWc
    }
};

int main(int argc, char* argv[]){
    if(!initializePipe(pfd)){
        terminate(EXIT_FAILURE, true);
    }

    if(!spawnChildProcesses()){
        terminate(EXIT_FAILURE, true);
    }

    cleanup();
    waitForChildren();

    return 0;
}

bool initializePipe(int pfd[2]) {
    if (pipe(pfd) == -1) {
        log_error("pipe creation failed");
        return false;
    }
    return true;
}

bool spawnChildProcesses() {
    // Fork child processes based on the handlers defined
    for (size_t i = 0; i < sizeof(handlers)/sizeof(handlers[0]); i++) {
        // Ensure we do not exceed the cid array size
        if(i >=  sizeof(cid)/sizeof(cid[0])){
            log_error("Insufficient space in cid array");
            return false;
        }

        // Fork with specified handlers
        cid[i] = forkWithHandlers(
            handlers[i].roleDesc,
            handlers[i].parentHandler,
            handlers[i].childHandler
        );

        //Any fork failure, retrun false
        if (cid[i] == -1) {
            return false;
        }
    }
    return true;
}

pid_t forkWithHandlers(
    const char *roleDesc,
    void (*parentHandler)(pid_t),
    void (*childHandler)(void)
) {
    pid_t cid = fork();
    if (cid == 0) {
        // Child process
        log_info("Child process for '%s' started", roleDesc);
        if (childHandler) {
            childHandler();
        }
        exit(EXIT_SUCCESS);
    } else if (cid < 0) {
        log_error("Fork failed for %s", roleDesc);
        return -1;
    } else {
        //Parent process
        log_info("Child process for '%s' created with pid = %d", roleDesc, cid);
        if (parentHandler) {
            parentHandler(cid);
        }
        return cid;
    }
}

void childHandlerLs() {
    extern int pfd[2];
    executeCommandWithPipe(
        pfd,
        STDOUT_FILENO,
        pfd[1],
        "/bin/ls",
        "ls",
        "-l",
        "ls -l Command"
    );
}

void childHandlerWc() {
    extern int pfd[2];
    executeCommandWithPipe(
        pfd,
        STDIN_FILENO,
        pfd[0],
        "/usr/bin/wc",
        "wc",
        "-l",
        "wc -l Command"
    );
}

void executeCommandWithPipe(
    int pfd[2],
    int redirectFd,
    int pipeEndToUse,
    const char *cmdPath,
    const char *arg0,           
    const char *arg1,
    const char *roleDesc
) {
    //Close unused pipe end
    close(pfd[redirectFd == STDIN_FILENO ? 1 : 0]);

    //Redirect
    if (dup2(pipeEndToUse, redirectFd) == -1) {
        log_error("dup2 failed");
        terminate(EXIT_FAILURE, false);
    }
    close(pipeEndToUse); //close the original pipe end after duplication

    // execute command
    if (execl(cmdPath, arg0, arg1, NULL) == -1) {
        log_error("execl failed for %s", roleDesc);
        terminate(EXIT_FAILURE, false);
    }

    exit(EXIT_FAILURE); // This line should never be reached
}

void cleanup() {
    //Close both ends of the pipe in the parent process
    close(pfd[0]);
    close(pfd[1]);
}

void waitForChildren() {
    for (size_t i = 0; i < sizeof(cid) / sizeof(cid[0]); i++) {
        int status;
        pid_t wpid = waitpid(cid[i], &status, 0);

        if (wpid == -1) {
            log_error("Failed to wait for child process with pid %d", cid[i]);
            continue;
        }

        if (WIFEXITED(status)) {
            log_info("Child process %d exited with status %d", wpid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            log_warn("Child process %d terminated by signal %d", wpid, WTERMSIG(status));
        } else {
            log_warn("Child process %d exited abnormally", wpid);
        }
    }
}
