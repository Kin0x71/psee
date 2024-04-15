#include "ui_text_edit.h"
#include "../timer.h"
#include "../input.h"

extern cTimer Timer;
extern cTextBase TextBase;
extern cInput Input;

cUiTextEdit::cUiTextEdit(char* winname,float x,float y,float w,float h,cUiBase* parent):cUiTextBase(winname,x,y,w,h,parent)
{
	UiWindowClass=UI_TEXT_EDIT;
	//this->base_constructor(winname,x,y,w,h,parent);

	focus_keeper_flag=true;
	
	FontId=0;
		
	TmpBuffSize=_TEXT_EDIT_TMP_START_MAX_LEN;
	TmpBuff=new char[_TEXT_EDIT_TMP_START_MAX_LEN];
	text_len=0;
	text_ofs=0;
		
	Lines=CurentLine=DrawFromLine=new _line_item;
		
	caret_x=0;
	caret_y=0;
	XStartOffset=0;
	YStartOffset=0;
	caret_show=false;
	caret_switch=false;
	caret_time=0.0f;
	caret_line=0;
	caret_char=0;
	lines_count=0;
	SelectOfsA=0;
	SelectOfsB=0;
	SelectDir=0;
		
	r=0;
	g=255;
	b=0;
		
	char_space=1;
	
	pUiTextEdit_OnKeyDown_Callback=0;
		
	SetFont(0);
		
	Create(int(w),int(h));
}

cUiTextEdit::~cUiTextEdit()
{
	if(buff){
		delete[] buff;
		buff=0;
	}
}

void cUiTextEdit::_line_item::add_buffer(int add_size)
{
	if(!buffer){
		size=add_size;
		buffer=new char[size+1];
		buffer[0]=0;
	}else{
		char* new_buff=new char[size+add_size+1];
		memcpy(new_buff,buffer,size);
		delete[] buffer;
		size+=add_size;
		buffer=new_buff;
	}
}

void cUiTextEdit::_line_item::add_next_line(int add_size)
{
	_line_item* ln=next;
	
	next=new _line_item();
	next->prev=this;
	next->size=add_size;
	next->buffer=new char[add_size+1];
	next->buffer[0]=0;

	if(ln){
		next->next=ln;
		ln->prev=next;
	}
}

void cUiTextEdit::_line_item::set_str(char* str,int start,int len,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int x,int y)
{
	if(length){
		move_right(start,len,ToSurf,pCharacters,FontId,x,y);
		length+=len;
	}else{
		if(!size){
			add_buffer(start+len);
		}
		length=len;
	}

	memcpy(&buffer[start],str,len);

	buffer[length]=0;
}

int cUiTextEdit::_line_item::size_of_width(int start,int len,_FONT_CHAR* pCharacters,int FontId)
{
	if(start+len>size)return 0;

	int w=0;
	for(int i=start;i<len;++i)
	{
		int cid=uchar(buffer[i])-32;
		if(cid<0)cid=0;
		w+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
	}

	return w;
}

void cUiTextEdit::_line_item::clear_texture(int start,int len,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int x,int y)
{
	int ofs_x=x;
	for(int i=0;i<start;++i)
	{
		int cid=uchar(buffer[i])-32;
		if(cid<0)cid=0;
		ofs_x+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
	}

	int w=0;
	for(int i=length-1;i>start-1;--i)
	{
		int cid=uchar(buffer[i])-32;
		if(cid<0)cid=0;
		w+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;

		if(w>int(ToSurf->w))break;
	}

	ClearRect(ToSurf,ofs_x,y,w,int(TextBase.ppFonts[FontId]->h));
}

void cUiTextEdit::_line_item::move_right(int start,int len,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int x,int y)
{
	if(start+len+length>size){
		add_buffer(len);
	}

	int ofs_x=x;
	for(int i=0;i<start;++i)
	{
		int cid=uchar(buffer[i])-32;
		if(cid<0)cid=0;
		ofs_x+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
	}

	int w=0;
	for(int i=length-1;i>start-1;--i)
	{
		int cid=uchar(buffer[i])-32;
		if(cid<0)cid=0;
		w+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
		
		buffer[i+len]=buffer[i];
	}

	ClearRect(ToSurf,ofs_x,y,w,int(TextBase.ppFonts[FontId]->h));
}

void cUiTextEdit::_line_item::move_left(int start,int len,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int x,int y)
{
	int ofs_x=x;
	for(int i=0;i<start;++i)
	{
		int cid=uchar(buffer[i])-32;
		if(cid<0)cid=0;
		ofs_x+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
	}

	int w=0;
	for(int i=start;i<length;++i)
	{
		int cid=uchar(buffer[i])-32;
		if(cid<0)cid=0;
		w+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
	}

	//memcpy(&buffer[start],&buffer[start+len],length-start);
	for(int i=0;i<length-start;++i)buffer[start+i]=buffer[start+len+i];
	//memset(&buffer[length-len],0,len);
	for(int i=0;i<len;++i)buffer[length-len+i]=0;
	
	length-=len;

	ClearRect(ToSurf,ofs_x,y,w,int(TextBase.ppFonts[FontId]->h));
}

