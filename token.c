#include <stdio.h>
#include <stdlib.h>

int main(){
	FILE *orig;
	FILE *dest;
	char f[50];
	char token[50];
	char c;

	printf("Digite o endereço do arquivo de origem:");
	scanf("%s", &f);
	orig = fopen(f,"r");
		
	printf("Digite o endereço do arquivo de destino:");
	scanf("%s", &f);
	dest = fopen(f,"w");

	if((orig)&&(dest)) {
		while( (c = getc(orig)) != EOF ){
			//printf("%c\n",c);
			token = c;			
			if(c == 'C'){
				c = getc(orig);
				if(c == EOF){
					fprinf(dest, token);
				}
				else{
					if(c == 'A'){
						c = getc(orig);
						if((c == '\n')||(c == ' ')||(c == '\t')){
								
						}
					}
					else{
						//id
						token = token + c;
						c = getc(orig);
						while((c != EOF)&&(c != ' ')&&(c != '\n')&&(c != '\t')){
							token = token + c;
							c = getc(orig);
						}
						fprintf(dest,token);
						if(c == EOF){
							return 0;
						}
					}
				}
			}
		}
	}
	fclose(file);
}
