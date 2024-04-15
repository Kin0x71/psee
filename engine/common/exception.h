#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <engine/defines.h>

#include <exception>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef WINDOWS_SPACE
#include <windows.h>
#elif  defined(LINUX_DESCTOP_SPACE)
#include <SDL2/SDL.h>
#endif

class exception_CallDefaultMethod : public std::exception {
public:
	//throw exception_CallDefaultMethod(__FUNCTION__, __FILE__, __LINE__)
	exception_CallDefaultMethod(const char* function_name, const char* file_name, int line_number)
	{
		int file_name_len = strlen(file_name);
		int divergence_pos = 0;
		for(; __FILE__[divergence_pos]; ++divergence_pos){
			if(__FILE__[divergence_pos] != file_name[divergence_pos] || divergence_pos >= file_name_len)break;
		}

		const char* c_file_name = &file_name[divergence_pos];

		file_name_len = strlen(c_file_name);
		int function_name_len = strlen(function_name);

		size_t length = 0;

		const char* format_string = "������ ��������� �����.\n� �����:%s\n������: %d\n�����: %s";

		char* buff = new char[strlen(file_name) + function_name_len + strlen(format_string) + 24];
		sprintf(buff, format_string, file_name, line_number, function_name);

#ifdef WINDOWS_SPACE
		MessageBoxA(0, buff, "��, ��� ������.", MB_ICONERROR);
#elif defined(LINUX_DESCTOP_SPACE)
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,L"��, ��� ������.",buff,NULL);
#endif
		exit(0);
	}
};

class exception_ValueNotFound : public std::exception {
public:
	//throw exception_ValueNotFound(__FUNCTION__, __FILE__, __LINE__)
	exception_ValueNotFound(const char* function_name, const char* file_name, int line_number)
	{
		int file_name_len = strlen(file_name);
		int divergence_pos = 0;
		for(; __FILE__[divergence_pos]; ++divergence_pos){
			if(__FILE__[divergence_pos] != file_name[divergence_pos] || divergence_pos >= file_name_len)break;
		}

		const char* c_file_name = &file_name[divergence_pos];

		file_name_len = strlen(c_file_name);
		int function_name_len = strlen(function_name);

		const char* format_string = "�������� �� �������.\n������ � �����:\n%s\n������: %d\n�����/�������: %s";

		char* buff = new char[file_name_len + function_name_len + strlen(format_string) + 24];

		sprintf(buff, format_string, file_name, line_number, function_name);

#ifdef WINDOWS_SPACE
		MessageBoxA(0, buff, "�����! ���-�� ����� �� ���.", MB_ICONERROR);
#elif defined(LINUX_DESCTOP_SPACE)
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"�����! ���-�� ����� �� ���.",buff,NULL);
#endif
		exit(0);
	}
};

class exception_box : public std::exception {
public:
	exception_box(const char* function_name, const char* file_name, int line_number, const char* format, ...)
	{
		va_list marker;
		va_start(marker, format);

		const char* fs = "function:\n   %s\nfile:\n   %s\nline:\n   %d\n\n%s";

		int file_name_len = strlen(file_name);
		int divergence_pos = 0;
		for(; __FILE__[divergence_pos]; ++divergence_pos){
			if(__FILE__[divergence_pos] != file_name[divergence_pos] || divergence_pos >= file_name_len)break;
		}

		const char* c_file_name = &file_name[divergence_pos];

		file_name_len = strlen(c_file_name);

		char* format_buffer = new char[strlen(format) + strlen(function_name) + file_name_len + strlen(fs) + 24];

		sprintf(format_buffer, fs, function_name, &file_name[divergence_pos], line_number, format);

		char* buff = new char[strlen(format_buffer) + 512];

		vsprintf(buff, format_buffer, marker);
		va_end(marker);

#ifdef WINDOWS_SPACE
		MessageBoxA(0, buff, "������!", MB_ICONERROR);
#elif defined(LINUX_DESCTOP_SPACE)
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"������!",buff,NULL);
#endif

		delete[] format_buffer;
		delete[] buff;

		exit(0);
	}
};

void warning_format_log(const char* function_name, const char* file_name, int line_number, const char* format, ...);

#endif