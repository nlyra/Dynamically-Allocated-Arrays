// Nathaniel Lyra
// COP 3502C, Spring 2019
// na107394

// Create a "lonely party array", where arrays are broken into fragments
// that get allocated and deallocateded on an as needed basis.

#include "LonelyPartyArray.h"
#include <stdio.h>
#include <stdlib.h>

LonelyPartyArray *createLonelyPartyArray(int num_fragments, int fragment_length)
{
    int i, j;

    // If one or both parameters passed are <= 0, immediately return NULL.
    if ((num_fragments || fragment_length) <= 0)
    {
        return NULL;
    }

    // Dynamically allocates a new struct LPA type called party of size LPA*.
    LPA *party = malloc(sizeof(LPA*) * (num_fragments * fragment_length));

    // If malloc function fails for some reason, return NULL.
    if (party == NULL)
        return NULL;

    // Set each new struct member to corresponding and appropriate value.
    party->num_fragments = num_fragments;
    party->fragment_length = fragment_length;
    party->num_active_fragments = 0;
    party->size = 0;

    // Dynamically allocate struct member "fragments" to size of the parameter being passed.
    party->fragments = malloc(sizeof(int*) * (num_fragments));

    // If malloc function fails, free all memory previously allocated and return NULL.
    if (party->fragments == NULL)
    {   
        free(party);
        return NULL;
    }

    // Dynamically allocate struct member fragment_sizes
    party->fragment_sizes = malloc(sizeof(int) * (num_fragments));

    // Set each fragments to NULL and fragment_sizes to 0 since no cells are currently in use.
    for (i = 0; i < num_fragments; i++)
    {
        party->fragments[i] = NULL;
        party->fragment_sizes[i] = 0;
    }

    // Since function has not yet returned NULL, means malloc succeeded. Print success message.
    printf("-> A new LonelyPartyArray has emerged from the void. (capacity: %d, fragments: %d)\n",
    party->num_fragments * party->fragment_length, party->num_fragments);

    // Return pointer to new LonelyPartyArray.
    return party;
}

LonelyPartyArray *destroyLonelyPartyArray(LonelyPartyArray *party)
{  
    int i, j;

    // If party is NULL, just return NULL.
    if (party == NULL)
        return NULL;
    
    // Free each of fragments subsets if it's not NULL.
    for (i = 0; i < party->num_fragments; i++)
    {
        if ((party->fragments[i]) != NULL)
            free(party->fragments[i]);
        else
            free(party->fragments[i]);
    }

    // Then free each of the struct members themselves after having freed their subsets.
    free(party->fragments);
    free(party->fragment_sizes);
    free(party);

    printf("-> The LonelyPartyArray has returned to the void.\n");

    return NULL;
}

int set(LonelyPartyArray *party, int index, int key)
{
    // Variable declarations/initializations. 
    int num_fragments, fragment_length, printfrag, printoffset;
    int i, lowindex, highindex, invalid = 0;

    // Check if struct pointer is NULL, return failure. 
    if (party == NULL)
    {
        printf("-> Bloop! NULL pointer detected in set().\n");
        return LPA_FAILURE;
    }

    // Assigning values to easier variable names.
    fragment_length = party->fragment_length;
    num_fragments = party->num_fragments; 
    printfrag = index/fragment_length;
    printoffset = index%fragment_length;


    // If index is considered "invalid", return failure.
    if ((index < 0) || (index > (num_fragments * fragment_length - 1)))
    {
        invalid = 1;

        // If party is not NULL and it's invalid..
        if ((party != NULL) && (invalid == 1))
        {
            printf("-> Bloop! Invalid access in set(). ");
            printf("(index: %d, fragment: %d, offset: %d)\n", index, printfrag, printoffset);
            return LPA_FAILURE;
        }
        return LPA_FAILURE;
    }

    // If the specific fragment is not NULL..
    if ((party->fragments[index/fragment_length]) != NULL)
    {   
        // If the fragment has been allocated but the cell has been marked UNUSED...
        if (party->fragments[index / fragment_length][index % fragment_length] == UNUSED)
        {
            // Insert key.
            party->fragments[index / fragment_length][index % fragment_length] = key;

            // Increment size and fragment_sizes members accordingly.
            party->size++;
            party->fragment_sizes[index / fragment_length]++;

            return LPA_SUCCESS;
        }

        // If the cell is already in use, just increcement the count for that index.
        else if (party->fragments[index / fragment_length][index % fragment_length] != UNUSED)
        {
            party->fragments[index / fragment_length][index % fragment_length] = key;
            return LPA_SUCCESS;
        }
    }
   
    // If fragment where we need to insert key is NULL, malloc that 
    // fragment (an array of fragment_length integers)...
    // Update our number of active fragments struct member.
    else
    {   
        party->fragments[index/fragment_length] = malloc(sizeof(int) * (fragment_length));
        lowindex = index - (index%fragment_length);
        highindex = lowindex + (fragment_length - 1);
        printf("-> Spawned fragment %d. ",  index/fragment_length);
        printf("(capacity: %d, indices: %d..%d)\n", fragment_length, lowindex, highindex);
        party->num_active_fragments++;

        // And set each cell in the fragment to UNUSED.
        for (i = 0; i < fragment_length; i++)
            party->fragments[index/fragment_length][i] = UNUSED;

        // Set the key to the appropriate corresponding index.
        party->fragments[index/fragment_length][index % fragment_length] = key;

        // Increment Size member.
        party->size++;

        // Increment fragment_sizes member.
        party->fragment_sizes[index/fragment_length]++;
        
        return LPA_SUCCESS;
    }

    return LPA_FAILURE;

}

