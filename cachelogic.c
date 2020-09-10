#include "tips.h"

/* The following two functions are defined in util.c */

/* finds the highest 1 bit, and returns its position, else 0xFFFFFFFF */
unsigned int uint_log2(word w); 

/* return random int from 0..x-1 */
int randomint( int x );

/*
  This function allows the lfu information to be displayed

    assoc_index - the cache unit that contains the block to be modified
    block_index - the index of the block to be modified

  returns a string representation of the lfu information
 */
char* lfu_to_string(int assoc_index, int block_index)
{
  /* Buffer to print lfu information -- increase size as needed. */
  static char buffer[9];
  sprintf(buffer, "%u", cache[assoc_index].block[block_index].accessCount);

  return buffer;
}

/*
  This function allows the lru information to be displayed

    assoc_index - the cache unit that contains the block to be modified
    block_index - the index of the block to be modified

  returns a string representation of the lru information
 */
char* lru_to_string(int assoc_index, int block_index)
{
  /* Buffer to print lru information -- increase size as needed. */
  static char buffer[9];
  sprintf(buffer, "%u", cache[assoc_index].block[block_index].lru.value);

  return buffer;
}

/*
  This function initializes the lfu information

    assoc_index - the cache unit that contains the block to be modified
    block_number - the index of the block to be modified

*/
void init_lfu(int assoc_index, int block_index)
{
  cache[assoc_index].block[block_index].accessCount = 0;
}

/*
  This function initializes the lru information

    assoc_index - the cache unit that contains the block to be modified
    block_number - the index of the block to be modified

*/
void init_lru(int assoc_index, int block_index)
{
  cache[assoc_index].block[block_index].lru.value = 0;
}