void  cUiTextEdit::_line_item::cut(char* ret_buff,int start,int len,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int x,int y)
{
	if(start+len>size){
		return;
	}

	memcpy(ret_buff,&buffer[start],len);

	move_left(start,len,ToSurf,pCharacters,FontId,x,y);
}

void cUiTextEdit::_line_item::print(int start,int len,char* str,_FONTSURF* FromSurf,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int* pxpos,int ypos,uchar r,uchar g,uchar b)
{
	int ofs=start;
	for(int i=0;i<len;++i)
	{
		buffer[ofs++]=str[i];
			
		int cid=uchar(str[i])-32;
		if(cid<0)cid=0;
			
		TextBase.FontToTextureStretchRectColorMask(
			FromSurf,
			pCharacters[cid].x,pCharacters[cid].y,
			pCharacters[cid].w,pCharacters[cid].h,
			ToSurf,
			*pxpos+pCharacters[cid].ofsx,ypos+pCharacters[cid].ofsy,
			r,g,b
		);
			
		*pxpos+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
	}
}

void cUiTextEdit::_line_item::print(int start,int len,char* str,_FONTSURF* FromSurf,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int xpos,int ypos,uchar r,uchar g,uchar b)
{
	int txpos=xpos;
	print(start,len,str,FromSurf,ToSurf,pCharacters,FontId,&txpos,ypos,r,g,b);
}

void cUiTextEdit::_line_item::redraw(int start,int len,_FONTSURF* FromSurf,_IMGSURF* ToSurf,_FONT_CHAR* pCharacters,int FontId,int xpos,int ypos,uchar r,uchar g,uchar b)
{
	int xofs=xpos;
	for(int i=start;i<len;++i)
	{
		int cid=uchar(buffer[i])-32;
		if(cid<0)cid=0;
		
		int px=xofs+pCharacters[cid].ofsx;
		int py=ypos+pCharacters[cid].ofsy;

		if(int(px+pCharacters[cid].w)>0 && int(px+pCharacters[cid].w)<ToSurf->w && int(py+pCharacters[cid].h)>0 && int(py+pCharacters[cid].h)<ToSurf->h)
		{
			TextBase.FontToTextureStretchRectColorMask(
				FromSurf,
				pCharacters[cid].x,pCharacters[cid].y,
				pCharacters[cid].w,pCharacters[cid].h,
				ToSurf,
				px,py,
				r,g,b
			);
		}
		xofs+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
	}
}

void cUiTextEdit::OnCursorDown(float x,float y)
{
}

void cUiTextEdit::OnFocusKeep()
{
	caret_show=true;
}

void cUiTextEdit::OnFocusLost()
{
	caret_show=false;
}

void cUiTextEdit::OnKeyDown(_KEY_KODE_ key)
{
	if(pUiTextEdit_OnKeyDown_Callback){
		if(!pUiTextEdit_OnKeyDown_Callback(this,key))return;
	}
	
	if(Input.KeyCharTab[key].lower)
	{
		if((Input.IsKeyDown(_KEY_SHIFTL) || Input.IsKeyDown(_KEY_SHIFTR)) && Input.KeyCharTab[key].upper){
			char str[2];
			str[0]=Input.KeyCharTab[key].upper;
			str[1]=0;
			InputStr(str);
			return;
		}else{
			if(Input.IsKeyDown(_KEY_CTRLL) /*&& Input.IsKeyDown(_KEY_C,true)*/)
			{
				SelectEnd();
			}else{
				char str[2];
				str[0]=Input.KeyCharTab[key].lower;
				str[1]=0;
				InputStr(str);
				return;
			}
		}
	}else{
		if(key==_KEY_RIGHT)
		{
			if(Input.IsKeyDown(_KEY_SHIFTL) || Input.IsKeyDown(_KEY_SHIFTR))
			{
				SelectRight();
			}else{
				CaretRight();
			}
			return;
		}
		
		if(key==_KEY_LEFT)
		{
			if(Input.IsKeyDown(_KEY_SHIFTL) || Input.IsKeyDown(_KEY_SHIFTR))
			{
				SelectLeft();
			}else{
				CaretLeft();
			}
			return;
		}

		if(key==_KEY_UP)
		{
			CaretUp();
			return;
		}
		
		if(key==_KEY_DOWN)
		{
			CaretDown();
			return;
		}
		
		if(key==_KEY_BACK)
		{
			Backspace();
			return;
		}
		
		if(key==_KEY_DELETE)
		{
			Delete();
			return;
		}
		
		if(key==_KEY_HOME)
		{
			//InputStr("home");
			return;
		}
		
		if(key==_KEY_END)
		{
			//InputStr("{1abc}\n{2abc}\n{3abc}\n{4abc}\n{5abc}");
//			printf("%d %d %f [%d]",caret_y,pTexture->surf.h,TextBase.ppFonts[FontId]->h,(caret_y-pTexture->surf.h)+int(TextBase.ppFonts[FontId]->h));
			ScrollOffsetV(-((int(caret_y-pTexture->surf.h)+int(TextBase.ppFonts[FontId]->h))-5));
			return;
		}
		
		if(key==_KEY_PGUP)
		{
			//InputStr((char*)glGetString(GL_EXTENSIONS));
			ScrollOffsetV(200);
			return;
		}
		
		if(key==_KEY_PGDN)
		{
			//InputStr("{page}\n{down}");
			ScrollOffsetV(-10);
			return;
		}

		if(key==_KEY_F1)
		{
			Redraw(0);
			return;
		}

		if(key==_KEY_F2)
		{
			ClearLinesTexture(0);
			return;
		}

		if(key==_KEY_F3)
		{
			ScrollOffsetV(10);
			return;
		}
		
		if(key==_KEY_F4)
		{
			ScrollAlignByCaretH(false);
			return;
		}
		
		if(key==_KEY_F5)
		{
			ScrollAlignByCaretV(false);
			return;
		}
	}
}

