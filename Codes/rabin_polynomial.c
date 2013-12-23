/*
 * rabin_polynomial.c
 * 
 * Created by Joel Lawrence Tucci on 09-March-2011.
 * 
 * Copyright (c) 2011 Joel Lawrence Tucci
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of the project's author nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <string.h>
#include <malloc.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif


#include "rabin_polynomial.h"
#include "rabin_polynomial_constants.h"
#include "sha1.h"

uint64_t rabin_polynomial_prime=RAB_POLYNOMIAL_REM;

unsigned int rabin_sliding_window_size=RAB_POLYNOMIAL_WIN_SIZE;

unsigned int rabin_polynomial_max_block_size=RAB_MAX_BLOCK_SIZE;
unsigned int rabin_polynomial_min_block_size=RAB_MIN_BLOCK_SIZE;

unsigned int rabin_polynomial_average_block_size=RAB_POLYNOMIAL_AVG_BLOCK_SIZE;

int rabin_poly_init_completed=0;

char minimum_sha[50];
int min_sha_check = 0; //revision (DO NOT DELETE): stop minimum_sha check once first write is made. // might revisit renaming
char fn[100];
int mindex = 9000;
int chunkcount = 0;
int chunk_limit = 150; //variable that limits chunking

uint64_t *polynomial_lookup_buf;


void test(int err, const char* msg) { // asterisk
    if (err) {
        fprintf(stderr, "%s: error %d\n", msg, err);
        exit(1);
    }
}

// HASHES CHUNK
char * hashing(char *string){ // asterisk
    //char *string;
    char temp[20];
    static char hash[100];
    int n, i, len;
    static unsigned char buf[20] = {0};
    SHA1Context sha;

    memset(buf, 0, sizeof buf);
    memset(hash, 0, sizeof hash);
    //string = "The quick brown fox jumps over the lazy dog";
    
    len = strlen(string);
    test(SHA1Reset(&sha), "SHA1Reset");
    test(SHA1Input(&sha, string, len), "SHA1Input");
    test(SHA1Result(&sha, buf), "SHA1Result");
	
    int int_hash = 0;
    for (i=0; i<20; ++i){
        sprintf(temp, "%02X", buf[i]);
        int_hash += buf[i];
        strcat(hash,temp);
    }
	
	//printf("____________________________________________\n");
	//printf("minimumsha = %s\n",minimum_sha);
	
	// get minimum hash
	if(min_sha_check == 0 && int_hash < mindex){
		mindex = int_hash;
		strcpy(minimum_sha,hash);
	}
    
    return hash;
}



// SPLITS INTO CHUNKS
void split_into_chunks(FILE *out_file, char file_dest[], FILE *fin, int c, int val[], int partitions){ // asterisk
    // chunk directory = hash_filename.bin
	long i = 0, j = 0;
	int length,counter = 0;
	char *chunkptr;
	char split[300],shahash[50];
	char ** hashes_array = malloc(chunk_limit * sizeof(char*)); //malloc(partitions * sizeof(char*));
	int chunk = 18000; //?
	int cur_chunk_count = 0; // current chunk count, for limiting
	
	// reset static/global variables
	chunkcount = 0;
	mindex = 9000;
	memset(minimum_sha, 0, sizeof minimum_sha);
	chunkptr = malloc(chunk);
	
	
	FILE *bin;
	char concat[500];
	int bin_count = 0;
	chunkcount = partitions + 1;
	
	
	
	// THE WHILE LOOP THAT SPLITS
	while(1){
		long f;
		chunk = val[j];
		//printf("valval: %d\n",val[j]);
		f = fread(chunkptr, 1, chunk, fin);//
		//printf("f = %d\n",(int) f);//
		//printf("cur_poly->length = %d \n",val[j]);//
		j+=1;
		
		if(f > 0){
			//sprintf(split, "%s.%ld.bin", file_dest, ++i); //
			//printf("____________________\n");
            strcpy(shahash,hashing(chunkptr));
            sprintf(split, "%s%s.bin",file_dest,shahash); // might add .filetype some other time?!?!?!? ito yung pampalit sa split (chunk name == chunk id)
			out_file = fopen(split, "wb"); //
			
			fwrite(chunkptr, 1, (int) f, out_file);
			fclose(out_file);
			
			hashes_array[j-1] = malloc(50 * sizeof(char));
			strcpy(hashes_array[j-1],shahash);
            memset(shahash, 0, sizeof shahash);
			cur_chunk_count += 1;
			
			if(cur_chunk_count % chunk_limit == 0){
				printf("HURDURHURDUR\n");
				bin_count += 1;
				int k = 0;
				
				if(cur_chunk_count == chunk_limit){ // EXTREME BINNING
					strcpy(concat,"../chunkstempofolder/");
					strcat(concat,"__bin__");
					strcat(concat,fn);
					strcat(concat,"__");
					strcat(concat,minimum_sha);
					strcat(concat,".bin");
					//printf("concat: %s\n",concat);
				}
				bin = fopen(concat, "a");
				
				for(k=0;k<chunk_limit;k++){
					if(strcmp(hashes_array[k],minimum_sha)){ // if not primary
						fprintf(bin,"%s\n",hashes_array[k]); //might add chunk size later? (but it's pretty useless though)
					}
				}
				
				min_sha_check = 1;
				fclose(bin);
				j = 0;
			}
			
			
		}else break;
	}
	
	if(cur_chunk_count % chunk_limit != 0){
		int t = 0;
		int k = 0;
		
		if(cur_chunk_count <= 2){ /////////////////////////////////// omg copy file blablabla no need to chunk, tas yung name lang magiiba okay??? (append) mamaya na haha
			char holder[100];
			strcpy(holder,hashes_array[0]);
			if(cur_chunk_count == 2){
				strcat(holder,"_");
				strcat(holder,hashes_array[1]);
			}
			//strcpy(temp->chunkID,holder);
		}
		else{
			if(cur_chunk_count < chunk_limit){
				t = cur_chunk_count;
				strcpy(concat,"../chunkstempofolder/");
				strcat(concat,"__bin__");
				strcat(concat,fn);
				strcat(concat,"__");
				strcat(concat,minimum_sha);
				strcat(concat,".bin");
			}
			else{
				t = chunkcount-(bin_count*chunk_limit);
			}
			bin = fopen(concat, "a");
			
			for(k=0;k<t;k++){ //0'd hashes
				if(strcmp(hashes_array[k],minimum_sha)){ // if not primary
					fprintf(bin,"%s\n",hashes_array[k]); //might add chunk size later? (but it's pretty useless though)
				}
			}
			fclose(bin);
		}
	}
	
	printf("chunkcount = %d, cur_chunk_count = %d\n",chunkcount,cur_chunk_count);
	min_sha_check = 0;
}






/**
 * Prints the list of rabin polynomials to the given file
 */
