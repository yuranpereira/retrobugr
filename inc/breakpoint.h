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

/* Ensure that uint8_t is defined */
#ifndef UINT8_T
typedef unsigned char uint8_t
#endif

/*                                                                                                                                        
 * This structure represents a single breakpoint.
 * It has embedded within it a linked list entry
 * that contains a pointer to the next breakpoint
 * in the list.
 */
struct breakpoint {
    pid_t           pid;
    void            *addr;
    int             enabled;
    uint8_t         saved_data;

    /* This number is returned to the user when
     * a new breakpoint is created. It is unique
     * And represents a single breakpoint in the
     * list. 
     */
    unsigned int    number;
};

/**
 * Sets the `enabled` flag of the breakpoint structure
 */
void inline __set_breakpoint_enabled(struct breakpoint *bp)
{
    bp->enabled = 1;
}

/**
 * Unsets the `enabled` flag of the breakpoint structure
 */
void inline __unset_breakpoint_enabled(struct breakpoint *bp)
{
    bp->enabled = 0;
}

/**
 * Enables the breakpoint by setting the `enabled` flag
 */
void breakpoint_enable(struct breakpoint *bp)
{
    __set_breakpoint_enabled(bp);
}

/**
 * Disables the breakpoint by unsetting the `enabled` flag
 */
void breakpoint_disable(struct breakpoint *bp)
{
    __unset_breakpoint_enabled(bp);
}

/**/
void breakpoint_save_data(struct breakpoint *bp, uint8_t data)
{
    bp->saved_data = data;
}

/*
 * Get the breakpoint's saved data
 */
uint8_t breakpoint_get_saved_data(struct breakpoint *bp)
{
    return bp->saved_data;
}