void cUiTextEdit::SetFont(int fid)
{
	FontId=fid;
	
	int cid='_';
	_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
//	_IMGSURF* ToSurf=&pTexture->surf;
	cursor_w=pCharacters[cid-32].w+pCharacters[cid-32].ofsx;
	cursor_h=pCharacters[cid-32].h;//+pCharacters[cid-32].ofsy;

	//int maxW=0;
	int maxH=0;
	
	for(int i=0;i<TextBase.ppFonts[FontId]->CharNum;++i)
	{
		//if(pChars[i].w+pChars[i].ofsx>maxW)maxW=pChars[i].w;
		bool calc=false;
		for(int j=0;j<KEY_BUFFER_SIZE;++j)
		{
			if(Input.KeyCharTab[j].upper==pCharacters[i].id || Input.KeyCharTab[j].lower==pCharacters[i].id){
				calc=true;
				break;
			}
		}
		
		if(calc)
		{
			int h=pCharacters[i].h+pCharacters[i].ofsy;
			if(h>maxH)maxH=h;
		}
	}
	
	cursor_offsety=maxH;
}

void cUiTextEdit::OnUpdate()
{
	if(Timer.ProgrammTime-caret_time>0.100f)
	{
		caret_time=Timer.ProgrammTime;
	
		_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
		_FONTSURF* FromSurf=&TextBase.ppFonts[FontId]->FontSurf;
		_IMGSURF* ToSurf=&pTexture->surf;
		char c='_';
		int cid=c-32;
	
		if(caret_switch && caret_show)
		{
			if(cid>-1)
			{
				if(cid<TextBase.ppFonts[FontId]->CharNum)
				{
					ClearRect(ToSurf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);

					TextBase.FontToTextureStretchRectColorMask(
						FromSurf,
						pCharacters[cid].x,pCharacters[cid].y,
						pCharacters[cid].w,pCharacters[cid].h,
						ToSurf,
						caret_x+pCharacters[cid].ofsx,caret_y+cursor_offsety,
						r,g,b
					);
				}
			}
		}else{
			ClearRect(ToSurf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);
		}
	
		caret_switch=!caret_switch;
	}
	
	glBindTexture( GL_TEXTURE_2D, pTexture->textureID );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, pTexture->surf.w, pTexture->surf.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTexture->surf.buff );

	glBindTexture( GL_TEXTURE_2D, 0 );
}

void cUiTextEdit::RedrawLines(int yofs,_line_item* line)
{
	_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf=&TextBase.ppFonts[FontId]->FontSurf;
	_IMGSURF* ToSurf=&pTexture->surf;
		
	_line_item* cur=Lines;
	
	int ypos=YStartOffset;

	while(cur)
	{
		if(ypos+int(TextBase.ppFonts[FontId]->h)>0)
		{
			cur->redraw(0,cur->length,FromSurf,ToSurf,pCharacters,FontId,XStartOffset,ypos,r,g,b);
		}

		ypos+=int(TextBase.ppFonts[FontId]->h);

		if(ypos>int(pTexture->surf.h)){
			break;
		}
		
		cur=cur->next;
	}
}

void cUiTextEdit::Redraw(int from_line,int count)
{
	if(!caret_switch)ClearRect(&pTexture->surf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);

	_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf=&TextBase.ppFonts[FontId]->FontSurf;
	_IMGSURF* ToSurf=&pTexture->surf;
		
	_line_item* cur=Lines;
	
	int ypos=YStartOffset;

	int nlines=count==-1?lines_count:count;
	
	for(int i=0;cur;++i)
	{
		if(ypos+int(TextBase.ppFonts[FontId]->h)>0 && i>=from_line && i<=nlines)
		{
			cur->redraw(0,cur->length,FromSurf,ToSurf,pCharacters,FontId,XStartOffset,ypos,r,g,b);
		}

		ypos+=int(TextBase.ppFonts[FontId]->h);

		if(ypos>int(pTexture->surf.h)){
			break;
		}
		
		cur=cur->next;
	}
}

void cUiTextEdit::GetPointsFromOffset(int offet,POINTI2* pret)
{
	int x=XStartOffset;
	int y=YStartOffset;
	
	for(int i=0;i<offet;++i)
	{
		int cid=buff[i]-32;
		
		if(cid>0)
		{
			x+=TextBase.ppFonts[FontId]->pCharacters[cid].w+TextBase.ppFonts[FontId]->pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
		}else{
			y+=int(TextBase.ppFonts[FontId]->h);
			x=XStartOffset;
		}
	}
	
	pret->x=x;
	pret->y=y;
}