void print_rabin_poly_list_to_file(FILE *out_file, struct rabin_polynomial *poly, char file_dest[], FILE *fin, char file_name[]) { // char **file_dest & FILE :)
    
    struct rabin_polynomial *cur_poly=poly;
    
	int counter = 0;
	
	while(cur_poly != NULL) {
		counter += 1;
		cur_poly=cur_poly->next_polynomial;
	}
	//printf("cntr = %d\n",counter); // counter check
	cur_poly=poly;
	
	int lengthvalues[counter],j=0;
	
    while(cur_poly != NULL) {
		lengthvalues[j] = (int) cur_poly->length; //
		j += 1; //
		
        print_rabin_poly_to_file(out_file,cur_poly,1);
        cur_poly=cur_poly->next_polynomial;
    }
	
    
	
	// For checking if values are same.
	/*
	int i=0;
	for(i=0;i<counter;i++){
		printf("%d: %d\n",i,lengthvalues[i]);
	}
	*/
    
    
	strcpy(fn,file_name);
	split_into_chunks(out_file,file_dest,fin,counter,lengthvalues,j-1); // added function
    
}

/**
 * Prints a given rabin polynomial to file in the format:
 * start,length hash
 */
void print_rabin_poly_to_file(FILE *out_file, struct rabin_polynomial *poly,int new_line) {
    
    if(poly == NULL)
        return;
    
   // fprintf(stdout, "%llu,%u %llu",poly->start,poly->length,poly->polynomial);
    //fprintf(out_file, "%llu,%u %llu",poly->start,poly->length,poly->polynomial); //
    ///// get thingy then get thing
    //if(new_line)
    //    fprintf(stdout, "\n");
        //fprintf(out_file, "\n");
}

/*
 * Initialize the algorithm with the default params.
 */