int get(LonelyPartyArray *party, int index)
{
    int fraglen, numfrags;

    // If party pointer is NULL, printf and return failure.
    if (party == NULL)
    {
        printf("-> Bloop! NULL pointer detected in get().\n");
        return LPA_FAILURE;
    }

    fraglen = party->fragment_length;
    numfrags = party->num_fragments;
    
    // If index is invalid and party pointer is not NULL, printf and return failure.
    if (((index < 0) || (index > (numfrags * fraglen - 1))))
    {
        printf("-> Bloop! Invalid access in get(). ");
        printf("(index: %d, fragment: %d, offset: %d)\n", index, index/fraglen, index%fraglen);
        return LPA_FAILURE;
    }

    // If index refers to a cell in an unallocated fragment, return UNUSED.
    if (party->fragments[index/fraglen] == NULL)
    {
        return UNUSED;
    }

    // Return value in cell where index maps to.
    return party->fragments[index/fraglen][index%fraglen];
}

int delete(LonelyPartyArray *party, int index)
{
    int fraglen, numfrags;
    int lowindex, highindex;

    // If party pointer is NULL, printf and return failure.
    if (party == NULL)
    {
        printf("-> Bloop! NULL pointer detected in delete().\n");
        return LPA_FAILURE;
    }

    fraglen = party->fragment_length;
    numfrags = party->num_fragments;

    // If index is invalid, printf and return failure.
    if (((index < 0) || (index > (numfrags * fraglen - 1))))
    {
        printf("-> Bloop! Invalid access in delete(). ");
        printf("(index: %d, fragment: %d, offset: %d)\n", index, index/fraglen, index%fraglen);
        return LPA_FAILURE;
    }

    // If fragment being mapped to is NULL, return failure.
    if (party->fragments[index/fraglen] == NULL)
        return LPA_FAILURE;

    // If cell being sought is not alreadys set to UNUSED..
    if (party->fragments[index/fraglen][index%fraglen] != UNUSED)
    {
        // Set the cell to UNUSED, decrease size member, and decrease 
        // fragment_sizes to correct number of used cells in that fragment.
        party->fragments[index/fraglen][index%fraglen] = UNUSED;
        party->size--;
        party->fragment_sizes[index/fraglen]--;

        // If deleting the value in that cell causes that fragment to be
        // empty..
        if(party->fragment_sizes[index/fraglen] == 0)
        {   
            // Deallocate that fragment, and set it to NULL.
            free(party->fragments[index/fraglen]);
            party->fragments[index/fraglen] = NULL;

            // Update number of active fragments struct member.
            party->num_active_fragments--;

            // Printf appropriate variables.
            lowindex = index - (index % fraglen);
            highindex = lowindex + (fraglen - 1);
            printf("-> Deallocated fragment %d. ", index/fraglen);
            printf("(capacity: %d, indices: %d..%d)\n", fraglen, lowindex, highindex);
        }

        return LPA_SUCCESS;
    }

    // If cell is already marked as UNUSED, return failure.
    if (party->fragments[index/fraglen][index%fraglen] == UNUSED)
        return LPA_FAILURE;

    return LPA_FAILURE;

}

