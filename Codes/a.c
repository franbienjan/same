#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/types.h>
//#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include "sha1.c"

// searches the string 'str' in file 'fname'
int searchWord(char *fname, char *str) ;

// gets the line in 'initFiles.txt' with the 'str' string
char* getLine(char *str,FILE *f);

// prints out the changed files and puts in out.txt
void compareFiles(const char *name, int level,FILE *f);

// initializes the file list in initFiles.txt
void initFileList(const char *name, int level,FILE *f);

// returns the size of the file in bytes
off_t getSize(const char *filename);

// test function for getSha()
void test(int err, const char* msg);

// gets "sha-1"
char* getSha(char* fileDirectory, char* fileName);

long int convertUTF8ToUnicode(FILE *fileHandler, FILE *fOut, int c);
// initializes initFiles.txt
void callInit(char *str);
/*
 *	initFiles.txt - information of files
 *  out.txt - list of files to be deduplicated
 *
 */

FILE *file1;
FILE *file2;
typedef struct FolderList{
    char *fname;
    struct FolderList *next;
}FolderList;


void letGo(FolderList *f){
	FolderList *temp;
	do{
		temp=f->next;
		free(f);
		f=temp;
	}while(temp!=NULL);
}


char* getSha2(char* fileDirectory){
    char	buffer[20];
    char	eureka[100];
    int  	n,
    		i;

    static	unsigned char buf[20] = {0};
    SHA1Context sha;
    FILE 	*f;
    memset(eureka, 0, sizeof eureka);

	int size=getSize(fileDirectory);
    f=fopen(fileDirectory,"rb");
	
	char *string=malloc(size);

	if(!f) return;
	fseek(f, SEEK_SET, 0);
	strcpy(string,"");
	fread(string,1,size,f);
	fclose(f);
    if(size==0) strcpy(string,"");
	
    test(SHA1Reset(&sha), "SHA1Reset");
    test(SHA1Input(&sha, string, size), "SHA1Input");
    test(SHA1Result(&sha, buf), "SHA1Result");
	
    int int_hash = 0;
    for (i=0; i<20; ++i){
        sprintf(buffer, "%02X", buf[i]);
        int_hash += buf[i];
        strcat(eureka,buffer);
    }
	free(string);
	char *temp=eureka;
	return temp;

}

void initializeFileList(char *name, int level){
	DIR *dir;
	struct dirent *ent;
	char path[1024]="";
    char t[ 512 ] = "";
    int len;
	struct stat b;
	if (!(dir = opendir(name))) return;
    if (!(ent = readdir(dir))) return;
	do{
        // if folder
        if (ent->d_type == DT_DIR){
            if(strcmp(ent->d_name,"..")==0) len = snprintf(path, sizeof(path)-1, "%s", name);
            else{
				len = snprintf(path, sizeof(path)-1, "%s/%s", name, ent->d_name);
			}
            path[len] = 0;
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            initializeFileList(path,level+1);
        }
        else{
        	len = snprintf(path, sizeof(path)-1, "%s", name);
			char *ext=strrchr(ent->d_name, '.');			// gets extension of a file
            char file[strlen(path)+strlen(ent->d_name)+1];
            strcpy(file,path);
            strcat(file,"/");
            strcat(file,ent->d_name);
			stat(file, &b);
			
			
			char *sha=getSha2(file);
			
			/* file1 */
			fprintf(file1, "%s|", sha);
			
			fprintf(file2, "%s|",file);
			fprintf(file2, "%s|",sha);
			
			int size=getSize(file);
            fprintf(file1, "%d|", size);
			fprintf(file2, "%d|",size);

            strftime(t, 100, "%m%d%Y-%H:%M:%S", localtime(&b.st_ctime));
            fprintf(file1, "%s|",t);
			fprintf(file2, "%s|",t);

            strftime(t, 100, "%m%d%Y-%H:%M:%S", localtime(&b.st_mtime));
            //printf("%s|",t);
            fprintf(file1, "%s|",t);
			fprintf(file2, "%s|",t);

            strftime(t, 100, "%m%d%Y-%H:%M:%S", localtime(&b.st_atime));
            //printf("%s",t);
			fprintf(file2, "%s\n",t);
			strcat(t,"|");
            fprintf(file1, "%s||\"%s\"\n",t,file);
			
		}
			
			
    }while (ent = readdir(dir));

}