/*
  This is the primary function you are filling out,
  You are free to add helper functions if you need them

  @param addr 32-bit byte address
  @param data a pointer to a SINGLE word (32-bits of data)
  @param we   if we == READ, then data used to return
              information back to CPU

              if we == WRITE, then data used to
              update Cache/DRAM
*/
void accessMemory(address addr, word* data, WriteEnable we)
{
  /* Declare variables here */
  int success = 0;
  unsigned int blockReplace = 0;

  address dirtyAddress = 0;

  unsigned int LRUValueInit = 1;
  unsigned int LRUValueCompare = 0;
  unsigned int LRUCycler = 0;


  

  int accessBytes;
    if(block_size == 4){
      accessBytes = 2;
    }
    else if(block_size == 8){
      accessBytes = 3;
    }
    else if (block_size == 16){
      accessBytes = 4;
    }
    else if (block_size == 32){
      accessBytes = 5;
    }


  unsigned int indexbits;
    if(set_count == 1){
      indexbits = 0;
    }
    else if(set_count == 2){
      indexbits = 1;
    }
    else if(set_count == 4){
      indexbits = 2;
    }
    else if(set_count == 8){
      indexbits = 3;
    }
    else if(set_count == 16){
      indexbits = 4;
    }

  unsigned int offsetbits; 
    if(block_size == 4){
      offsetbits = 0;
    }
    else if(block_size == 8){
      offsetbits = 1;
    }
    else if(block_size == 16){
      offsetbits = 2;
    }
    else if (block_size == 32){
      offsetbits = 3;
    }

  

  unsigned int tagbits = 30 - (indexbits + offsetbits);

  unsigned int offset = addr << (tagbits + indexbits); 
               offset = offset >> (tagbits + indexbits + 2);
               offset = offset * 4;


  unsigned int index = addr << tagbits; 
               index = index >> (tagbits + offsetbits + 2);

  

  unsigned int tag = addr >> (indexbits + offsetbits + 2);

 


  /* handle the case of no cache at all - leave this in */
  if(assoc == 0) {
    accessDRAM(addr, (byte*)data, WORD_SIZE, we);
    return;
  }

  /*
  You need to read/write between memory (via the accessDRAM() function) and
  the cache (via the cache[] global structure defined in tips.h)

  Remember to read tips.h for all the global variables that tell you the
  cache parameters

  The same code should handle random, LFU, and LRU policies. Test the policy
  variable (see tips.h) to decide which policy to execute. The LRU policy
  should be written such that no two blocks (when their valid bit is VALID)
  will ever be a candidate for replacement. In the case of a tie in the
  least number of accesses for LFU, you use the LRU information to determine
  which block to replace.

  Your cache should be able to support write-through mode (any writes to
  the cache get immediately copied to main memory also) and write-back mode
  (and writes to the cache only gets copied to main memory when the block
  is kicked out of the cache.

  Also, cache should do allocate-on-write. This means, a write operation
  will bring in an entire block if the block is not already in the cache.

  To properly work with the GUI, the code needs to tell the GUI code
  when to redraw and when to flash things. Descriptions of the animation
  functions can be found in tips.h
  */

  /* Start adding code here */
  success = 0;

  if (policy == LRU) {
    if(cache[index].block[0].lru.value == 0){

    
		  for (int i = 0; i < assoc; i++){

    
			  cache[index].block[i].lru.value = LRUValueInit;
        LRUValueInit++;
      }
      LRUValueInit = 1;
    }

	}



 if(we == READ){ // read flag

 
    for(int i = 0; i < assoc; i++){ // Read Hit
      if((cache[index].block[i].tag == tag) && cache[index].block[i].valid == 1 ){ 
       
        highlight_offset(index, i, offset, HIT);
        success = 1;
        memcpy(data, &cache[index].block[i].data[offset], 4);
        if (policy == LRU) {
          if(cache[index].block[i].lru.value == assoc){

          }
          else{
            LRUValueCompare = cache[index].block[i].lru.value;
            for(int j = 0; j < assoc; j++){
              if(cache[index].block[j].lru.value > LRUValueCompare){
                cache[index].block[j].lru.value--;
              }
            }
            cache[index].block[i].lru.value = assoc;
          }


        }
     }
    }

    if(success == 0){ // Read miss

      if (policy == LRU) {
				for (int i = 0; i < assoc; i++){
					if(cache[index].block[i].lru.value == 1){
						blockReplace = i;
            LRUValueCompare = 1;
						for(int j = 0; j < assoc; j++){
              if(cache[index].block[j].lru.value > 1){
            
                cache[index].block[j].lru.value--;
              }
            }
            cache[index].block[i].lru.value = assoc;
            LRUCycler++;
					}
          if(LRUCycler == 1){
            LRUCycler = 0;
            break;
          }
        }
			}


      if(policy == RANDOM){
          blockReplace = randomint(assoc);
      }

      if(cache[index].block[blockReplace].dirty == DIRTY){
				dirtyAddress = cache[index].block[blockReplace].tag << (indexbits + offsetbits) + (index << offsetbits); 
        if (block_size == 16){
           accessDRAM(dirtyAddress, cache[index].block[blockReplace].data, QUADWORD_SIZE, WRITE);
        }
        else if(block_size == 8){
            accessDRAM(dirtyAddress, cache[index].block[blockReplace].data, DOUBLEWORD_SIZE, WRITE);
        }
        else if(block_size == 4){
            accessDRAM(dirtyAddress, cache[index].block[blockReplace].data, WORD_SIZE, WRITE);
        }
        else if(block_size == 32){
          accessDRAM(dirtyAddress, cache[index].block[blockReplace].data, OCTWORD_SIZE, WRITE);
        }
				
			}

        
        
        
        if (block_size == 16){
           accessDRAM(addr, cache[index].block[blockReplace].data, QUADWORD_SIZE, we);
        }
        else if(block_size == 8){
            accessDRAM(addr, cache[index].block[blockReplace].data, DOUBLEWORD_SIZE, we);
        }
        else if(block_size == 4){
            accessDRAM(addr, cache[index].block[blockReplace].data, WORD_SIZE, we);
        }
        else if(block_size == 32){
          accessDRAM(addr, cache[index].block[blockReplace].data, OCTWORD_SIZE, we);
        }
        
        
        highlight_block(index, blockReplace);
        highlight_offset(index, blockReplace, offset, MISS);
        cache[index].block[blockReplace].valid = 1;
        cache[index].block[blockReplace].dirty = VIRGIN;
        cache[index].block[blockReplace].tag = tag;
        memcpy( data, &cache[index].block[blockReplace].data[offset], 4);
    }
      
  }
  
    
  

  else{ // write flag

      if (memory_sync_policy == WRITE_THROUGH){ // write through hit
        for(int i = 0; i < assoc; i++){
          if((cache[index].block[i].tag == tag) && cache[index].block[i].valid == 1 ){
       
            highlight_offset(index, i, offset, HIT);
            cache[index].block[i].dirty = VIRGIN;
            success = 1;
            memcpy(&cache[index].block[i].data[offset], data , 4);
            

            if (block_size == 16){
               accessDRAM(addr, cache[index].block[blockReplace].data, QUADWORD_SIZE, we);
            }
            else if(block_size == 8){
                accessDRAM(addr, cache[index].block[blockReplace].data, DOUBLEWORD_SIZE, we);
            }
            else if(block_size == 4){
                accessDRAM(addr, cache[index].block[blockReplace].data, WORD_SIZE, we);
            }
            else if(block_size == 32){
                accessDRAM(addr, cache[index].block[blockReplace].data, OCTWORD_SIZE, we);
            }

            if (policy == LRU) {
              if(cache[index].block[i].lru.value == assoc){

              }
              else{
                  LRUValueCompare = cache[index].block[i].lru.value;
                  for(int j = 0; j < assoc; j++){
                    if(cache[index].block[j].lru.value > LRUValueCompare){
                        cache[index].block[j].lru.value--;
                    }
                  }
                  cache[index].block[i].lru.value = assoc;
              }



            }
          }
        }
      

      if(success == 0){ // write through miss
          if (policy == LRU) {
				    for (int i = 0; i < assoc; i++){
					    if(cache[index].block[i].lru.value == 1){
					    	blockReplace = i;
                LRUValueCompare = 1;
					      for(int j = 0; j < assoc; j++){
                  if(cache[index].block[j].lru.value > 1){
                  cache[index].block[j].lru.value--;
                  }
                }
                cache[index].block[i].lru.value = assoc;
                LRUCycler++;
					    }
              if(LRUCycler == 1){
                LRUCycler = 0;
                break;
              }
              
            }
			    }  

          if(policy == RANDOM){
              blockReplace = randomint(assoc);
          }

          if (block_size == 16){
            accessDRAM(addr, cache[index].block[blockReplace].data, QUADWORD_SIZE, READ);
          }
          else if(block_size == 8){
              accessDRAM(addr, cache[index].block[blockReplace].data, DOUBLEWORD_SIZE, READ);
          }
          else if(block_size == 4){
              accessDRAM(addr, cache[index].block[blockReplace].data, WORD_SIZE, READ);
          }
          else if(block_size == 32){
            accessDRAM(addr, cache[index].block[blockReplace].data, OCTWORD_SIZE, READ);
          }

            highlight_block(index, blockReplace);
            highlight_offset(index, blockReplace, offset, MISS);
            cache[index].block[blockReplace].valid = 1;
            cache[index].block[blockReplace].tag = tag;
            cache[index].block[blockReplace].dirty = VIRGIN;
            memcpy( &cache[index].block[blockReplace].data[offset], data,  4);


        }

    }

    else{ //write back

       for(int i = 0; i < assoc; i++){ // write back hit

          if((cache[index].block[i].tag == tag) && cache[index].block[i].valid == 1 ){ 
                memcpy( &cache[index].block[i].data[offset], data,  4);
                cache[index].block[i].dirty = DIRTY;
                success = 1;

                if (policy == LRU) {
                  if(cache[index].block[i].lru.value == assoc){

                  }
                  else{
                    LRUValueCompare = cache[index].block[i].lru.value;
                    for(int j = 0; j < assoc; j++){
                      if(cache[index].block[j].lru.value > LRUValueCompare){
                        cache[index].block[j].lru.value--;
                      }
                    }
                    cache[index].block[i].lru.value = assoc;
                  }
                }

          }


       }

      if(success == 0){ // write back miss

          if (policy == LRU) {
				    for (int i = 0; i < assoc; i++){
				    	if(cache[index].block[i].lru.value == 1){
						   blockReplace = i;
               LRUValueCompare = 1;
						   for(int j = 0; j < assoc; j++){
                  if(cache[index].block[j].lru.value > 1){
                  cache[index].block[j].lru.value--;
                 }
                }
                cache[index].block[i].lru.value = assoc;
                LRUCycler++;
					    }
              if(LRUCycler == 1){
                LRUCycler = 0;
                break;
              }
              
            }
			    }

          if(policy == RANDOM){
              blockReplace = randomint(assoc);
          }

           if(cache[index].block[blockReplace].dirty == DIRTY){
				      dirtyAddress = cache[index].block[blockReplace].tag << (indexbits + offsetbits) + (index << offsetbits); 
            
              if (block_size == 16){
                accessDRAM(dirtyAddress, cache[index].block[blockReplace].data, QUADWORD_SIZE, WRITE);
              }
              else if(block_size == 8){
                accessDRAM(dirtyAddress, cache[index].block[blockReplace].data, DOUBLEWORD_SIZE, WRITE);
              }
              else if(block_size == 4){
                accessDRAM(dirtyAddress, cache[index].block[blockReplace].data, WORD_SIZE, WRITE);
              }
              else if(block_size == 32){
                accessDRAM(dirtyAddress, cache[index].block[blockReplace].data, OCTWORD_SIZE, WRITE);
              }
				
			    }


          cache[index].block[blockReplace].valid = 1;
          cache[index].block[blockReplace].dirty = VIRGIN;
				  cache[index].block[blockReplace].tag = tag;
          
          if (block_size == 16){
             accessDRAM(addr, cache[index].block[blockReplace].data, QUADWORD_SIZE, READ);
          }
          else if(block_size == 8){
              accessDRAM(addr, cache[index].block[blockReplace].data, DOUBLEWORD_SIZE, READ);
          }
          else if(block_size == 4){
              accessDRAM(addr, cache[index].block[blockReplace].data, WORD_SIZE, READ);
          }
          else if(block_size == 32){
              accessDRAM(addr, cache[index].block[blockReplace].data, OCTWORD_SIZE, READ);
          }
          memcpy( &cache[index].block[blockReplace].data[offset], data,  4);


      }


    }

  }
}