int containsKey(LonelyPartyArray *party, int key)
{
    int i, j;
    int numfrags, fraglen;

    // If party pointer is NULL, return 0.
    if (party == NULL)
        return 0;

    numfrags = party->num_fragments;
    fraglen = party->fragment_length;
    
    // Run a for-loop through each cell of each fragment to check for key.
    for (i = 0; i < numfrags; i++)
    {
        for (j = 0; j < fraglen; j++)
        {
            if (party->fragments[i][j] == key)
                return 1;
        }
    }

    // If key was not found in the above for-loop, return 0.
    return 0;
}

int isSet(LonelyPartyArray *party, int index)
{
    int numfrags, fraglen;

    // If party pointer is NULL, return 0.
    if (party == NULL)
        return 0;

    numfrags = party->num_fragments;
    fraglen = party->fragment_length;
    
    // If index is invalid, return 0.
    if (((index < 0) || (index > (numfrags * fraglen - 1))))
        return 0;

    // If fragment has not been allocated yet, return 0.
    if (party->fragments[index/fraglen] == NULL)
        return 0;
    
    // If cell is marked as UNUSED, return 0.
    if (party->fragments[index/fraglen][index%fraglen] == UNUSED)
        return 0;

    // Otherwise, return 1.
    return 1;
}

int printIfValid(LonelyPartyArray *party, int index)
{
    int fraglen, numfrags;

    // If 1) party pointer is NULL, 2) index is invalid, 3) fragment is NULL
    // or 4) the cell is UNUSED, return failure.
    if (party == NULL)
        return LPA_FAILURE;

    fraglen = party->fragment_length;
    numfrags = party->num_fragments;

    // If index is invalid, return failure.
    if (((index < 0) || (index > (numfrags * fraglen - 1))))
        return LPA_FAILURE;

    // If fragment where index is mapping to is NULL, return failure.
    if (party->fragments[index/fraglen] == NULL)
        return LPA_FAILURE;
    
    // If cell where index is being mapped to is UNUSED, return failure.
    if (party->fragments[index/fraglen][index%fraglen] == UNUSED)
        return LPA_FAILURE;

    // Otherwise printf and return success.
    printf("%d\n", party->fragments[index/fraglen][index%fraglen]);
    return LPA_SUCCESS;    

    
}

LonelyPartyArray *resetLonelyPartyArray(LonelyPartyArray *party)
{
    int i, j;
    int fraglen, numfrags;

    // If struct pointer is NULL, return failure.
    if (party == NULL)
    {
        printf("-> Bloop! NULL pointer detected in resetLonelyPartyArray().\n");
        return party;
    }

    fraglen = party->fragment_length;
    numfrags = party->num_fragments;


    // Run a for-loop through each fragment and the fragment_sizes array.
    for (i = 0; i < numfrags; i++)
    {
        // Set each one back to 0.
        party->fragment_sizes[i] = 0;

        // And if fragment is not NULL, and cell is not UNUSED, set it to UNUSED.
        if (party->fragments[i] != NULL)
        {
            for (j = 0; j < fraglen; j++)
            {
                if (party->fragments[i][j] != UNUSED)
                {
                    party->fragments[i][j] = UNUSED;
                }
            }
            // Free fragment and set it to NULL.
            free(party->fragments[i]);
            party->fragments[i] = NULL;
        }
    }

    // Reset rest of struct members to 0.
    party->size = 0;
    party->num_active_fragments = 0;

    printf("-> The LonelyPartyArray has returned to its nascent state. ");
    printf("(capacity: %d, fragments: %d)\n", fraglen*numfrags, numfrags);

    return party;
}

int getSize(LonelyPartyArray *party)
{
    // If struct pointer is NULL, return -1, otherwise return its size.
    if (party == NULL)
        return -1;
    else
        return party->size;
}

int getCapacity(LonelyPartyArray *party)
{
    int numfrags, fraglen;

    // If struct pointer is NULL, return -1, otherwise return its capacity.
    if (party == NULL)
    {
        return -1;
    }    
    else
    {
        numfrags = party->num_fragments;
        fraglen = party->fragment_length;

        return numfrags * fraglen;
    }
}