void cUiTextEdit::SelectRight()
{
	if(!SelectDir)
	{
		SelectOfsA=text_ofs;
		SelectOfsB=text_ofs;
		SelectDir=1;
	}
	//printf("R %d %d | %d\n",SelectOfsA,SelectOfsB,SelectDir);
	
	if(SelectDir==-1)
	{
		//printf(" [%c]",buff[ofs]);
		int cid=buff[text_ofs]-32;
		
		if(!buff[text_ofs] || buff[text_ofs]=='\n')
		{
			cid=1;
		}
		
		_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
		ClearRect(&pTexture->surf,caret_x,caret_y,pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace,int(TextBase.ppFonts[FontId]->h));
		RedrawSurface(caret_x,caret_y,text_ofs,1,r,g,b,true);
		
		CaretRight();
		SelectOfsA=text_ofs;
				
		if(!(SelectOfsB-SelectOfsA))
		{
			SelectOfsA=text_ofs;
			SelectOfsB=text_ofs;
			SelectDir=1;
			SelectRender(0,0);
		}
		return;
	}
	//printf("%d | %d\n",SelectOfsA,SelectOfsB);
	CaretRight();
	SelectOfsB=text_ofs;
	SelectRender(0,0);
}

void cUiTextEdit::SelectLeft()
{
	if(!SelectDir)
	{
		SelectOfsA=text_ofs;
		SelectOfsB=text_ofs;
		SelectDir=-1;
	}
	//printf("L %d %d | %d\n",SelectOfsA,SelectOfsB,SelectOfsB-SelectOfsA);
	
	if(SelectDir==1)
	{
		//printf(" [%c]",buff[ofs]);
		int cid=buff[text_ofs]-32;
		
		if(!buff[text_ofs] || buff[text_ofs]=='\n')
		{
			cid=1;
		}
		
		_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
		ClearRect(&pTexture->surf,caret_x,caret_y,pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace,int(TextBase.ppFonts[FontId]->h));
		RedrawSurface(caret_x,caret_y,text_ofs,1,r,g,b,true);
		
		CaretLeft();
		SelectOfsB=text_ofs;
		
		if(!(SelectOfsB-SelectOfsA))
		{
			SelectDir=0;
		}
		return;
	}
	
	SelectRender(0,0);
	CaretLeft();
	SelectOfsA=text_ofs;
}

void cUiTextEdit::SelectUp()
{
	if(!SelectDir)
	{
		SelectOfsA=text_ofs;
		SelectOfsB=text_ofs;
		SelectDir=-1;
	}
	
	int i=text_ofs;
	for(;i>0 && buff[i]!='\n';--i)
	{
		
	}
}

void cUiTextEdit::SelectDown()
{
	
}

void cUiTextEdit::SelectEnd()
{
	if(SelectOfsB-SelectOfsA)
	{		
		//RedrawSurface(caret_x,caret_y,sofs,slen,r,b,g);
		
		char copy_buff[256];
		memcpy(copy_buff,&buff[SelectOfsA],SelectOfsB-SelectOfsA);
		copy_buff[SelectOfsB-SelectOfsA]=0;
//		printf("\"%s\"",copy_buff);
		
		POINTI2 start_pos;
		GetPointsFromOffset(SelectOfsA+1,&start_pos);
		
		RedrawSurface(start_pos.x,start_pos.y,SelectOfsA+1,SelectOfsB-SelectOfsA,r,g,b);
	}
	
	printf("SelectEnd %d %d | %d\n",SelectOfsA,SelectOfsB,SelectOfsB-SelectOfsA);
	
	SelectOfsA=0;
	SelectOfsB=0;
	SelectDir=0;
}

void cUiTextEdit::SelectRender(int offset,int count)
{
	//if(SelectOfsB-SelectOfsA)
	{
		int cid=buff[text_ofs]-32;
		
		if(!buff[text_ofs] || buff[text_ofs]=='\n')
		{
			cid=1;
		}
		_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
		ClearRect(&pTexture->surf,caret_x,caret_y,pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace,int(TextBase.ppFonts[FontId]->h),"\x0a\xff\x0a\x30");
		RedrawSurface(caret_x,caret_y,text_ofs,1,r,b,g,true);
	}
}

