
#include "file.h"
#include "string.h"

#ifdef _DEBUG
#include "engine/debug/sizeof_memory.hpp"
#endif

using namespace data_files;

files* files::pFilesObject = 0;
/*
bool32 data_files::_file_version::cmp(_file_version *in)
{
	if(in->ver==ver && !strcmp(format,in->format))return true;
	return false;
}*/

void data_files::_FILE_KEY_LIST::reset()
{
	Time=0;
	position=vec3(0,0,0);
	scale=vec3(1.0,1.0,1.0);
	rotation=quat(0,0,0,1);
	AnimMod=0;
}

data_files::_FILE_KEY_LIST::_FILE_KEY_LIST()
{
	reset();
}
data_files::_FILE_KEY_LIST::~_FILE_KEY_LIST()
{
}

_FILE* data_files::files::Open(char * fname)
{
	FILE* pf=fopen(fname,"rb");

	if(!pf){
		return 0;
	}

	fseek(pf,0,SEEK_END);
	int fsize=ftell(pf);
	fseek(pf,0,0);
	unsigned char *fdata=new unsigned char[fsize+1];
	fread(fdata,1,fsize, pf);
	fclose(pf);
	fdata[fsize]=0;

	return new _FILE(fname, fdata, fsize);
}

_FILE* data_files::files::Open(const char* fname)
{
	FILE* pf = fopen(fname, "rb");

	if(!pf){
		return 0;
	}

	fseek(pf, 0, SEEK_END);
	uint fsize = ftell(pf);
	fseek(pf, 0, 0);
	unsigned char* fdata = new unsigned char[fsize + 1];
	fread(fdata, 1, fsize, pf);
	fclose(pf);
	fdata[fsize] = 0;

	return new _FILE(fname, fdata, fsize);
}

_FILE* data_files::files::GetFile(char *fname)
{
	for(int i = 0; i < files_list.count; i++)
	{
		if(!strcmp(fname, files_list[i]->name)){
			return files_list[i];
		}
	}

	_FILE* pf = Open(fname);

	if(pf){
		files_list.push(pf);

		return pf;
	}

	return 0;
}

_FILE* data_files::files::GetFile(const char* fname)
{
	for(int i = 0; i < files_list.count; i++)
	{
		if(files_list[i] && !strcmp(fname, files_list[i]->name)){
			return files_list[i];
		}
	}

	_FILE* pf = Open(fname);

	files_list.push(pf);

	return pf;
}

int data_files::_cpy_fdata(void *in,char *fdata,uint *fcur,uint size)
{
	memcpy(in,&fdata[*fcur],size);
	(*fcur)+=size;
	return (*fcur);
}

int str_get_word(char* str, int ofs, int size, char* buff)
{
	char* n = strstr(&str[ofs], "\n");
	char* s = strstr(&str[ofs], " ");
	int len = 0;

	if(n && s){
		int lenN = uint(n) - uint(str) - ofs;
		int lenS = uint(s) - uint(str) - ofs;
		len = lenN < lenS ? lenN : lenS;
	}
	else if(n && !s){
		len = uint(n) - uint(str) - ofs;
	}
	else if(!n && s){
		len = uint(s) - uint(str) - ofs;
	}

	memcpy(buff, &str[ofs], len);
	buff[len] = 0;
	++len;

	return len;
}

int data_files::FileRead(void *in, uint size,_FILE *pf)
{
	//printf("FileRead %d %d %s\n", pf->cur, pf->size, pf->name);
	uint rcount=size;
	if(rcount>pf->size)rcount-=(rcount-pf->size);
	
	memcpy(in,&pf->data[pf->cur],rcount);
	pf->cur+=size;
	return rcount;
}

int data_files::FileReadLine(char *in,_FILE *pf)
{
	int len=uint(strstr((char*)&pf->data[pf->cur],"\n"))-uint(pf->data)-pf->cur;

	if(len<=0)
	{
		len=pf->size-pf->cur;
		memcpy(in,&pf->data[pf->cur],len);

		in[len]=0;

	}else{
		memcpy(in,&pf->data[pf->cur],len-1);
		in[len-1]=0;
		++len;
	}

	pf->cur+=len;

	return len;
}

int data_files::FileReadWord(char *buff,_FILE *pf)
{
	int len=str_get_word((char*)pf->data,pf->cur,pf->size,buff);
	pf->cur+=len;

	if(!len){
#ifdef DEBUG_FILE_LOG
	printf("FileReadWord(%x) ERROR\n",buff);
#endif
		return 0;
	}
#ifdef DEBUG_FILE_LOG
	printf("FileReadWord(%s)\n",buff);
#endif

	return len;
}

FILE* data_files::OpenFileForWrite(char* fname)
{
	return fopen(fname,"wb");
}

int data_files::FileClose(_FILE* pf, bool free)
{
	pf->cur = 0;
	if(free){

		int index = files::pFilesObject->files_list.enumerate(
			[pf](data_files::_FILE* ipf){
				if(ipf == pf){
					return true;
				}
				return false;
			}
		);

		if(index != -1){
			delete files::pFilesObject->files_list.pop_index(index);
			//remove(files::pFilesObject->files_list.begin(), files::pFilesObject->files_list.end(), pf);
			//delete pf;
		}
	}
	return 0;
}

int data_files::CreateFolder(char* path)
{
#ifdef LINUX_SPACE
	char full_path[MAX_PATH_LEN];
	sprintf(full_path,"%s/%s",pGFile->CurDir,path);
	mkdir(full_path,0777);
#endif
	return 0;
}


void data_files::files::DBG_PrintFiles()
{
	printf("--- DBG_PrintFiles ---\n");
	for(int i = 0; i < files_list.count; ++i)
	{
		if(files_list[i]){
			printf("\t%s (%d) (%d)\n",files_list[i]->name, files_list[i]->size, files_list[i]->cur);
		}
		else{
			printf("\tERROR\n");
		}
	}
}