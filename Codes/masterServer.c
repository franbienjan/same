//**********************************************************************
//* TASKS TO COMPLETE:
//* [X] Fix Bloom Filter Data Structure
//* [X] Compare and filter out duplicate files
//* [X] Ignore and filter out small files (<8000)
//* [X] Read paper on SIFL and File Locality
//*     >> [X] Compare file to folder (in extreme binning-ish technique)
//*     >> [X] Stream Informed File Layout technique
//*		>> [X] Cache files
//**********************************************************************

//**********************************************************************
//* NOTES:
//*	[X] Hash lang ang ilagay Bloom Filter, pero store yung whole
//* [X] Produce a return text file for LCD Stage of the game 
//*
//**********************************************************************

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
//#include "bloom.h"
#include "MurmurHash2.c"
#include "bloom.c"

#include <dirent.h>

//Adding folders and files in File Cache
//Using path, get all strings in hashcatalog that start with the same path
void folderCache (char *path, char *hash, FILE *f2, FILE *f3) {

	char linus[300]="", *splithash[30], *splithash2[30];
	int i = 0;

	for(i=0;i<30;i++){
		splithash[i]=NULL;
		splithash2[i]=NULL;
	}
	i=0;
	rewind(f2);
	fclose(f3);
	f3 = fopen("cache.txt", "w+");
	
	splithash[i] = strtok(path, "/");
	while (splithash[i] != NULL)
		splithash[++i] = strtok(NULL, "/");

	splithash[i-1] = '\0';
	fprintf(f3, "%s\n", hash);
	
	while (fgets (linus, 300, f2) != NULL) {
		
		if (strncmp (*splithash, linus, strlen(*splithash)) == 0) {
			
			i = 0;
			splithash2[i] = strtok(linus, "|");
			while (splithash2[i] != NULL)
				splithash2[++i] = strtok(NULL, "|");
	
			fprintf(f3, "%s\n", splithash2[1]);
		}
	}
	
	rewind(f3);
	
}

void masterServer(FILE *f1, FILE *f2, FILE *f3, char *buffer) {

	char	line[300]="",
			fullline[300]="",
			cacheline[300]="";
	char *splithash[10];
	int i;
	long num, entries = 0, hashcatalogentries = 0;
	int nobloom = 0;
	struct bloom bloom;
	FILE *f4;	
	f4=fopen(buffer,"w+");

	for(i=0;i<10;i++) splithash[i]=NULL;

	//check length of new input
	while (fgets (line, 300, f1) != NULL) {
		entries++;
	}
	//printf("\nEntries:%lu",entries);
	
	//check length of hashcatalog
	while (fgets (line, 300, f2) != NULL) {
		hashcatalogentries++;
	}
	//printf("\nHCE:%lu",hashcatalogentries);
	
	rewind(f1);
	rewind(f2);
	bloom_init(&bloom, hashcatalogentries + entries, 0.01);	
	
	//add hashcatalog entries to bloom filter
	while (fgets (line, 300, f2) != NULL) {
		i = 0;
		splithash[i] = strtok(line, "|");
		while (splithash[i] != NULL)
			splithash[++i] = strtok(NULL, "|");
		
	
		//****************** EXPERIMENT *********************
		//**        	File Age Restriction
		//**
		//**	The Master Server will not add files that 
		//**		 are more than X days old.
		//**
		//**		 Lets hope may difference ahahahaha.
		//**
		//**   Please remove if you want to have normal SAM
		//**
		//** In this case, X = 500; PLEASE ADJUST
		
		if (atoi(splithash[3]) > 500) {
			printf("HAHAHAHA TIGULANG NA KA! %s\n", splithash[0]);
			continue;
		}
		
		//**
		//**
		//***************************************************
		
		bloom_add(&bloom, splithash[1], strlen(splithash[1]));
	}
	
	//read list of updated files
	while (fgets (line, 300, f1) != NULL) {
		
		i = 0;
		strcpy(fullline,line);
		splithash[i] = strtok(line, "|");
		while (splithash[i] != NULL)
			splithash[++i] = strtok(NULL, "|");
		
		//Size Filter
		num = atoi(splithash[2]);
		if (num >= 8000) {
		
			//Cache Check; NOTE: Hash lang ang nakastore sa Cache
			while (fgets (cacheline, 300, f3) != NULL) {
				cacheline[strlen(cacheline) - 1] = '\0';
				
				if (strcmp(splithash[1], cacheline) == 0) {
					nobloom = 1;				
				}
					
			}
			
			//Nobloom = 1 if meron sa cache, no need to continue to bloom
			if (nobloom) {
				nobloom = 0;
				continue;
			}
				
			//Bloom Filter
			if (bloom_add(&bloom, splithash[1], strlen(splithash[1])) == 0) {
				folderCache (splithash[0], splithash[1], f2, f3);
				fprintf(f2, "%s", fullline);
				fprintf(f4, "%s", fullline);
			}
		} else {
			//Include agad sa return (BUT NOT HASHCATALOG) without checking
			fprintf(f4, "%s",fullline);
		}

	}
	fclose(f4);
	
}

//Everything begins here, dearies!
void fran() {

	FILE *f1, *f2, *f3;
	DIR *dir;
    struct dirent *ent;
	if (!(dir = opendir("./Outputs"))) return;
    if (!(ent = readdir(dir))) return;
 
    do{
		if(strcmp(ent->d_name,"..")!=0 && strcmp(ent->d_name,".")!=0){
			char buffer[strlen(ent->d_name)+10];
    		strcpy(buffer,"./Outputs/");
    		strcat(buffer,ent->d_name);
    		printf("\nReading:%s",buffer);
			
			//Ignore .ret files
			if(strstr(buffer,".ret")!=NULL) 
				continue;

			f1 = fopen (buffer, "r");
    		if(!f1) 
				printf("\nno file:%s",buffer);

    		f2 = fopen ("hashcatalog.txt", "r+");
    		//f2 = fopen ("../Raf/uniquefiles.txt", "r+");
			f3 = fopen ("cache.txt", "r+");

			strcat(buffer,".ret");

			masterServer(f1, f2, f3, buffer);

			fclose(f2);
			fclose(f3);
		}
	} while (ent = readdir(dir));
}