void raf() {
	int i=1,
        folderNum=0,
        folderChoice=0;
    DIR *dir;
    FolderList *folder, *head, *node, *lastNode;		//////////
    struct dirent *ent;
	FILE *f;
    do{
    	i=1;
    	folder=(FolderList*)malloc(sizeof(FolderList));
		head=folder;
		lastNode=NULL;

		if (!(dir = opendir("."))) return;
		if (!(ent = readdir(dir))) return;
	    do{

	    	if(ent->d_type==DT_DIR && strstr(ent->d_name,"Client")){
	    		if(i==1){
	    			head->fname=(char *)malloc(sizeof(char)*strlen(ent->d_name));
	    			strcpy(head->fname, ent->d_name);
	    		}else{
	    			node=(FolderList*)malloc(sizeof(FolderList));
	    			node->fname=(char *)malloc(sizeof(char)*strlen(ent->d_name));
	    			strcpy(node->fname, ent->d_name);
	    			head->next=node;
	    			head=head->next;
	    			head->next=NULL;
	    		}
	    		i++;
	    	}
	    }while(ent = readdir(dir));
	    i=1;
	    head=folder;
		
		/* lists the options */
	    while(head!=NULL){
	    	printf("\n[%d] Initialize %s",i++,head->fname);
	    	head=head->next;
	    }
	    head=folder;
	    folderNum=i-1;
	    printf("\n[%d] Exit\n: ",i++);
		///////////////////////
		
		
	    scanf("%d",&folderChoice);
		
		/* code according to choice */
	    if(folderChoice==(folderNum+1)) break;
		else if(folderChoice==0){
            printf("Cleared");
            FILE *ff;
            ff=fopen("hashcatalog.txt","w");
            fclose(ff);
            ff=fopen("cache.txt","w");
            fprintf(ff,"onwoief\n");
            fclose(ff);

            DIR *dir2;
            struct dirent *ent2;
            if(!(dir2 = opendir("./Outputs"))){ printf("-----------------------"); return;}
            if(!(ent2 = readdir(dir2))) return;
            do{
                if(strcmp(ent2->d_name,"..")!=0 && strcmp(ent2->d_name,".")!=0){
                    char tempStr[9+strlen(ent2->d_name)];
                    strcpy(tempStr,"./Outputs/");
                    strcat(tempStr,ent2->d_name);
                    remove(tempStr);
                }
            }while (ent2 = readdir(dir2));
    	}else{
			for(i=1 ; i<folderChoice ; i++) head=head->next;
			char temp[strlen(head->fname)+14];
			file1=fopen("initFiles.txt","w+");
			
            strcpy(temp,"./Outputs/out.");
            strcat(temp,head->fname);
            file2=fopen(temp,"w+");
			
			strcpy(temp,"./");
			strcat(temp,head->fname);
			
			initializeFileList(temp,0);
			fclose(file1);
			fclose(file2);
		}
		//////////////////////////////
		
	    letGo(folder);
	}while(1);
	closedir(dir);
}

