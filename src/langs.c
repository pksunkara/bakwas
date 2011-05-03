#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "constants.h"
#include "langs.h"

void a2b(char *a, int d) {
	int s=ENG;
	if((int)a[0]==-32 && ((int)a[1]==-92 || (int)a[1]==-91) && (int)a[2]<-64 && (int)a[2]>-129) {
		s=HIN;
	} else if((int)a[0]==-32 && ((int)a[1]==-80 || (int)a[1]==-79) && (int)a[2]<-64 && (int)a[2]>-129) {
		s=TEL;
	} else if((int)a[0]==-32 && ((int)a[1]==-82 || (int)a[1]==-81) && (int)a[2]<-64 && (int)a[2]>-129) {
		s=TAM;
	}
	if(s==HIN && d==TEL) {
		hindi2telugu(a);
	} else if(s==TEL && d==HIN) {
		telugu2hindi(a);
	} else if(s==TEL && d==TAM) {
		telugu2tamil(a);
	} else if(s==TAM && d==TEL) {
		tamil2telugu(a);
	} else if(s==TAM && d==HIN) {
		tamil2hindi(a);
	} else if(s==HIN && d==TAM) {
		hindi2tamil(a);
	}
	return;
}

void hindi2telugu(char *a) {
	if((int)a[1]==-92)
		a[1] = (char)-80;
	else if((int)a[1]==-91)
		a[1] = (char)-79;
	return;
}

void telugu2hindi(char *a) {
	if((int)a[1]==-80)
		a[1] = (char)-92;
	else if((int)a[1]==-79)
		a[1] = (char)-91;
	return;
}

void hindi2tamil(char *a) {
	if((int)a[1]==-92) {
		a[1] = (char)-82;
		int j = (int)a[2];
		if(j==-106 || j==-105 || j==-104)
			a[2] = (char)-107;
		if(j==-101)
			a[2] = (char)-102;
		if(j==-99)
			a[2] = (char)-100;
		if(j==-94 || j==-95 || j==-94)
			a[2] = (char)-97;
		if(j==-89 || j==-90 || j==-91)
			a[2] = (char)-92;
		if(j==-83 || j==-84 || j==-85)
			a[2] = (char)-86;
	} else if((int)a[1]==-91)
		a[1] = (char)-81;
	return;
}

void tamil2hindi(char *a) {
	if((int)a[1]==-82)
		a[1] = (char)-92;
	else if((int)a[1]==-81)
		a[1] = (char)-91;
	return;
}


void tamil2telugu(char *a) {
	if((int)a[1]==-82)
		a[1] = (char)-80;
	else if((int)a[1]==-81)
		a[1] = (char)-79;
	return;
}

void telugu2tamil(char *a) {
	if((int)a[1]==-80) {
		a[1] = (char)-82;
		int j = (int)a[2];
		if(j==-106 || j==-105 || j==-104)
			a[2] = (char)-107;
		if(j==-101)
			a[2] = (char)-102;
		if(j==-99)
			a[2] = (char)-100;
		if(j==-94 || j==-95 || j==-94)
			a[2] = (char)-97;
		if(j==-89 || j==-90 || j==-91)
			a[2] = (char)-92;
		if(j==-83 || j==-84 || j==-85)
			a[2] = (char)-86;
	} else if((int)a[1]==-79) {
		a[1] = (char)-81;
	}
	return;
}