void cUiTextEdit::Backspace()
{
	if(!caret_switch)ClearRect(&pTexture->surf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);

	_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf=&TextBase.ppFonts[FontId]->FontSurf;
	_IMGSURF* ToSurf=&pTexture->surf;

	_line_item* cur_line=CurentLine;
	
	if(caret_char==0){

		if(cur_line->prev)
		{
			CaretLeft();
			cur_line=CurentLine;

			if(cur_line->next)
			{
				ClearLinesTexture(cur_line->next,caret_y+int(TextBase.ppFonts[FontId]->h));

				cur_line->set_str(cur_line->next->buffer,cur_line->length,cur_line->next->length,ToSurf,pCharacters,FontId,caret_x,caret_y);

				cur_line->redraw(caret_char,cur_line->length,FromSurf,ToSurf,pCharacters,FontId,caret_x,caret_y,r,g,b);

				_line_item* del_line=cur_line->next;
			
				cur_line->next=cur_line->next->next;

				del_line->next=0;
				delete del_line;

				--lines_count;

				RedrawLines(caret_y+int(TextBase.ppFonts[FontId]->h),cur_line->next);

			}
		}
		return;
	}

	--caret_char;

	int cid=uchar(cur_line->buffer[caret_char])-32;
	if(cid<0)cid=0;
	caret_x-=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;

	cur_line->clear_texture(caret_char,cur_line->length-caret_char,ToSurf,pCharacters,FontId,XStartOffset,caret_y);

	cur_line->move_left(caret_char,1,ToSurf,pCharacters,FontId,caret_x,caret_y);

	if(caret_x<0){
		int last_caret_x=caret_x;
		int ofs=cur_line->size_of_width(caret_char>=4?caret_char-4:0,caret_char,pCharacters,FontId)-last_caret_x;

		ScrollOffsetH(ofs);
		
		return;
	}
	
	cur_line->redraw(caret_char,CurentLine->length,FromSurf,ToSurf,pCharacters,FontId,caret_x,caret_y,r,g,b);
}

void cUiTextEdit::Delete()
{
	if(!caret_switch)ClearRect(&pTexture->surf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);

	_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf=&TextBase.ppFonts[FontId]->FontSurf;
	_IMGSURF* ToSurf=&pTexture->surf;

	_line_item* cur_line=CurentLine;
	
	if(caret_char>=cur_line->length){

		if(cur_line->next)
		{
			ClearLinesTexture(cur_line->next,caret_y+int(TextBase.ppFonts[FontId]->h));

			cur_line->set_str(cur_line->next->buffer,cur_line->length,cur_line->next->length,ToSurf,pCharacters,FontId,caret_x,caret_y);

			cur_line->redraw(caret_char,cur_line->length,FromSurf,ToSurf,pCharacters,FontId,caret_x,caret_y,r,g,b);

			_line_item* del_line=cur_line->next;
			
			cur_line->next=cur_line->next->next;

			del_line->next=0;
			delete del_line;

			--lines_count;

			RedrawLines(caret_y+int(TextBase.ppFonts[FontId]->h),cur_line->next);

		}
		return;
	}

	cur_line->clear_texture(caret_char,cur_line->length-caret_char,ToSurf,pCharacters,FontId,XStartOffset,caret_y);

	cur_line->move_left(caret_char,1,ToSurf,pCharacters,FontId,caret_x,caret_y);

	cur_line->redraw(caret_char,cur_line->length,FromSurf,ToSurf,pCharacters,FontId,caret_x,caret_y,r,g,b);
}

int cUiTextEdit::RedrawSurface(int Xofs,int Yofs,int ofs,int len,uchar r,uchar g,uchar b,bool NoClear)
{
	_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf=&TextBase.ppFonts[FontId]->FontSurf;
	_IMGSURF* ToSurf=&pTexture->surf;

	//memset(&ToSurf->buff[Xofs],0,(ToSurf->w*ToSurf->h)*ToSurf->pixel_size);
	
	int xpos=Xofs;
	int ypos=Yofs;
	for(int i=ofs;i<len+ofs;++i)
	{
		int cid=uchar(buff[i])-32;
		
		if(cid<0)cid=0;
			
		if(cid<TextBase.ppFonts[FontId]->CharNum)
		{
			if(!NoClear){
				ClearRect(ToSurf,xpos,ypos,(pCharacters[cid].ofsx+pCharacters[cid].w)+TextBase.ppFonts[FontId]->CharSpace,int(TextBase.ppFonts[FontId]->h));//(pCharacters[cid].ofsy+pCharacters[cid].h));
			}
			TextBase.FontToTextureStretchRectColorMask(
				FromSurf,
				pCharacters[cid].x,pCharacters[cid].y,
				pCharacters[cid].w,pCharacters[cid].h,
				ToSurf,
				xpos+pCharacters[cid].ofsx,ypos+pCharacters[cid].ofsy,
				r,g,b
			);
			
			xpos+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
		}
	
		switch(uchar(buff[i])-32)
		{
			case 0:
				//xpos+=TextBase.ppFonts[FontId]->w;
			break;
			case -22:
				ypos+=int(TextBase.ppFonts[FontId]->h);
				xpos=XStartOffset;
				//printf("--__\n");
			break;
		}
	}
	
	XposEnd=xpos;
	YposEnd=ypos;
	
	return 0;
}

void cUiTextEdit::SetLineStrFrom(int start,char* str,int len,_line_item* line)
{
	if(start+len>=line->size)return;
	memcpy(&line->buffer[start],str,len);
	line->length=start+len;
}

void cUiTextEdit::GetLineStrFrom(int start,char* str,int* plen,_line_item* line)
{
	if(start>=line->length)return;
	memcpy(str,&line->buffer[start],line->length-start);
	*plen=line->length-start;
	str[*plen]=0;
}

void cUiTextEdit::GetLineStrFrom(int start,char* str,int len,_line_item* line)
{
	if(start+len>=line->length)return;
	memcpy(str,&line->buffer[start],len);
}

