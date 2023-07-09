#include <sys/ptrace.h>
#include <sys/personality.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "linenoise.h"
#include "list.h"
#include "../inc/breakpoint.h"
#include "../inc/breakpoint_array.h"
#include "../inc/debugger.h"

/**
 * Tokenize a string and retun an array of tokens.
 * The returned array of tokens must be freed by the user
 *
 * @param line       - String to tokenize
 * @param delim      - Character to use as delimeter
 * @param max_tokens - Maximum number of tokens to return
 *
 * @return An array of tokens (strings)
 */
char **split(char *line, const char *delim, int max_tokens)
{
    char *line_copy = strdup(line);
    char **tokens = NULL;
    int token_count = 0;

    tokens = malloc((sizeof(char *) * (max_tokens + 1)));
    if (tokens == NULL)
        return NULL;

    tokens[token_count] = strtok(line_copy, delim);
    while (tokens[token_count] != NULL) {
        if (token_count >= max_tokens)
            break;
        tokens[++token_count] = strtok(NULL, delim);
    }
    tokens[token_count] = NULL;

    return tokens;
}

/*
 * Check if `s' is a prefix of `of' 
 */
int is_prefix(char *s, const char *of)
{
    if (strlen(s) > strlen(of))
        return 0;

    return strncmp(s, of, strlen(of)) == 0;
}

/*--------------------------*/
struct debugee {
    // Is this even necessary?
};
/*--------------------------*/
#define INT3    0xcc

/*
 * This structure represents a single breakpoint.
 * It has embedded within it a linked list entry
 * that contains a pointer to the next breakpoint
 * in the list.
 */
struct breakpoint {
    pid_t pid;
    void *addr;
    int enabled;
    u_int8_t saved_data;

    /* This number is returned to the user when
     * a new breakpoint is created. It is unique
     * And represents a single breakpoint in the
     * list. */
    int number;

    /* The breakpoint entry list */
    struct sl_list_node bp_entry;
};

struct breakpoint *breakpoint_enable(pid_t pid, void *addr)
{
    u_int64_t data = 0;
    struct breakpoint *bp;
    u_int64_t data_with_int3;

    bp = malloc(sizeof(struct breakpoint));
    if (bp == NULL)
        return NULL;

    bp->pid = pid;
    bp->addr = addr;
    errno = 0;
    data = ptrace(PTRACE_PEEKDATA, pid, addr, 0);
    if (data == -1 && errno != 0)
        printf("errno is: %d\n", errno);
    bp->saved_data = ( data & (u_int8_t)0xff); // save bottom byte
    data_with_int3 = (( data & ~0xff) | INT3); // set bottom byte to 0xcc
    printf("errno = %d, pid = %d, addr = %p original = 0x%lx, modified = 0x%lx\n",
            errno, pid, addr, data, data_with_int3);

    ptrace(PTRACE_POKEDATA, pid, addr, data_with_int3);
    bp->enabled = 1;
    printf("Enabled breakpoint\n");

    return bp;
}

void breakpoint_disable(struct breakpoint *bp)
{
    u_int64_t data, restored_data; 
    
    data = ptrace(PTRACE_PEEKDATA, bp->pid, bp->addr, NULL);
    restored_data = ((data & ~0xff) | bp->saved_data);

    ptrace(PTRACE_PEEKDATA, bp->pid, bp->addr, restored_data);
    free(bp);
}
/*--------------------------*/
/* Debugger stuff */
struct debugger {
    char *dbge_path;
    pid_t dbge_pid;
    
    /* This is the `head` of the 
     * breakpoints linked list. */
    struct sl_list_node breakpoints;
};

/**
 * This function allocates memory for a debugger structure and
 * returns to the caller.
 */
struct debugger *debugger_alloc(void) 
{
    return (struct debugger *)malloc(sizeof(struct debugger));
}

/**
 * Free the debugger structure acquired by `debugger_alloc()`
 *
 * @param dbg - The `struct debugger *` to be freed
 */
void debugger_free(struct debugger *dbg)
{
    free(dbg);
}


/*
 * Initialize a debugger structure allocated by `debugger_alloc()`
 *
 * @param dbg       - pointer to the to a `struct debugger`
 * @param dbge_path - Debugee's path
 * @param dbge_pid  - Debugee's pid
 */
