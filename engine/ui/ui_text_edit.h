#ifndef _UI_TEXT_EDIT_H_
#define	_UI_TEXT_EDIT_H_
#include "ui_base.h"
#include "ui_text_base.h"

#define _TEXT_EDIT_START_MAX_LEN 64
#define _TEXT_EDIT_TMP_START_MAX_LEN 64

enum _KEY_KODE_;

typedef bool (*_cUiTextEdit_OnKeyDown_Callback)(cUiBase*,_KEY_KODE_);

class cUiTextEdit:public cUiTextBase
{
public:
	
	struct _line_item
	{
		_line_item* next;
		_line_item* prev;
		int size;
		int length;
		char* buffer;
		
		_line_item()
		{
			next=0;
			prev=0;
			size=0;
			length=0;
			buffer=0;
		}
		
		~_line_item()
		{
			if(buffer)delete[] buffer;
			if(next)delete next;
			prev=0;
		}

		void add_buffer(int add_size);
		void add_next_line(int add_size);
		void set_str(char* str,int start,int len,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int x,int y);
		int size_of_width(int start,int len,_FONT_CHAR* pCharacters,int FontId);
		void clear_texture(int start,int len,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int x,int y);
		void move_right(int start,int len,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int x,int y);
		void move_left(int start,int len,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int x,int y);
		void cut(char* ret_buff,int start,int len,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int x,int y);
		void print(int start,int len,char* str,_FONTSURF* FromSurf,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int* pxpos,int ypos,uchar r,uchar g,uchar b);
		void print(int start,int len,char* str,_FONTSURF* FromSurf,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int xpos,int ypos,uchar r,uchar g,uchar b);
		void redraw(int start,int len,_FONTSURF* FromSurf,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int xpos,int ypos,uchar r,uchar g,uchar b);
	};
	
	_line_item* Lines;
	_line_item* CurentLine;
	_line_item* DrawFromLine;
	
	int text_ofs;
	int caret_x;
	int caret_y;
	int XposEnd;
	int YposEnd;
	int XStartOffset;
	int YStartOffset;
	int char_space;
	uchar r;
	uchar g;
	uchar b;
	
	bool caret_show;
	bool caret_switch;
	float caret_time;
	int caret_line;
	int caret_char;
	int lines_count;
	int cursor_w;
	int cursor_h;
	int cursor_offsety;
	int SelectOfsA;
	int SelectOfsB;
	int SelectDir;

	int TmpBuffSize;
	char* TmpBuff;
	
	_cUiTextEdit_OnKeyDown_Callback pUiTextEdit_OnKeyDown_Callback;
	
	cUiTextEdit(char* winname,float x,float y,float w,float h,cUiBase* parent=0);
	~cUiTextEdit();

	//
	void OnCursorDown(float x,float y);
	void OnFocusKeep();
	void OnFocusLost();
	void OnKeyDown(_KEY_KODE_ key);
	//

	void SetFont(int fid);
	
	int RedrawSurface(int Xofs,int Yofs,int ofs,int len,uchar r,uchar g,uchar b,bool NoClear=false);
	
	void SetLineStrFrom(int start,char* str,int len,_line_item* line);
	void GetLineStrFrom(int start,char* str,int* plen,_line_item* line);//возвращает остаток строки от смещения, в plen записывает длину остатка.
	void GetLineStrFrom(int start,char* str,int len,_line_item* line);//возвращает строку от смещения длиной len.
	_line_item* SplitTextOnLines(char* str,int len);
	void GetPointsFromOffset(int offet,POINTI2* pret);
	void OnUpdate();

	void RedrawLines(int yofs,_line_item* line);
	void Redraw(int from_line,int count=-1);
		
	void SelectRight();
	void SelectLeft();
	void SelectUp();
	void SelectDown();
	void SelectEnd();
	void SelectRender(int offset,int count);
	
	void CaretUp();
	void CaretDown();
	void CaretRight();
	void CaretLeft();

	void ScrollAlignByCaretV(bool top);
	void ScrollAlignByCaretH(bool left);
	void ScrollOffsetV(int ofs);
	void ScrollOffsetH(int ofs);
	void ScrollSetV(int pos);
	void ScrollSetH(int pos);
	
	void Backspace();
	void Delete();
	
	void RenderLine(char* str,_line_item* line,int line_number);
	void ClearLineTexture(int start,int yofs,_line_item* line);
	void ClearLinesTexture(int start_line_id);
	void ClearLinesTexture(_line_item* line,int y);
	void InputStr(char* str);
	char GetCaretChar();
};

#endif