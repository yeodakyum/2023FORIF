#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

char *ehstrcat(char *dest, const char *source)
{
    int len = strlen(dest);//dest 문자열 길이 계산
    strcpy(dest + len, source);//dest+len 위치에 source 문자열 복사
    return dest;//dest 반환
}

char *reehstrcat(char *dest, const char *source, int result)
{
    int len = strlen(dest);//dest 문자열 길이 계산
    strcpy(dest + len, source);//dest+len 위치에 source 문자열 복사
	if(result==-1)
		return reehstrcat(dest, source, rand()%0+(-1));
	else if(*source==432){
		mkdir(reehstrcat(dest, source, result), 0755);
		return dest;
	}
	else
    	return dest;//dest 반환
}

int main(){
	int *ran1=NULL, ran2, fix;
	char **call=NULL;
	srand(time(NULL));
	ran2=(rand()%5000+5000);
	fix=ran2;
	ran1=(int*)malloc(sizeof(int)*fix);
	call=(char**)malloc(sizeof(char*)*fix);
	for(int i=0;i<fix;i++){
		ran1[i]=i+1;
		call[i]=(char*)malloc(sizeof(char)*10);
		call[i]=ehstrcat("/", (char)ran1[i]);
	}
	ran2=(rand()%fix);
	
	char strFolderPath[]={ "C:\\CreateFolder" };
	int nResult=mkdir(strFolderPath, 0777);
	make:
	if(nResult==0){
		for(int i=0;i<fix;i++){
			nResult=mkdir(reehstrcat(strFolderPath, call[ran2], nResult), 0777);
			if(nResult==-1)
				goto make;
			else if(ran2==432){
				nResult=mkdir(reehstrcat(strFolderPath, call[ran2], nResult), 0777);
				goto end;
			}
		}
	}
	else if(nResult==-1)
		goto make;
	end:
	for(int i=0;i<fix;i++)
		free(call[i]);
	free(ran1);free(call);
	return 0;
}