cUiTextEdit::_line_item* cUiTextEdit::SplitTextOnLines(char* str,int len)
{
	_line_item* NewLines=new _line_item();
	_line_item* cur=NewLines;

	char* aofs=str;
	char* bofs=strchr(aofs,'\n')+1;

	do
	{
		if(bofs==(char*)1){
			break;
		}
		
		aofs=bofs;
		bofs=strchr(bofs,'\n')+1;
		
		if(!cur->next){
			cur->add_next_line(_TEXT_EDIT_START_MAX_LEN);
		}
		
		cur=cur->next;
		
	}while(aofs);

	return NewLines;
}

void cUiTextEdit::ClearLineTexture(int start,int yofs,_line_item* line)
{
	_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf=&TextBase.ppFonts[FontId]->FontSurf;
	_IMGSURF* ToSurf=&pTexture->surf;

	int start_x=XStartOffset;

	for(int i=0;i<start;++i)
	{
		int cid=uchar(line->buffer[i])-32;
		if(cid<0)cid=0;
		start_x+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
	}

	int w=0;

	for(int i=start;i<line->length;++i)
	{
		int cid=uchar(line->buffer[i])-32;
		if(cid<0)cid=0;
		w+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
		
		line->buffer[i]=0;
	}

	ClearRect(ToSurf,start_x,yofs,w,int(TextBase.ppFonts[FontId]->h));

	line->length=start;
}

void cUiTextEdit::ClearLinesTexture(int start_line_id)
{
	_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf=&TextBase.ppFonts[FontId]->FontSurf;
	_IMGSURF* ToSurf=&pTexture->surf;
		
	_line_item* cur=Lines;
	
	int ypos=YStartOffset;

	for(int i=0;cur;++i)
	{
		if(ypos+int(TextBase.ppFonts[FontId]->h)>0 && i>=start_line_id){
			cur->clear_texture(0,cur->length,ToSurf,pCharacters,FontId,XStartOffset,ypos);
		}

		ypos+=int(TextBase.ppFonts[FontId]->h);

		if(ypos>int(pTexture->surf.h)){
			break;
		}

		cur=cur->next;
	}
}

void cUiTextEdit::ClearLinesTexture(_line_item* line,int y)
{
	_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf=&TextBase.ppFonts[FontId]->FontSurf;
	_IMGSURF* ToSurf=&pTexture->surf;
		
	_line_item* cur=line;
	
	int ypos=y;

	for(int i=0;cur;++i)
	{
		if(ypos+int(TextBase.ppFonts[FontId]->h)>0)
		{
			cur->clear_texture(0,cur->length,ToSurf,pCharacters,FontId,XStartOffset,ypos);
		}

		ypos+=int(TextBase.ppFonts[FontId]->h);

		if(ypos>int(pTexture->surf.h)){
			break;
		}

		cur=cur->next;
	}
}