void debugger_init(struct debugger *dbg, char *dbge_path, 
        pid_t dbge_pid)
{
    dbg->dbge_path = dbge_path;
    dbg->dbge_pid  = dbge_pid;
    sl_list_init(&dbg->breakpoints);
}

/*
 * Insert a breakpoint into the debugger's breakpoint list.
 *
 * @param dbg - pointer to debugger structure 
 * @param bp  - pointer to breakpoint structure to insert
 */
int __debugger_breakpoint_insert(struct debugger *dbg, 
        struct breakpoint *bp)
{
    sl_list_add(&dbg->breakpoints, &bp->bp_entry);
    /* TODO: Attribute breakpoint number.*/
}

void __debugger_breakpoint_delete(struct debugger *dbg, 
        unsigned int bpn)
{
    /* TODO: Implement this function */
}

void set_breakpoint_at_address(struct debugger *dbg,  void *addr)
{
    struct breakpoint *bp;
    printf("Set breakpoint at address %p\n", addr);

    bp = breakpoint_enable(dbg->dbge_pid, addr);
    __debugger_breakpoint_insert(dbg, bp);
}

/**
 * Continue execution of debugee process.
 *
 * @param pid - Pid of the debugee process
 */
void continue_execution(pid_t pid)
{
    int wait_status, options = 0;

    ptrace(PTRACE_CONT, pid, NULL, NULL);
    waitpid(pid, &wait_status, options);
}

#define MAX_LINE_ARGS 64

/*
 * Handle input
 */
void handle_command(struct debugger *dbg, char *line)
{
    char **args = split(line, "  ", MAX_LINE_ARGS);
    char *command = args[0];

    if (is_prefix(command, "continue")) {
        continue_execution(dbg->dbge_pid);
    }
    else if (is_prefix(command, "break")) {
        char *addr = (char *)strtol(args[1], NULL, 16);

        set_breakpoint_at_address(dbg, addr);
        printf("breakpoint set at %p\n", addr);
    }
    else if (is_prefix(command, "quit")) {
        exit(0);
    }
    else {
        puts("Unknown command\n");
    }
}

/*
 * Launch the debugger by initiating the debugger loop
 *
 * @param dbg - pointer to debugger structure
 */
void debugger_launch(struct debugger *dbg)
{
    char *line;
    int wait_status;
    int options = 0;

    /* Wait for SIGTRAP */
    waitpid(dbg->dbge_pid, &wait_status, options);

    while ((line = linenoise("retrobugr> ")) != NULL) {
        handle_command(dbg, line);
        linenoiseHistoryAdd(line);
        linenoiseFree(line);
    }

}

int main(int argc, char **argv) 
{
    struct debugger *dbg;
    char *program;
    pid_t pid;

    if (argc < 2) {
        printf("Please specify target program.\n");
        return -1;
    }
    program = argv[1];

    dbg = debugger_alloc();
    if (!dbg) {
        printf("Couldn't allocate debugger\n");
        return -1;
    }

    pid = fork();
    if (pid == 0) {
        printf("Child started.\n");
        personality(ADDR_NO_RANDOMIZE);
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl(program, program, NULL);
    }
    else if (pid >= 1) {
        printf("Parent process.\n");
        debugger_init(dbg, program, pid);
        debugger_launch(dbg);
    }

    return 0;
}


/*
 * SIDENOTES: Following are points that I must keep in mind next time
 * come back.
 *
 * Implement breakpoint deletion functions
 * Find a way to handle breakpoint numbers
 * Fix empty input segmentation fault
 *
 * Note: Consider using a hash table to store breakpoints instead lin-
 * -ked lists. Consider using the breakpoint address as a key? My fear
 *  of hash tables is: possible issues with hash collision.
 *
 * But hash tables seem to me like a better choice than linked lists,
 * because I can easily use the index returned to the user as to cal-
 * -culate the index of the breakpoint in the hash table.
 *
 * If I can fix the hash collision issue, I can easily use the index 
 * returned to the user as an index for deletion. A hash table seems 
 * like the best data structure for this scenario.
 *
 */
