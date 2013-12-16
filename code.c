#include "Codes/masterServer.c"
#include "Codes/a.c"
#include "Codes/lcd.c"
#include <dirent.h>
#include <string.h>

void main(){
	raf();
	fran();
	
	//Chy("./Outputs/out.Client 5 Files.ret");
	printf("\nSTART DIR");
	DIR *dir;
	struct dirent *ent;
	if (!(dir = opendir("./Outputs"))) return;
    if (!(ent = readdir(dir))) return;
	do{
		if (ent->d_type != DT_DIR){
			if(strcmp(ent->d_name,"..")!=0 && strcmp(ent->d_name,".")!=0){
				if(strcmp(strrchr(ent->d_name, '.'),".ret")==0){
					char fileName[40];
					strcpy(fileName,"./Outputs/");
					strcat(fileName,ent->d_name);
					printf("FILENAME: %s\n",fileName);
					Chy(fileName);
				}
			}
		}
	}while (ent = readdir(dir));
	//*/
	//closedir(dir);
}