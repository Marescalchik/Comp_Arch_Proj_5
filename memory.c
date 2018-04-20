#include "memory.h"

unsigned int clockX;
unsigned int numMisses;
int cache_org;

void resetClock()
{
  clockX = 0;
  numMisses = 0;
}

void printCacheOrg(int org)
{
  printf("Cache Organization: ");
  if (org == DIRECT)
    printf ("DIRECT MAPPED\n");
  else if (org == TWOWAY)
    printf ("TWO-WAY SET ASSOCIATIVE\n");
  else if (org == FULLY)
    printf ("FULLY ASSOCIATIVE\n");
  else
    printf ("ERROR: WRONG CACHE ORG\n");
}

// show cache contents: DIRECT or FULLY
void showCacheAddress()
{
  for(int j=0; j<BLOCKS_IN_CACHE; j++) {
    printf("Address in block %d contains tag %d. It was last used %d turns ago. \n", j, m.myCache.cblocks[j].tag, m.myCache.cblocks[j].last_used);
    for(int k=0; k<WORDS_PER_BLOCK; k++) {
		printf("Location: %d contains data %d. \n", k, m.myCache.cblocks[j].data[k]);


    }
    printf("\n");
  }
}

int getData (int address)                // load
{
  int data;
  /*////////////////////////////////////////////

  BEGIN THE DIRECT MAPPED CACHE ORGANIZATION getData(address) FUNCTION.

  */////////////////////////////////////////////
  if (cache_org == DIRECT) {
	  unsigned int wordAddress = address & 0b11;				// Position in word = bits 0/1 of the address
	  unsigned int index = address & 0b11100;					// Index = bits 2/3/4 of the address
	  unsigned int tag = address & 0b11111100000;				// Tag = upper remaining (6) bits of the address
	  index = index >> 2;										// Shift index to proper location (2 to right)
	  tag = tag >> 5;											// Shift tag to proper location (shift 5 to the right)
	  if ((m.myCache.cblocks[index].tag == tag) &&				// Check to see if the tag in index slot of cache is the same as the tag of the data, 
		  (m.myCache.cblocks[index].valid == 1))				// Also check to see if the data is valid.
	  {
		  data = m.myCache.cblocks[index].data[wordAddress];	// If it is, then the desired data was in the cache. Take the data out from the position of the word (lower two bits.)
		  m.myCache.cblocks[index].last_used = 0;				// set last_used for this position in the cache to 0
		  clockX = clockX + 2;									// Increment clock by 2 (as per instructions for cache hit)
	  }
	  else
	  {
		  data = mm.blocks[address >> 2].data[wordAddress];		// If it's not in the cache, get data from the memory location in the address. Use lowest two bits for the location, 
		  m.myCache.cblocks[index].last_used++;					// Increment the last time this was last used. It doesn't really *matter*
		  clockX = clockX + 100;
		  numMisses++;
	  }
	}
  /*////////////////////////////////////////////

  BEGIN THE TWO-WAY MAPPED CACHE ORGANIZATION getData(address) FUNCTION.

  */////////////////////////////////////////////
  else if (cache_org == TWOWAY)
  {
	  unsigned int wordAddress = address & 0b11;				// Position in word = bits 0/1 of the address
	  unsigned int setnum = address & 0b01100;					// setnum = bits 2/3 of the address
	  unsigned int tag = address & 0b11111110000;				// Tag = upper remaining (7) bits of the address
	  unsigned int foundVal = 0;
	  setnum = setnum >> 2;										// Shift index to proper location (2 to right)
	  tag = tag >> 4;											// Shift tag to proper location (shift 4 to the right)
		for (int j = 0; j < BLOCKS_IN_SET; j++)				// Starting at the 0th position in the set, search until the blocks/set - 1 position (aka, for two blocks, look at position 0 and 1)
		{ 
			if ((m.myCache.cblocks[2*setnum + j].tag == tag) && (m.myCache.cblocks[2*setnum + j].valid == 1))				// If the tag in position 2*setnum+j, aka position 0/1 in set setnum, is a match then you take that data and store it.
			{
				printf("Twoway data found!");
				data = m.myCache.cblocks[2 * setnum + j].data[wordAddress];	// Take data from that address in that block and store
				m.myCache.cblocks[2 * setnum + j].last_used = 0;			// This was the most recently used address bit, so set it to 0!
				clockX = clockX + 2;									// Increment clock by 2 as per instruction
				foundVal = 1;											// Set flag bit.
			}
			else if (j == (BLOCKS_IN_SET - 1))
			{
				m.myCache.cblocks[2 * setnum + j].last_used++;				// If it was NOT in that position, and this is the last tag in that block, 
																	// then you want to increment the LRU value of that block.
			}
		}
	  if (foundVal != 1)
	  {
		  data = mm.blocks[address >> 2].data[wordAddress];				// If it's not in the cache, get data from the memory location in the address. Use lowest two bits for the location, 

		  clockX = clockX + 100;										// Increment clock by 100, as per instruction
		  numMisses++;													// Increment the number of misses by one.
	  }
  }
  return data;
}

void putData (int address, int value)     // store
{
	/*////////////////////////////////////////////

	BEGIN THE DIRECT MAPPED CACHE ORGANIZATION putData(address, value) FUNCTION.

	*/////////////////////////////////////////////
	if (cache_org == DIRECT) {
		unsigned int wordAddress = address & 0b11;				// Position in word = bits 0/1 of the address
		unsigned int index = address & 0b11100;					// Index = bits 2/3/4 of the address
		unsigned int tag = address & 0b11111100000;				// Tag = upper remaining (6) bits of the address
		tag = tag >> 5;											// Reduce tag to proper size (shift 5 to the right)
		index = index >> 2;
		m.myCache.cblocks[index].tag = tag;						// Set the tag of the cache to be equal to the block tag
		m.myCache.cblocks[index].data[wordAddress] = value;		// Update the data in this specific block of the cache
		m.myCache.cblocks[index].valid = 1;
		mm.blocks[address >> 2].data[wordAddress] = value;		// Update the data in the memory for this location.
	}
	/*////////////////////////////////////////////

	END THE DIRECT MAPPED CACHE ORGANIZATION.

	*/////////////////////////////////////////////
	else if (cache_org == TWOWAY)
	{
		//unsigned int wordAddress = address & 0b11;				// Position in word = bits 0/1 of the address
		//unsigned int setnum = address & 0b01100;					// setnum = bits 2/3 of the address
		//unsigned int tag = address & 0b11111110000;				// Tag = upper remaining (7) bits of the address

	}


  // implement here
}

void printAsBinary(int c)
{
	int i = 0;
	for (i = 11; i >= 0; i--) {
		if ((c & (1 << i)) != 0) {
			printf("1");
		}
		else {
			printf("0");
		}
	}
	printf("\n");
}
