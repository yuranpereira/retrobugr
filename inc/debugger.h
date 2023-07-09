/* 
 * This is a fundamental structure which represents the actual
 * debugger. This structure must be properly allocated and in-
 * -itialized prior to the start of the debugging session.
 */
struct debugger {
    char *              dbge_path;
    pid_t               dbge_pid;
    
    /* This is the `head` of the  breakpoint array linked
     * list. This list contains all the breakpoint_array
     * structures which house the `breakpoint * array[]`.
     */
    struct sl_list_node bpa_list;
};

/*
 * Allocate a breakpoint_array structure.
 */
struct breakpoint_array *debugger_bpa_alloc() 
{
    return (struct breakpoint_array *)malloc(sizeof(
                struct breakpoint_array));
}

/*
 * Free the breakpoint array allocated by debugger_bpa_alloc()
 *
 * @param bpa - breakpoint_array pointer to free
 */
void debugger_bpa_free(struct breakpoint_array *bpa) 
{
    free(bpa);
}

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
    sl_list_init(&dbg->bpa_list);
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


