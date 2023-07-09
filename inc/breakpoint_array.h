/*
 * Copyright (c) 2023 Yuran Pereira
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the “Software”), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software 
 * is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all 
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN 
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <breakpoint.h>

// Error codes
#define ENOBP   -1      // No breakpoint set

#define MAX_BREAKPOINTS_PER_LIST    128

/*
 * This structure represents a linked list node entry which
 * stores an array of breakpoint pointers.
 */
struct breakpoint_array {
    struct breakpoint * array[MAX_BREAKPOINTS_PER_LIST];
    unsigned int        count;
    int                 full;

    /* Lowest free index.*/
    int                 lf_idx;
    struct sl_list_node entry;
};

/**
 * Check if breakpoint array full flag is set.
 *
 * @param bpa - breakpoint array pointer
 */
int breakpoint_array_full(struct breakpoint_array *bpa)
{
    return bpa->full;
}

/**
 * Return the index of the next free position in the breakpoint 
 * array.
 *
 * @param bpa - breakpoint array pointer
 */
int __breakpoint_array_next_free(struct breakpoint_array *bpa)
{
    int i;
    for (i = 0; i < MAX_BREAKPOINTS_PER_LIST; i++)
        if (bpa->array[i] == NULL)
            return i;   
}

/**
 * Add a breakpoint pointer to the breakpoint array in the lowest
 * free index.
 *
 * Note that the caller is responsible for checking if the array 
 * is full prior to calling this function. Attempting to add a
 * breakpoint to a breakpoint_array whose `full` field is set will
 * have no effect on the array.
 *
 * @param bpa - Pointer to the `breakpoint_array` structure
 * @param bp  - Pointer to the breakpoint to store in array
 * @return    - Returns the index `lf_idx` in the array where the 
 *              breakpoint was inserted on success, or an 
 *              appropriate error code (e.g ENOBP) on error
 */
int breakpoint_array_add_breakpoint(struct breakpoint_array *bpa, 
        struct breakpoint *bp)
{
    int idx = bpa->lf_idx;

    if (bpa->full)
        return ENOBP;

    bpa->array[idx] = bp;

    bpa->count++;
    if (bpa->count == MAX_BREAKPOINTS_PER_LIST)
        bpa->full = 1;
    
    bpa->lf_idx = __breakpoint_array_next_free(bpa);
    return idx;
}

/**
 * Delete a breakpoint pointer at the index `idx` from the array 
 *
 * Attempting to delete from an empty breakpoint array has no effect in
 * the breakpoint array.
 *
 * @param bpa - pointer to breakpoint array
 * @param idx - index of breakpoint in the array
 */
void breakpoint_array_del_breakpoint(struct breakpoint *bpa, 
        unsigned int idx)
{
    if (!bpa->count || bpa->idx >= MAX_BREAKPOINTS_PER_LIST)
        return;
        
    bpa->array[idx] = NULL;
    bpa->count--;
    bpa->lf_idx = __breakpoint_array_next_free(bpa);
}


/**
 * Get the count field of the breakpoint array 
 *
 * Note that this function does not does not actually iterate 
 * through the array to count. It simply returns the value of
 * the count field.
 *
 * @param bpa - breakpoint array pointer
 */
int breakpoint_array_get_count(struct breakpoint_array *bpa)
{
    return bpa->count;
}

/**
 * Increment the count field of the breakpoint array
 *
 * @param bpa - breakpoint array pointer
 */
void breakpoint_array_inc_count(struct breakpoint_array *bpa)
{
    bpa->count++;
}

/**
 * Decrement the count field of the breakpoint array
 *
 * @param bpa - breakpoint array pointer
 */
void breakpoint_array_dec_count(struct breakpoint_array *bpa)
{
    bpa->count--;
}

/*
 * Get a breakpoint from the breakpoint_array structure 
 */
struct breakpoint *breakpoint_array_get_breakpoint(struct breakpoint_array *bpa, 
        unsigned int idx)
{
    if (idx >= MAX_BREAKPOINTS_PER_LIST)
        return NULL;

    return bpa->array[idx];
}