void cUiTextEdit::InputStr(char* str)
{
	if(!caret_switch)ClearRect(&pTexture->surf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);

	_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf=&TextBase.ppFonts[FontId]->FontSurf;
	_IMGSURF* ToSurf=&pTexture->surf;
		
	//int xpos=caret_x;
	//int ypos=caret_y;
	
	int str_len=strlen(str);
	char* aofs=str;
	char* bofs=strchr(aofs,'\n')+1;
	int len=0;

	char bdg_buff[256];

	if(bofs!=(char*)1)
	{
		len=(bofs-1)-aofs;

		int posy=caret_y;

		memcpy(bdg_buff,aofs,len);
		bdg_buff[len]=0;

		//printf("FIRST STRING [%s:%d] caret_char:%d\n",bdg_buff,len,caret_char);
		int residue_len=CurentLine->length-caret_char;//остаток

		CurentLine->cut(TmpBuff,caret_char,residue_len,ToSurf,pCharacters,FontId,XStartOffset,posy);
		TmpBuff[residue_len]=0;

		CurentLine->set_str(aofs,caret_char,len,ToSurf,pCharacters,FontId,XStartOffset,posy);
		CurentLine->redraw(caret_char,CurentLine->length,FromSurf,ToSurf,pCharacters,FontId,caret_x,posy,r,g,b);

		//printf("FIRST	[%s:%d:%x]",CurentLine->buffer,CurentLine->length,CurentLine);

		ClearLinesTexture(CurentLine->next,posy+int(TextBase.ppFonts[FontId]->h));

		_line_item* cur=CurentLine;

		aofs=bofs;
		bofs=strchr(bofs,'\n')+1;

		if(bofs!=(char*)1)
		{
			do
			{
				len=(bofs-1)-aofs;

				cur->add_next_line(len);
				++lines_count;
				++caret_line;
				cur=cur->next;

				posy+=int(TextBase.ppFonts[FontId]->h);

				cur->set_str(aofs,0,len,ToSurf,pCharacters,FontId,XStartOffset,posy);
				cur->redraw(0,cur->length,FromSurf,ToSurf,pCharacters,FontId,XStartOffset,posy,r,g,b);

				memcpy(bdg_buff,aofs,len);
				bdg_buff[len]=0;

				//printf("ADD	[%s:%d:%x] %d\n",cur->buffer,cur->length,cur,posy);
		
				aofs=bofs;
				bofs=strchr(bofs,'\n')+1;

				if(bofs==(char*)1)
				{
					break;
				}

			}while(aofs);
		}

		len=str_len-(aofs-str);

		cur->add_next_line(len+residue_len);
		++lines_count;
		
		//cur->clear_texture(0,cur->length,ToSurf,pCharacters,FontId,XStartOffset,posy);

		cur=cur->next;

		posy+=int(TextBase.ppFonts[FontId]->h);
		
		cur->set_str(aofs,0,len,ToSurf,pCharacters,FontId,XStartOffset,posy);
		cur->set_str(TmpBuff,len,residue_len,ToSurf,pCharacters,FontId,XStartOffset,posy);
		cur->redraw(0,cur->length,FromSurf,ToSurf,pCharacters,FontId,XStartOffset,posy,r,g,b);

		++caret_line;
		caret_char=len;
		caret_y=posy;
		caret_x=XStartOffset+cur->size_of_width(0,len,pCharacters,FontId);

		CurentLine=cur;

		//printf("LAST	[%s:%d:%x] :%d\n",cur->buffer,cur->length,cur,len);

		cur=cur->next;
		posy+=int(TextBase.ppFonts[FontId]->h);
		
		if(caret_x+int(TextBase.ppFonts[FontId]->w)<=0){
//			printf("HSCROLL:%d %d\n",caret_x,-(caret_x+int(TextBase.ppFonts[FontId]->w)));
			ScrollSetH(-(caret_x+int(TextBase.ppFonts[FontId]->w)));
		}
		
		/*if(caret_x+int(TextBase.ppFonts[FontId]->w)>=int(reg.x+reg.w)){
			printf("HSCROLL:%d %d\n",caret_x,caret_x-int(reg.x+reg.w));
			ScrollH(-((caret_x+int(TextBase.ppFonts[FontId]->w))-int(reg.x+reg.w)));
			return;
		}*/
		
		if(caret_y+int(TextBase.ppFonts[FontId]->h)>=int(reg.y+reg.h)){
			ScrollOffsetV(-((caret_y+int(TextBase.ppFonts[FontId]->h))-int(reg.y+reg.h)));
			return;
		}

		while(cur)
		{
			//printf("REDRAW	[%s:%d:%x] %d\n",cur->buffer,cur->length,cur,posy);
			cur->redraw(0,cur->length,FromSurf,ToSurf,pCharacters,FontId,XStartOffset,posy,r,g,b);
			posy+=int(TextBase.ppFonts[FontId]->h);

			cur=cur->next;
		}

	}else{

		CurentLine->set_str(str,caret_char,str_len,ToSurf,pCharacters,FontId,XStartOffset,caret_y);
		CurentLine->redraw(caret_char,CurentLine->length,FromSurf,ToSurf,pCharacters,FontId,caret_x,caret_y,r,g,b);

		caret_x=XStartOffset+CurentLine->size_of_width(0,caret_char+str_len,pCharacters,FontId);

		caret_char+=str_len;

		//printf("SINGLE	[%s:%d:%x]",CurentLine->buffer,CurentLine->length,CurentLine);
		if(caret_x+int(TextBase.ppFonts[FontId]->w)>=int(reg.x+reg.w)){
			//printf("HSCROLL:%d %d\n",caret_x,caret_x-int(reg.x+reg.w));
			ScrollOffsetH(-((caret_x+int(TextBase.ppFonts[FontId]->w))-int(reg.x+reg.w)));
			return;
		}
	}
}

char cUiTextEdit::GetCaretChar()
{
	return CurentLine->buffer[caret_char];
}

void cUiTextEdit::RenderLine(char* str,_line_item* line,int line_number)
{
	_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf=&TextBase.ppFonts[FontId]->FontSurf;
	_IMGSURF* ToSurf=&pTexture->surf;
	
	int str_len=strlen(str);
	
	_line_item* cur=Lines;
	
	int xpos=XStartOffset;
	int ypos=YStartOffset;
	
	while(cur->next){
		ypos+=int(TextBase.ppFonts[FontId]->h);
		cur=cur->next;
	}
	
	for(int i=0;i<cur->length;++i)
	{
		int cid=uchar(cur->buffer[i])-32;
		if(cid<0)cid=0;
		xpos+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
	}
	
	if(!cur->buffer){
		cur->size=_TEXT_EDIT_START_MAX_LEN;
		cur->buffer=new char[cur->size+1];
		cur->buffer[0]=0;
	}
	
	//printf("%s\n",cur->buffer);
	
	for(int i=0;i<str_len;++i)
	{
		if(str[i]=='\n')
		{
			cur->next=new _line_item;
			cur->next->prev=cur;
			cur=cur->next;
			cur->size=_TEXT_EDIT_START_MAX_LEN;
			cur->buffer=new char[cur->size+1];
			
			ypos+=int(TextBase.ppFonts[FontId]->h);
			xpos=XStartOffset;
		}else{
			cur->buffer[cur->length]=str[i];
			cur->buffer[cur->length+1]=0;
			
			if(++cur->length>=cur->size)
			{
				char* new_buff=new char[cur->size+_TEXT_EDIT_START_MAX_LEN];
				memcpy(new_buff,cur->buffer,cur->size+1);
				delete[] cur->buffer;
				cur->size+=_TEXT_EDIT_START_MAX_LEN;
				cur->buffer=new_buff;
			}
			
			int cid=uchar(str[i])-32;
			
			if(cid<0)cid=0;
			
			TextBase.FontToTextureStretchRectColorMask(
				FromSurf,
				pCharacters[cid].x,pCharacters[cid].y,
				pCharacters[cid].w,pCharacters[cid].h,
				ToSurf,
				xpos+pCharacters[cid].ofsx,ypos+pCharacters[cid].ofsy,
				r,g,b
			);
			
			xpos+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
		}
	}
}

