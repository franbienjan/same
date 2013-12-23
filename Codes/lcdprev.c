/// WINDOWS


#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>

#include <errno.h>
#include "rabin_polynomial.c"
#include "sha1.c"
//#include "rabin_polynomial.h"
//#include "rabin_polynomial_constants.h"

#ifdef _WIN32 // note the underscore: without it, it's not msdn official!
    #define PLATFORM 0
#elif __unix__
    #define PLATFORM 1
#elif __linux__
    #define PLATFORM 2
#elif __APPLE__
    #define PLATFORM 3
#endif

// to compile: gcc rabin_polynomial.c lcd.c

// [] Implement on one folder.
// [] Implement for multiple folders w/in File Agent.
// [] Write "future" todo list.
// [] 0 filesize hangs!!! D:
// [] how to know if file has already been chunked?!?!?! (maybe some file from somewhere? Master Server?)
// [] cleanup code: separate rabin from chunking for more flexibility
// [] Place lcd w/in File Agent okay? (do necessary directory edits :( )
// [] Possible problem: SHA-1 file-reading. (EOF?)
// [] RESTORE FILES FROM CHUNKS
// [] lolololol

//char *file_agent_dir = "./File Agent/";
char *file_agent_dir = "./";
//char *file_agent_dir_system = "\"File Agent\"\\";
char *file_storage_dir = "./Storage/";
char *chunk_storage_dir = "../chunkstempofolder"; //"chunkstempofolder";

int chunk_storage_exists = 0;
int file_count = 0;

// info per file okay
// might have to free some at some point, once siguro nawrite na yung kelangan.
struct files{
	char filename[100];
	char filedir[300];
	char chunkID[50];
	struct files* ptr;
};

struct files *root;




char* GetFileDirectory(char *str){
	char *p=strchr(str,'|');
	int i = (int)(p - str);
	char *string=malloc(i);
	strncpy(string,str,i);
	string[i]=0;
	return string;
}

char* GetFileName(char *str){
	return strrchr(str,'/')+1;
}

char* GetFileExtension(char *str){
	return strrchr(str,'.');
}


void getFileNames(char *string){
	//DIR *dir , *dorf;
	//int isDir = 0;
	//struct dirent *ent;
	struct files *t1,*t2;
	char fn[100],tfold[300];
	int i;
	
	//printf("FILECOUNT: %d\n",file_count);

	root = (struct files *)malloc(sizeof(struct files));
	t1 = root;
	//file_agent_dir = string;

	
	/*
	// look in given directory
	if ((dir = opendir (string)) != NULL) {
		// loop through each file in directory
		while ((ent = readdir (dir)) != NULL) {
			
			// check if chunk directory exists
			if(!strcmp(ent->d_name,chunk_storage_dir)){
				chunk_storage_exists = 1;
			}
			
			// add subfolders feature here someday
			
			// remove current and previous folders from file choices
			if(strcmp(ent->d_name,".") && strcmp(ent->d_name,"..")){ // "if =="... due to binary values... strcmp is weird
				strcpy(fn,ent->d_name);
				
				// find last stop for filetype segregation
				for(i = 99; i >= 0; i--){ // find a more efficient way later on
					if(fn[i] == '.'){
						break;
					}
				}

				// check if what is read is file or directory
				//printf("string: %s\tent->d_name: %s\n",string,ent->d_name);
				strcpy(tfold,string);
				strcat(tfold,"/");
				strcat(tfold,ent->d_name);
				//printf("tfold: %s\n",tfold);

				if((dorf = opendir(tfold)) != NULL){
				//	printf("THIS IS A FOLDER: %s\n",ent->d_name);
					isDir = 1;
				}
				closedir(dorf);
				
				// save select names into RAM aka disregard ff filetypes
				if(isDir == 0 && strcmp(ent->d_name,".DS_Store") && strcmp(ent->d_name,chunk_storage_dir) && strcmp(fn+i,".bmp") && strcmp(fn+i,".png") && strcmp(fn+i,".jpg") && strcmp(fn+i,".jpeg") && strcmp(fn+i,".gif") && strcmp(fn+i,".zip") && strcmp(fn+i,".rar") && strcmp(fn+i,".tar") && strcmp(fn+i,".tiff") && strcmp(fn+i,".svg") && strcmp(fn+i,".mp3") && strcmp(fn+i,".mp4") && strcmp(fn+i,".mkv") && strcmp(fn+i,".exe") && strcmp(ent->d_name,"a.out")){
					//printf("%s\n",fn); // print all files accepted
					strcpy(t1->filename,fn);
					t2 = (struct files *)malloc(sizeof(struct files));
					t1->ptr = t2;
					t1 = t2;
					file_count += 1;
				}
				/*else{
					printf("notread: %s\n",ent->d_name);
				}*/
				
				/*
				memset(fn, 0, sizeof fn);
			}
			isDir = 0;
		}
	  closedir (dir);
	} 
	else {
		perror ("");
		//return EXIT_FAILURE;
	}

	*/
	//printf("______________________________________________\n\n");// separate files from other output
	
	FILE *fp=fopen(string,"r");
	if(!fp){
		printf("NO SUCH FILE");
		return;
	}
	char str[512];
	char *fDir, *fileName, *fExt;
	while(fgets(str,512,fp)!=NULL){
		fDir=GetFileDirectory(str);
		fileName=GetFileName(fDir);
		fExt=GetFileExtension(fileName);
		if(strcmp(fExt,".DS_Store") && strcmp(fExt,chunk_storage_dir) && strcmp(fExt,".bmp") && strcmp(fExt,".png") && strcmp(fExt,".jpg") && strcmp(fExt,".jpeg") && strcmp(fExt,".gif") && strcmp(fExt,".zip") && strcmp(fExt,".rar") && strcmp(fExt,".tar") && strcmp(fExt,".tiff") && strcmp(fExt,".svg") && strcmp(fExt,".mp3") && strcmp(fExt,".mp4") && strcmp(fExt,".mkv") && strcmp(fExt,".exe") && strcmp(fExt,"a.out")){
			//printf("%s\n",fn); // print all files accepted
			strcpy(t1->filedir,fDir);
			strcpy(t1->filename,filename);
			t2 = (struct files *)malloc(sizeof(struct files));
			t1->ptr = t2;
			t1 = t2;
			file_count += 1;
		}
	}
	fclose(fp);
	
	
	
	
	// "are the filenames saved in RAM?" test
	
	t1 = root;
	
	for(i=0; i<file_count; i++){
		printf("%s\n",t1->filename);
		t1 = t1->ptr;
	}
	
}