int initialize_rabin_polynomial_defaults() {
    
    
    
    if(rabin_poly_init_completed != 0)
        return 1; //Nothing to do
    
    polynomial_lookup_buf=malloc(sizeof(uint64_t)*RAB_POLYNOMIAL_MAX_WIN_SIZE);
    
    if(polynomial_lookup_buf == NULL) {
        fprintf(stderr, "Could not initialize rabin polynomial lookaside buffer, out of memory\n");
        return 0;
    }
    
    int index=0;
    uint64_t curPower=1;
    //Initialize the lookup values we will need later
    for(index=0;index<RAB_POLYNOMIAL_MAX_WIN_SIZE;index++) {
        //TODO check if max window size is a power of 2
        //and if so use shifters instead of multiplication
        polynomial_lookup_buf[index]=curPower;
        curPower*=rabin_polynomial_prime;
    }
    
    
    rabin_poly_init_completed=1;
    
    return 1;
    
}


/**
 * Modifies the average block size, checking to make sure it doesn't
 * go above the max or below the min
 */
void change_average_rabin_block_size(int increment_mode) {
    
    if(increment_mode && rabin_polynomial_average_block_size < rabin_polynomial_max_block_size) {
        rabin_polynomial_average_block_size++;
    } else if(!increment_mode && rabin_polynomial_average_block_size > rabin_polynomial_min_block_size) {
        rabin_polynomial_average_block_size--;
    }
}

/**
 * Initalizes the algorithm with the provided parameters
 */
int initialize_rabin_polynomial(uint64_t prime, unsigned max_size, unsigned int min_size, unsigned int average_block_size) {
    
    rabin_polynomial_prime=prime;
    rabin_polynomial_max_block_size=max_size;
    rabin_polynomial_min_block_size=min_size;
    rabin_polynomial_average_block_size=average_block_size;
    
    return initialize_rabin_polynomial_defaults();
    
    
}


/*
 * Generate a new fingerprint with the given info and add it to the tail //////////////////////// asterisk wuz hir
 */
struct rabin_polynomial *gen_new_polynomial(struct rabin_polynomial *tail, uint64_t total_len, uint16_t length, uint64_t rab_sum) {
    
    struct rabin_polynomial *next=malloc(sizeof(struct rabin_polynomial));
    
    if(next == NULL) {
        fprintf(stderr, "Could not allocate memory for rabin fingerprint record!");
        return NULL;
    }
    
    if(tail != NULL)
        tail->next_polynomial=next;
    
    next->next_polynomial=NULL;
    next->start=total_len-length;
    next->length=length;
    next->polynomial=rab_sum;
    
    return next;
    
}

/*
 * Writes out the fingerprint list in binary form
 */
int write_rabin_fingerprints_to_binary_file(FILE *file,struct rabin_polynomial *head) {
    
    struct rabin_polynomial *poly=head;
    
    while(poly != NULL) {
        size_t ret_val=fwrite(poly, sizeof(struct rabin_polynomial), 1, file);
        
        if(ret_val == 0) {
            fprintf(stderr, "Could not write rabin polynomials to file.");
            return -1;
        }
        
        poly=poly->next_polynomial;
    }
    
    return 0;
}

/**
 * Reads a list of rabin fingerprints in binary form
 */
struct rabin_polynomial *read_rabin_polys_from_file_binary(FILE *file) {
    struct rabin_polynomial *head=gen_new_polynomial(NULL,0,0,0);
    struct rabin_polynomial *tail=head;
    
    if(head == NULL)
        return NULL;
    
    size_t polys_read=fread(head, sizeof(struct rabin_polynomial), 1, file);
    
    while(polys_read != 0 && tail != NULL) {
        struct rabin_polynomial *cur_poly=gen_new_polynomial(tail,0,0,0);
        fread(cur_poly, sizeof(struct rabin_polynomial), 1, file);
        tail=cur_poly;
    }
    
    return head;
}


/*
 * Deallocates the entire fingerprint list
 */
void free_rabin_fingerprint_list(struct rabin_polynomial *head) {
    
    struct rabin_polynomial *cur_poly,*next_poly;
    
    cur_poly=head;
    
    while(cur_poly != NULL) {
        next_poly=cur_poly->next_polynomial;
        free(cur_poly);
        cur_poly=next_poly;
    }
    
}

/*
 * Gets the list of fingerprints from the given file
 */