void initFileList(const char *name, int level,FILE *f){
    DIR *dir;
    struct dirent *ent;
    char path[1024]="";
    char t[ 512 ] = "";
    int len;

    struct stat b;

    if (!(dir = opendir(name))) return;
    if (!(ent = readdir(dir))) return;
    int i=0;
    do{
        // if folder
        if (ent->d_type == DT_DIR){
            if(strcmp(ent->d_name,"chunkstempofolder")==0)continue;
            if(strcmp(ent->d_name,"..")==0) len = snprintf(path, sizeof(path)-1, "%s", name);
            else len = snprintf(path, sizeof(path)-1, "%s/%s", name, ent->d_name);
            path[len] = 0;

            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            initFileList(path, level + 1,f);
        }
        //if file
        else{
            //printf("\n===%s-%s",path,name);
            //printf("\n===%s",path);
            len = snprintf(path, sizeof(path)-1, "%s", name);
            //printf("\nscanning: %s",ent->d_name);
            //if(strcmp(ent->d_name,"a.c")!=0 && strcmp(ent->d_name,"a.out")!=0 && strcmp(ent->d_name,"sha1.c")!=0 && strcmp(ent->d_name,"sha1.h")!=0 && strcmp(ent->d_name,"out.txt")!=0 && strcmp(ent->d_name,"initFiles.txt")!=0) {
            char file[strlen(path)+strlen(ent->d_name)+1];
            strcpy(file,path);
            strcat(file,"/");
            strcat(file,ent->d_name);

            stat(file, &b);
            // for jpeg exceptions
            char *sha;
            /*if(strstr(strrchr(ent->d_name, '.') , ".jpg")!=NULL || strstr(strrchr(ent->d_name, '.') , ".pdf")!=NULL || strstr(strrchr(ent->d_name, '.') , ".mkv")!=NULL || strstr(strrchr(ent->d_name, '.') , ".zip") || strstr(strrchr(ent->d_name, '.') , ".flv") || strstr(strrchr(ent->d_name, '.') , ".ppt") || strstr(strrchr(ent->d_name, '.') , ".mp3")){
                FILE *tempF, *jpgF;
                char tempStr[strlen(path)+strlen(ent->d_name)+1];
                char tempStr2[strlen(ent->d_name)+4];

                strcpy(tempStr,path);
                strcat(tempStr,"/");
                strcat(tempStr,ent->d_name);
                //printf("\n%s",tempStr);

                jpgF=fopen(tempStr,"r");
                strcat(tempStr,".txt");
                tempF=fopen(tempStr,"w");
                if(strstr(strrchr(ent->d_name, '.') , ".jpg")!=NULL) convertUTF8ToUnicode(jpgF, tempF,0);
                while(!feof(jpgF)) convertUTF8ToUnicode(jpgF, tempF,1);
                strcpy(tempStr2,ent->d_name);
                strcat(tempStr2,".txt");
                sha=getSha(path,ent->d_name);
                //sha="WFEWFFDTJRERTYJHTREWFEGHTRWFERT2345yge";
                fprintf(f, "%s|", sha);
                
                if(remove(tempStr)==0) printf("\nDeleted:%s",tempStr);
            }else*/{
                sha=getSha(path,ent->d_name);
                fprintf(f, "%s|", sha);
            }

            //printf("%lu|",getSize(file));
            fprintf(f, "%lu|", getSize(file));

            strftime(t, 100, "%m%d%Y-%H:%M:%S", localtime(&b.st_ctime));
            //printf("%s|",t);
            fprintf(f, "%s|",t);

            strftime(t, 100, "%m%d%Y-%H:%M:%S", localtime(&b.st_mtime));
            //printf("%s|",t);
            fprintf(f, "%s|",t);

            strftime(t, 100, "%m%d%Y-%H:%M:%S|", localtime(&b.st_atime));
            //printf("%s",t);
            fprintf(f, "%s",t);

            //printf("%s\n",file);
            strcpy(file,path);
            strcat(file,"/\"");
            strcat(file,ent->d_name);
            strcat(file,"\"");
            fprintf(f, "%s\n",file);

            // file path
            strcpy(file,path);
            strcat(file,"/");
            //strcat(file,"\"");
            strcat(file,ent->d_name);
            //strcat(file,"\"");
            strcpy(t, file);

            strcat(t,"|");

            // file hash
            strcat(t,sha);

            strcat(t,"|");

            // file size
            char buffer[18];
            //printf("\n%s is changed\n",file);

            sprintf(buffer, "%lu", getSize(file));
            strcat(t,buffer);
            strcat(t,"|");

            stat(file, &b);
            strftime(buffer, 100, "%m%d%Y-%H:%M:%S|", localtime(&b.st_ctime));
            //printf("\n%s\n",buffer);
            strcat(t,buffer);

            strftime(buffer, 100, "%m%d%Y-%H:%M:%S|", localtime(&b.st_mtime));
            strcat(t,buffer);

            strftime(buffer, 100, "%m%d%Y-%H:%M:%S", localtime(&b.st_atime));
            strcat(t,buffer);

            fprintf(file2, "%s\n",t);
            //printf("\n====");
            //} 
        }

    } while (ent = readdir(dir));
    
    closedir(dir);
}
void compareFiles(const char *name, int level,FILE *f){
    DIR *dir;
    struct dirent *ent;
	char path[1024]="";
	char t[ 512 ] = "";
	int len;

	struct stat b;


    if (!(dir = opendir(name))) return;
    if (!(ent = readdir(dir))) return;
    int i=0;
    do{
    	// if folder
        if (ent->d_type == DT_DIR){
            if(strcmp(ent->d_name,"chunkstempofolder")==0)continue;
			if(strcmp(ent->d_name,"..")==0) len = snprintf(path, sizeof(path)-1, "%s", name);
			else len = snprintf(path, sizeof(path)-1, "%s/%s", name, ent->d_name);
            path[len] = 0;

            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            compareFiles(path, level + 1,f);
        }
        //if file
        else{
        	len = snprintf(path, sizeof(path)-1, "%s", name);
        	//if(strcmp(ent->d_name,"a.c")!=0 && strcmp(ent->d_name,"a.out")!=0 && strcmp(ent->d_name,"sha1.c")!=0 && strcmp(ent->d_name,"sha1.h")!=0 && strcmp(ent->d_name,"out.txt")!=0 && strcmp(ent->d_name,"initFiles.txt")!=0) {
			strcpy(t,getLine(ent->d_name,f));
            printf("\n:%s/%s",path, ent->d_name);

			if(t!=NULL){
                char *sha;

                if(strstr(strrchr(ent->d_name, '.') , ".jpg")!=NULL || strstr(strrchr(ent->d_name, '.') , ".pdf")!=NULL || strstr(strrchr(ent->d_name, '.') , ".mkv")!=NULL || strstr(strrchr(ent->d_name, '.') , ".zip")!=NULL){
                    FILE *tempF, *jpgF;
                    char tempStr[strlen(path)+strlen(ent->d_name)+1];
                    char tempStr2[strlen(ent->d_name)+4];

                    strcpy(tempStr,path);
                    strcat(tempStr,"/");
                    strcat(tempStr,ent->d_name);
                    //printf("\n%s",tempStr);

                    jpgF=fopen(tempStr,"r");
                    strcat(tempStr,".txt");
                    tempF=fopen(tempStr,"w");

                    if(strstr(strrchr(ent->d_name, '.') , ".jpg")!=NULL) convertUTF8ToUnicode(jpgF, tempF,0);
                    while(!feof(jpgF)) convertUTF8ToUnicode(jpgF, tempF,1);
                    strcpy(tempStr2,ent->d_name);
                    strcat(tempStr2,".txt");
                    //strcat(t,getSha(path,tempStr2));
                    sha=getSha(path,tempStr2);
                    //printf("\n=====%s",getSha(path,tempStr2));
                    //if(remove(tempStr)==0) printf("\nDeleted:%s",tempStr);
                }else 

                sha=getSha(path,ent->d_name);//strcat(t, getSha(path,ent->d_name));

				if(strcmp(t,sha)!=0){
				char file[strlen(path)+strlen(ent->d_name)+1];

				// file path
				strcpy(file,path);
				strcat(file,"/");
                //strcat(file,"\"");
    			strcat(file,ent->d_name);
                //strcat(file,"\"");
    			strcpy(t, file);

    			strcat(t,"|");

    			// file hash
                strcat(t,sha);

				strcat(t,"|");

				// file size
				char buffer[18];
                //printf("\n%s is changed\n",file);

				sprintf(buffer, "%lu", getSize(file));
				strcat(t,buffer);
				strcat(t,"|");

                stat(file, &b);
				strftime(buffer, 100, "%m%d%Y-%H:%M:%S|", localtime(&b.st_ctime));
                //printf("\n%s\n",buffer);
				strcat(t,buffer);

				strftime(buffer, 100, "%m%d%Y-%H:%M:%S|", localtime(&b.st_mtime));
				strcat(t,buffer);

				strftime(buffer, 100, "%m%d%Y-%H:%M:%S", localtime(&b.st_atime));
				strcat(t,buffer);

				fprintf(file2, "%s\n",t);
				}
			}
        	//}	
        }

    } while (ent = readdir(dir));
    
    closedir(dir);
}