void rabinChunking(){

	// CONSTANTS: (ESTIMATES FOR SAM)
	// RABIN WINDOW - 31 BYTES
	// RABIN AVG BLOCK - 8KB (8192 bytes)
	// RABIN TEMP MIN - 4KB (4096 bytes)
	// RABIN TEMP MAX - 16KB (16384 bytes)

	// [x] make chunk directory
	// [x] read files
	// [x] rabin algorithm
    // [x] chunk
    // [x] save chunks in a (hidden) folder w/in containers
    // [x] SHA-1 function to compute for chunk IDs
	
	// [x] determine primary fingerprint // HOW TO GET MINIMUM HASH?!?!? -- get minimum value. durr.
    // [x] primary/representative IDs must contain pointers to bin
	// [x] bin contains chunk ID, chunk sizes, and chunk addresses (.txt file, maybe?)
    // (one bin per file!)
    // [x] determine secondary fingerprint
    // [x] dump into disk
	// [x] piggyback small files

	
	char concat[500];
	struct files *temp,*del;
	FILE *fin,*fout;
	int i;
	
	temp = root;
	
	rabin_polynomial_min_block_size = 4096;
	rabin_polynomial_max_block_size = 16384;
	rabin_polynomial_average_block_size = 8192;
	
	
	
	// loop through each file to get fingerprint
	for(i=0; i<file_count; i++){
	
		// create chunk storage directory if it does not exist yet
		if(chunk_storage_exists == 0){
			//strcpy(concat,"md \"File Agent\"\\"); //////////edit later file_agent_dir_system
			strcpy(concat,"md "); //////////edit later file_agent_dir_system
			strcat(concat,"\"");
			strcat(concat,chunk_storage_dir);
			strcat(concat,"\"");
			system(concat);
			memset(concat, 0, sizeof concat);
			chunk_storage_exists = 1;
		}
		
		
		
		
		
		
		// read a particular file
		strcpy(concat,file_agent_dir);
		strcat(concat,"\\");
		//strcat(concat,"/");
		strcat(concat,temp->filename);
		printf("file read: %s\n",temp->filename);
		fin = fopen(concat, "rb+"); // made a code change here!!! initially r+ :)
		if(fin == NULL) {
			fprintf(stderr, "Cannot read file!\n");
		}
		
		
		// rabin algorithm + chunking
		struct rabin_polynomial *head = get_file_rabin_polys(fin);
		fclose(fin);
		free_rabin_fingerprint_list(head);
		
		// prints bin in stdout
		fin = fopen(concat, "rb");
		memset(concat, 0, sizeof concat);
		
		strcpy(concat,file_agent_dir);
		strcat(concat,"/");
		strcat(concat,chunk_storage_dir);
		strcat(concat,"/");
		
//printf("concat: %s\n",concat);

		//printf("concat: %s\n",concat);
        print_rabin_poly_list_to_file(fout,head,concat,fin,temp->filename);
		
		//printf("MINDEX = %d\nMIN HASH = %s\nchunkcount = %d\n",mindex,minimum_sha,chunkcount);
    
    
		// progress to next file + dynamic freeing
		del = temp;
		temp = temp->ptr;
		free(del);
		memset(concat, 0, sizeof concat);
    
    
		
	}
	
	
}





int main(){

char *str = "./"; ///// previous test, pachange nalang ng value nito, salamat!

//int Chy(char *str){
    
	// get contents of file agent
    getFileNames(str);
    
	// rabin fingerprinting, SHA-1, extreme binning
    rabinChunking();
	
	return 0;
    
}