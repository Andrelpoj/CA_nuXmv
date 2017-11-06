#include <stdio.h>
#include <stdlib.h>

int main(){
	FILE *orig;
	FILE *dest;
	char f[50];
	char token[50];
	char c;

	int count = 0;

	printf("Digite o endereço do arquivo de origem:");
	scanf("%s", &f);
	orig = fopen(f,"r");
		
	printf("Digite o endereço do arquivo de destino:");
	scanf("%s", &f);
	dest = fopen(f,"w");

	if((orig)&&(dest)) {
		while( (c = getc(orig)) != EOF ){
			//pular espaços			
			while((c == '\n') || (c == ' ') || (c == '\t')){				
				c = getc(orig);
				if (c == EOF){
					return 0; //erro
				}
			}
			
			if((c==1)||(c==2)||(c==3)||(c==4)||(c==5)||(c==6)||(c==7)||(c==8)||(c==9)){
				token[count] = c;
				count++;
				
				c = getc(orig);
				while((c!='\n')&&(c!=' ')&&(c!='\t')&&(c!=EOF)&&( (c==1)||(c==2)||(c==3)||(c==4)||(c==5)||(c==6)||(c==7)||(c==8)||(c==9) ){
					token[count] = c;
					count++;
					c = getc(orig);
				}
				if (c == EOF)||(c=='\n')||(c==' ')||(c=='\t')){	
					token[count] = '\n';
					token[count+1] = '\0';				
					fprintf(dest,token);
					count = 0;
				}
				else{
					if ((c!=1)&&(c!=2)&&(c!=3)&&(c!=4)&&(c!=5)&&(c!=6)&&(c!=7)&&(c!=8)&&(c!=9)){
						return 0;
					}
				}
				
			}
			else{

				token[count] = c;
				count++;
						
				if(c == 'C'){
					c = getc(orig);
					if(c == EOF){	
						token[count] = '\0';				
						fprintf(dest,token);
						count = 0;
					}
					else{
						if(c == 'A'){
							token[count] = c;
							count++;

							c = getc(orig);
							if((c == '\n')||(c == ' ')||(c == '\t')){
								token[count] = '\n';
								token[count+1] = '\0';							
								fprintf(dest,token);
								count = 0;
							}
						}
						else{
							//id
							token[count] = c;
							count++;

							c = getc(orig);
							while((c != EOF)&&(c != ' ')&&(c != '\n')&&(c != '\t')){
								token[count] = c;
								count++;

								c = getc(orig);
							}
							token[count] = '\n';
							token[count+1] = '\0';
							fprintf(dest,token);
							count = 0;

							if(c == EOF){
								return 0;
							}
						}
					}
				}
			}
		}
		fclose(orig);
		fclose(dest);
	}	
}