void cUiTextEdit::CaretLeft()
{
	if(!caret_switch)ClearRect(&pTexture->surf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);

	_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf=&TextBase.ppFonts[FontId]->FontSurf;
	_IMGSURF* ToSurf=&pTexture->surf;
		
	_line_item* cur=CurentLine;
	
	int xpos=caret_x;
	int ypos=caret_y;

	if(caret_char-1<0)
	{
		if(caret_line-1>-1)
		{
			--caret_line;

			if(cur->prev)
			{
				cur=cur->prev;
			}

			//printf("%s\n",cur->buffer);

			xpos=XStartOffset;
			ypos-=int(TextBase.ppFonts[FontId]->h);

			for(int i=0;i<cur->length;++i)
			{
				int cid=uchar(cur->buffer[caret_char++])-32;
				if(cid<0)cid=0;

				xpos+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
			}
		}
	}else{
		

		int cid=uchar(cur->buffer[--caret_char])-32;
		if(cid<0)cid=0;

		//printf("%d %c<--",caret_char,cid+32);

		xpos-=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
	}

	CurentLine=cur;
	caret_x=xpos;
	caret_y=ypos;
}

void cUiTextEdit::CaretRight()
{
	if(!caret_switch)ClearRect(&pTexture->surf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);

	_FONT_CHAR* pCharacters=TextBase.ppFonts[FontId]->pCharacters;
	_FONTSURF* FromSurf=&TextBase.ppFonts[FontId]->FontSurf;
	_IMGSURF* ToSurf=&pTexture->surf;
		
	_line_item* cur=CurentLine;
	
	int xpos=caret_x;
	int ypos=caret_y;

	if(caret_char+1>cur->length)
	{
		if(caret_line+1<=lines_count)
		{
			++caret_line;
			caret_char=0;
			if(cur->next)
			{
				cur=cur->next;
			}

			//printf("%s\n",cur->buffer);

			xpos=XStartOffset;
			ypos+=int(TextBase.ppFonts[FontId]->h);

			/*for(int i=0;i<cur->length;++i)
			{
				int cid=uchar(cur->buffer[caret_char++])-32;
				if(cid<0)cid=0;

				xpos+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
			}*/
		}
	}else{
		

		int cid=uchar(cur->buffer[caret_char++])-32;
		if(cid<0)cid=0;

		//printf("%d %c-->",caret_char,cid+32);

		xpos+=pCharacters[cid].w+pCharacters[cid].ofsx+TextBase.ppFonts[FontId]->CharSpace;
	}

	CurentLine=cur;
	caret_x=xpos;
	caret_y=ypos;
}

void cUiTextEdit::CaretUp()
{
}

void cUiTextEdit::CaretDown()
{
}

void cUiTextEdit::ScrollAlignByCaretV(bool top)
{
	if(!caret_switch)ClearRect(&pTexture->surf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);

	ClearLinesTexture(0);
	YStartOffset=caret_y-pTexture->surf.h;
	//caret_y=0;
	Redraw(0);
}

void cUiTextEdit::ScrollAlignByCaretH(bool left)
{
	if(!caret_switch)ClearRect(&pTexture->surf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);

	ClearLinesTexture(0);
	XStartOffset=caret_x;
	//caret_x=0;
	Redraw(0);
}

void cUiTextEdit::ScrollOffsetV(int ofs)
{
	if(!caret_switch)ClearRect(&pTexture->surf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);

	ClearLinesTexture(0);
	YStartOffset+=ofs;
	caret_y+=ofs;
	Redraw(0);
}

void cUiTextEdit::ScrollOffsetH(int ofs)
{
	if(!caret_switch)ClearRect(&pTexture->surf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);

	ClearLinesTexture(0);

	XStartOffset+=ofs;
	caret_x+=ofs;
	
	Redraw(0);
}

void cUiTextEdit::ScrollSetV(int pos)
{
//	printf("%d %d %d",caret_y,YStartOffset,pos);
	if(!caret_switch)ClearRect(&pTexture->surf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);

	ClearLinesTexture(0);
	YStartOffset=pos;
	//caret_y+=pos;
	//YStartOffset=YStartOffset+(-pos);
	//caret_y=pos+(pos-caret_y);
	
	caret_y=caret_line*int(TextBase.ppFonts[FontId]->h)+pos;
	
	Redraw(0);
}

void cUiTextEdit::ScrollSetH(int pos)
{
	if(!caret_switch)ClearRect(&pTexture->surf,caret_x,caret_y+cursor_offsety,cursor_w,cursor_h);

	ClearLinesTexture(0);
	XStartOffset=pos;
	caret_x=pos;
	Redraw(0);
}

