int getAllocatedCellCount(LonelyPartyArray *party)
{
    int fraglen, numactivefrags;

    // If struct pointer is NULL, return -1, otherwise return the number of allocated cells.
    if (party == NULL)
    {
        return -1;
    }    
    else
    {
        fraglen = party->fragment_length;
        numactivefrags = party->num_active_fragments;

        return fraglen * numactivefrags;
    }
}

long long unsigned int getArraySizeInBytes(LonelyPartyArray *party)
{   
    // If struct pointer is NULL, return 0, otherwise return size of a regular, 
    // non-dynamic array in bytes.
    if (party == NULL)
        return 0;

    int numCells = party->num_fragments * party->fragment_length;
    long long unsigned int bytes = numCells * sizeof(int);
    
    return bytes;
}

long long unsigned int getCurrentSizeInBytes(LonelyPartyArray *party)
{
    int i, j;

    long long unsigned int fragsArrayPointers, fragsArrayCells, fragSizesArray, total;

    // If struct pointer is NULL, return 0.
    if (party == NULL)
        return 0;
    
    // Store size of an LPA pointer and an LPA struct inside variables.
    long long unsigned int LPApointer = sizeof(LPA*);
    long long unsigned int LPAstruct = sizeof(LPA);

    // Store sizes of pointers in fragments array, size of each int cell, and size of
    // each int fragment in fragment_sizes array.
    fragsArrayPointers = sizeof(int*) * party->num_fragments;
    fragsArrayCells = (sizeof(int) * party->fragment_length) * party->num_active_fragments;
    fragSizesArray = sizeof(int) * party->num_fragments;
    
    // Add them all up, return that value.
    total = LPApointer + LPAstruct + fragsArrayPointers + fragsArrayCells + fragSizesArray;

    return total;
}

double difficultyRating(void)
{
    return 2.9;
}

double hoursSpent(void)
{
    return 15.5;
}

//Bonus function:
LonelyPartyArray *cloneLonelyPartyArray(LonelyPartyArray *party)
{
    int i, j;

    // If party pointer is NULL, return NULL.
    if (party == NULL)
        return NULL;

    // Dynamically allocate memory for a struct pointer of size num_fragments * fragment_length.
    LPA *clone = malloc(sizeof(LPA*) * (party->num_fragments * party->fragment_length));

    // If malloc failed, free memory and return NULL.
    if (party == NULL)
    {   
        free(clone);
        return NULL;
    }

    // Clone the integer data type struct members.
    clone->size = party->size;
    clone->num_fragments = party->num_fragments;
    clone->fragment_length = party->fragment_length;
    clone->num_active_fragments = party->num_active_fragments;

    // Dynamically allocate the fragments array of integer arrays.
    clone->fragments = malloc(sizeof(int*) * (party->num_fragments)); 
    
    // If call to malloc failed, free and return NULL.
    if (clone->fragments == NULL)
    {
        free(clone->fragments);
        return NULL;
    }
    
    // Go through each fragment in clone and...
    for (i = 0; i < clone->num_fragments; i++)
    {
        // ...malloc each array of integers.
        clone->fragments[i] = malloc(sizeof(int) * clone->fragment_length);

        // If that fragment in the original is NULL, set the clone to NULL also, and continue.
        if (party->fragments[i] == NULL)
        {
            clone->fragments[i] = NULL;
            continue;
        } 
        // Otherwise copy the original cell's value into the clone's cell.
        else
            for (j = 0; j < clone->fragment_length; j++)
            {
                clone->fragments[i][j] = party->fragments[i][j];   
            }   
    }

    // Dynamically allocate space for the fragment_sizes array of integers.
    clone->fragment_sizes = malloc(sizeof(int) * (party->num_fragments));

    // If call to malloc failed, free previously allocated blocks of memory.
    if (clone->fragment_sizes == NULL)
    {   
        for (i = 0; i < clone->num_fragments; i++)
        {
            free(clone->fragments[i]);
        }
        free(clone);
        free(clone->fragment_sizes);
        return NULL;
    }

    // If malloc succeeded, copy each of the original value into clone's.
    for (i = 0; i < clone->num_fragments; i++)
    {
        clone->fragment_sizes[i] = party->fragment_sizes[i];
    }

    printf("-> Successfully cloned the LonelyPartyArray. (capacity: %d, fragments: %d)\n",
    clone->num_fragments * clone->fragment_length, clone->num_fragments);

    return clone;
}
