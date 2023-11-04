#include <debug.h>
#include <proc.h>
#include <fs.h>

#define MAX_NR_PROC 4


PCB *current = NULL;
static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};



// Context* schedule(Context *prev) {};


void init_proc() {
  Log("Initializing processes...");
  //load program here
  // user_naive_load("/bin/menu");
  // TESTS = dummy file-test bmp-test cpp-test event-test exec-test hello timer-test
  // user_naive_load("/bin/dummy");
  // user_naive_load("/bin/file-test");
  // user_naive_load("/bin/bmp-test");
  // user_naive_load("/bin/cpp-test");
  // user_naive_load("/bin/event-test");
  // user_naive_load("/bin/exec-test");
  user_naive_load("/bin/hello");
  // user_naive_load("/bin/timer-test");
}


int execve(const char *filename, char *const argv[], char *const envp[]) {
    // check if the file exists
    int fd = fs_open(filename, 0, 0);
    if(fd == -1) {
        return -2;
    }
    fs_close(fd);
    // load the file
    user_ctx_load(current, filename, argv, envp);
    current = &pcb[0];
    yield();
    return 0;
}