struct rabin_polynomial *get_file_rabin_polys(FILE *file_to_read) {
    
    initialize_rabin_polynomial_defaults();
    
	//
	//char splt[300];
	//
	
	
    struct rab_block_info *block=NULL;
    char *file_data=malloc(RAB_FILE_READ_BUF_SIZE);
    
    if(file_data == NULL) {
        fprintf(stderr,"Could not allocate buffer for reading input file to rabin polynomial.\n");
        return NULL;
    }
    
    ssize_t bytes_read=fread(file_data,1,RAB_FILE_READ_BUF_SIZE,file_to_read);
    
    while(bytes_read != 0) {
        block=read_rabin_block(file_data,bytes_read,block); //////////////////////////////////// asterisk wuz hir
        bytes_read=fread(file_data,1,RAB_FILE_READ_BUF_SIZE,file_to_read);
		//sprintf(splitfn,"./File Agent/", ofn, ++part); //
    }
    
    free(file_data);
    struct rabin_polynomial *head=block->head;
    free(block);
    return head;
}

/**
 * Allocates an empty block
 */
struct rab_block_info *init_empty_block() {
    
    initialize_rabin_polynomial_defaults();
	struct rab_block_info *block=malloc(sizeof(struct rab_block_info));
    if(block == NULL) {
        fprintf(stderr,"Could not allocate rabin polynomial block, no memory left!\n");
        return NULL;
    }
	
	block->head=gen_new_polynomial(NULL,0,0,0);
    
	if(block->head == NULL)
        return NULL; //Couldn't allocate memory
    
	block->tail=block->head;
	block->cur_roll_checksum=0;
	block->total_bytes_read=0;
	block->window_pos=0;
	block->current_poly_finished=0;
    
    block->current_window_data=malloc(sizeof(char)*rabin_sliding_window_size);
    
	if(block->current_window_data == NULL) {
	    fprintf(stderr,"Could not allocate buffer for sliding window data!\n");
	    free(block);
	    return NULL;
	}
    int i;
	for(i=0;i<rabin_sliding_window_size;i++) {
	    block->current_window_data[i]=0;
	}
    
    return block;
}

/**
 * Reads a block of memory and generates a rabin fingerprint list from it.
 * Since most of the time we will not end on a border, the function returns
 * a block struct, which keeps track of the current blocksum and rolling checksum
 */
struct rab_block_info *read_rabin_block(void *buf, ssize_t size, struct rab_block_info *cur_block) {
    struct rab_block_info *block;
    
    if(cur_block == NULL) {
        block=init_empty_block();
        if(block == NULL)
            return NULL;
    }
    
    else {
     	block=cur_block;
    }
    //We ended on a border, gen a new tail
    if(block->current_poly_finished) {
        struct rabin_polynomial *new_poly=gen_new_polynomial(NULL,0,0,0);
        block->tail->next_polynomial=new_poly;
        block->tail=new_poly;
        block->current_poly_finished=0;
    }
   

    ssize_t i;
    for(i=0;i<size;i++) {
    	char cur_byte=*((char *)(buf+i));
        char pushed_out=block->current_window_data[block->window_pos];
        block->current_window_data[block->window_pos]=cur_byte;
        block->cur_roll_checksum=(block->cur_roll_checksum*rabin_polynomial_prime)+cur_byte;
        block->tail->polynomial=(block->tail->polynomial*rabin_polynomial_prime)+cur_byte;
        block->cur_roll_checksum-=(pushed_out*polynomial_lookup_buf[rabin_sliding_window_size]);
        
        block->window_pos++;
        block->total_bytes_read++;
        block->tail->length++;
        
        if(block->window_pos == rabin_sliding_window_size) //Loop back around
            block->window_pos=0;
        
        //If we hit our special value or reached the max win size create a new block
        if((block->tail->length >= rabin_polynomial_min_block_size && (block->cur_roll_checksum % rabin_polynomial_average_block_size) == rabin_polynomial_prime)|| block->tail->length == rabin_polynomial_max_block_size) {
            block->tail->start=block->total_bytes_read-block->tail->length;
            struct rabin_polynomial *new_poly=gen_new_polynomial(NULL,0,0,0);
            block->tail->next_polynomial=new_poly;
            block->tail=new_poly;
            
            if(i==size-1)
                block->current_poly_finished=1;
        }
    }
    
    return block;
    
}