char* getLine(char *str,FILE *f) {
	char *temp=malloc(512);
	char fname[strlen(str)+2];
    char *buf;
	strcpy(fname,"\"");
	strcat(fname,str);
	strcat(fname,"\"");

	while(fgets(temp, 512, f) != NULL){
		if(strstr(temp,fname)!=NULL) return strtok(temp, "|");
	}
	return NULL;
}

off_t getSize(const char *filename) {
    struct stat st; 
    if (stat(filename, &st)==0) return st.st_size;
    return 0;
}

void test(int err, const char* msg) {
    if (err) {
        fprintf(stderr, "%s: error %d\n", msg, err);
        exit(1);
    }
}

char* getSha(char* fileDirectory, char* fileName){
    char	buffer[20];
    char	*eureka=malloc(100);
    char	file[strlen(fileDirectory)+strlen(fileName)+1];
    int 	n,
    		i;

    static	unsigned char buf[20] = {0};
    SHA1Context sha;
    FILE 	*f;
    memset(eureka, 0, sizeof eureka);

    strcpy(file,fileDirectory);
    strcat(file,"/");
    strcat(file,fileName);
    int size=getSize(file);
    f=fopen(file,"rb");
	char string[size];

	if(!f) return;

	fseek(f, SEEK_SET, 0);


	strcpy(string,"");
    fread(string,1,size,f);

    if(size==0) strcpy(string,"");
	
    test(SHA1Reset(&sha), "SHA1Reset");
    test(SHA1Input(&sha, string, size), "SHA1Input");
    test(SHA1Result(&sha, buf), "SHA1Result");

    int int_hash = 0;
    for (i=0; i<20; ++i){
        sprintf(buffer, "%02X", buf[i]);
        int_hash += buf[i];
        strcat(eureka,buffer);
    }

	return eureka;

}

