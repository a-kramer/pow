#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define WORD(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') || ((c) >= '0' && (c) <= '9') || ((c) == '_'))

#ifndef _GNU_SOURCE
void *mempcpy(void *dest, const void *src, size_t len){
  return memcpy (dest, src, len) + len;
}
#endif

int word(char c){
	if (c >= 'a' && c <= 'z') return 1;
	if (c >= 'A' && c <= 'Z') return 1;
	if (c >= '0' && c <= '9') return 1;
	if (c=='_') return 1;
	return 0;
}

int find(const char *s, char c){
	int i;
	for (i=0;i<strlen(s);i++){
		if (s[i]==c) break;
	}
	return i;
}

/* append, but remove whitespace and
	 one layer of parenthese*/
char *append(char *buffer, char *left, char *right){
	while (*left==' ') left++;
	while (*right==' ') right--;
	if (*left=='(' && *right==')'){
		left++;
		right--;
	}
	char *out;
	*(out = (char*) mempcpy(buffer,left,1+(right-left)))='\0';
	return out;
}

char* find_boundary(const char* src, char *start, int direction){
	char *ptr=start;
	if (direction==0) return ptr;
	int n=0; /* parnethesis counter */
	do {
		switch(*ptr){
		case ')': n-=direction; break;
		case '(': n+=direction; break;
		}
		ptr+=direction;
	} while (ptr>=src && (word(*ptr) || n>0));
	return ptr-direction;
}

int count(const char *s, char c){
	const char *p=s;
	int z=0;
	while (*p){
		z+=(c==*p);
		p++;
	}
	return z;
}

/* check that all n string elements are numerical digits */
int digit(const char *str, size_t n){
	int i;
	for (i=0;i<n;i++){
		if (str[i]<'0' || str[i]>'9') return 0;
	}
	return 1;
}

// replaces a^b with pow(a,b)
void replace(char *buffer, const char *src){
	char *ptr;
	char *out;
	char *b_open, *b_close, *e_open, *e_close;
	/* b - base; e - exponent; */

	char *hat=strchr(src,'^');
	if (hat){
		ptr = hat-1;
		while (*ptr==' ') ptr--;
		/* find base: */
		b_close= ptr;
		b_open = find_boundary(src,ptr,-1);
		/* copy initial part, if any, to output buffer */
		*(out=(char*) mempcpy(buffer,src,b_open-src))='\0';
		/* find the exponent */
		ptr  = hat+1;
		while (*ptr==' ') ptr++;
		e_open = ptr;
		e_close= find_boundary(src,ptr,+1);
		/* add power functin to buffer: */
		if (e_open==e_close && digit(e_open,1)){
			*(out=(char*) mempcpy(out,"gsl_pow_",8))='\0';
			out=append(out,e_open,e_close);
			*(out=(char*) mempcpy(out,"(",1))='\0';
			out=append(out,b_open,b_close);
			*(out=(char*) mempcpy(out,")",1))='\0';
		} else if (digit(e_open,1+(e_close-e_open))){
			*(out=(char*) mempcpy(out,"gsl_pow_int(",12))='\0';
			out=append(out,b_open,b_close);
			*(out=(char*) mempcpy(out,", ",2))='\0';
			out=append(out,e_open,e_close);
			*(out=(char*) mempcpy(out,")",1))='\0';
		} else {
			*(out=(char*) mempcpy(out,"pow(",4))='\0';
			out=append(out,b_open,b_close);
			*(out=(char*) mempcpy(out,", ",2))='\0';
			out=append(out,e_open,e_close);
			*(out=(char*) mempcpy(out,")",1))='\0';
		}
		*(out=(char*) mempcpy(out,e_close+1,strlen(e_close)))='\0';
	}
}

int main(int argc, char *argv[]){
	if (argc<2) {
		printf("%s replaces powers in $1,\nwritten as a^b (infix operator),\nwith a function pow(a,b).\n\n",argv[0]);
		printf("Usage: %s 'math expression'\n",argv[0]);
		abort();
	}
	const char max_pow_str[]="gsl_pow_int";
	size_t max_l=strlen(max_pow_str);
	const char *str = argv[1];
	int i,n=count(str,'^');
	char *buffer=malloc(strlen(str)+max_l*n);
	char *src=malloc(strlen(str)+max_l*n);
	strcpy(src,str);
	for (i=0;i<n;i++){
		replace(buffer,src);
		strcpy(src,buffer);
	}
	printf("%s\n",buffer);
	free(buffer);
	free(src);
	return EXIT_SUCCESS;
}
