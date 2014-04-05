#include "Codes/masterServer.c"
#include "Codes/a.c"
#include "Codes/lcd.c"
#include <dirent.h>
#include <string.h>
#include <sys/time.h>

void main(){
	
	struct timeval start, end;
    long mtime, seconds, useconds;    
    gettimeofday(&start, NULL);

	if(raf()){
		fran();
		Chy("./Outputs/out.Files.ret");
		
		gettimeofday(&end, NULL);

		seconds  = end.tv_sec  - start.tv_sec;
		useconds = end.tv_usec - start.tv_usec;

		mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

		long tmt;
		printf("\n_______________________________________________________________\n\n");
		if(mtime > 1000){
			tmt = mtime % 1000;
			mtime /= 1000;
			if(mtime > 60){
				tmt = mtime%60;
				mtime /= 60;
				printf("Elapsed time: %ld.%ld minute/s\n", mtime,tmt);
			} else printf("Elapsed time: %ld.%ld second/s\n", mtime,tmt);
		} else printf("Elapsed time: %ld millisecond/s\n", mtime);
		printf("\n_______________________________________________________________\n");
	}else{
		printf("\nExit");
	}
}