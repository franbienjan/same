/// WINDOWS


#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>

#include <errno.h>
#include "rabin_polynomial.c"
//#include "sha1.c"
//#include "rabin_polynomial.h"
//#include "rabin_polynomial_constants.h"


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
char *chunk_storage_dir = "./chunkstempofolder"; //"chunkstempofolder";

int chunk_storage_exists = 0;
int file_count = 0;

// info per file okay
// might have to free some at some point, once siguro nawrite na yung kelangan.
struct files{
	char filename[300];
	char filedir[600];
	char chunkID[50];
	struct files* ptr;
};

struct files *root;




char* GetFileDirectory(char *str){
	char *p=strchr(str,'|');
	int i = (int)(p - str);
	char string[i];
	strncpy(string,str,i);
	//string[i]=0;
	char *temp=string;
	return temp;
}

char* GetFileName(char str[]){
	int i;
	for(i=strlen(str)-1; i>0; i--){
		if(str[i] == '/'){
			break;
		}
	}
	//char to[200];
	//strcpy(to,str+(i+1));
	return str+(i+1);
	//return strrchr(str,'/')+1;
}

char* GetFileExtension(char *str){
	return strrchr(str,'.');
}


void getFileNames(char *string){
	//DIR *dir , *dorf;
	//int isDir = 0;
	//struct dirent *ent;
	struct files *t1;
	char fn[100],tfold[300];
	int i;
	
	//printf("FILECOUNT: %d\n",file_count);

	root = (struct files *)malloc(sizeof(struct files));
	t1 = root;
	
	
	printf("string = %s\n",string);
	
	FILE *fp=fopen(string,"r");
	if(!fp){
		printf("NO SUCH FILE");
		return;
	}
	char str[512];
	char fDir[500], fileName[300], fExt[15]; ///////////////// limited sizes
	while(fgets(str,512,fp)!=NULL){
		printf("uloha\n");
	
	// fDir
		char strhld[500];
		strcpy(strhld,str);
		printf("strhld: %s\n",strhld);
		for(i=0;i<strlen(strhld);i++){
			if(strhld[i] == '|'){
				break;
			}
		}
		strncpy(fDir, strhld, i);
		fDir[i] = '\0';
		memset(strhld, 0, sizeof strhld);
		
	// fileName
		strcpy(strhld,fDir);
		for(i=strlen(strhld)-1;i>0;i--){
			if(strhld[i] == '/'){
				break;
			}
		}
		strncpy(fileName,strhld+(i+1),strlen(strhld)-i);
		fileName[strlen(strhld)-i] = '\0';
		memset(strhld, 0, sizeof strhld);
		
	//fExt
		strcpy(strhld,fileName);
		for(i=strlen(strhld)-1;i>0;i--){
			if(strhld[i] == '.'){
				break;
			}
		}
		strncpy(fExt,strhld+i,strlen(strhld)-i);
		fExt[strlen(strhld)-i] = '\0';
		printf("fExt %s\n",fExt);
		
		printf("fDir: %s \nfileName: %s \nfExt: %s\n",fDir,fileName,fExt);
		//maybe add a.out
		if(strcmp(fExt,".DS_Store") && strcmp(fExt,chunk_storage_dir) && strcmp(fExt,".bmp") && strcmp(fExt,".png") && strcmp(fExt,".jpg") && strcmp(fExt,".jpeg") && strcmp(fExt,".gif") && strcmp(fExt,".zip") && strcmp(fExt,".rar") && strcmp(fExt,".tar") && strcmp(fExt,".tiff") && strcmp(fExt,".svg") && strcmp(fExt,".mp3") && strcmp(fExt,".mp4") && strcmp(fExt,".mkv") && strcmp(fExt,".exe") && strcmp(fileName,"a.out")){
			printf("read: %s\n",fileName); // print all files accepted
			strcpy(t1->filedir,fDir);
			strcpy(t1->filename,fileName);
			t1->ptr = (struct files *)malloc(sizeof(struct files));
			t1 = t1->ptr;
			file_count += 1;
			printf("fc: %d\n",file_count);
		}
		else{
			printf("not read: %s\n",fileName); // print all files accepted
		}
	}
	printf("hurpdup\n");
	fclose(fp);
	
	printf("hey file count: %d\n",file_count);
	
	
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
		strcat(concat,temp->filedir);
		printf("file read: %s\n",temp->filedir);
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





//int main(){

//char *str = "./"; ///// previous test, pachange nalang ng value nito, salamat!

int Chy(char *str){
    
	// get contents of file agent
    getFileNames(str);
    
	// rabin fingerprinting, SHA-1, extreme binning
    rabinChunking();
	
	return 0;
    
}