void callInit(char *str){
    file1=fopen("initFiles.txt","w+");
    initFileList(str,0,file1);
    fclose(file1);
}

long int convertUTF8ToUnicode(FILE *f, FILE *fOut, int c){

    unsigned char charBuffer[1], helpBuffer[4];
    long int unicodeValue;

    fread(&charBuffer, sizeof(char), 1, f);
    if (charBuffer[0] < 0x80) unicodeValue = charBuffer[0];
    else if (charBuffer[0] < 0xc0) return -1;
    else if (charBuffer[0] < 0xe0){
        fread (&helpBuffer, sizeof(char), 2, f);
        unicodeValue = ( (charBuffer[0]&0x1f ) << 6 ) + (helpBuffer[0]&0x3f );
    }else if (charBuffer[0] < 0xf8) {
        fread (&helpBuffer, sizeof(char), 3, f);
        unicodeValue = ( (charBuffer[0]&0x0f ) << 12 ) + ((helpBuffer[0]&0x3f ) << 6 ) + (helpBuffer[1]&0x3f );
    }else if (charBuffer[0] < 0xfc) {
        fread (&helpBuffer, sizeof(char), 4, f);
        unicodeValue = ((charBuffer[0] & 0x07) << 18) + ((helpBuffer[1]&0x3f ) << 12 ) + ((helpBuffer[1]&0x3f ) << 6 ) +     (helpBuffer[2]&0x3f );
    }

    if(c==1)fprintf(fOut,"%lu",unicodeValue);
}