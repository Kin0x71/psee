GL_SHADER FShadeWindowPlane FRAGMENT_SHADER
{
	TexBackground:SAMPLER0
	programm
	{
		#version 130
		uniform sampler2D TexBackground;
		varying vec2 texl;
		void main()
		{
			vec4 resultBg=texture2D(TexBackground,texl);

			gl_FragData[0]=resultBg;
		}
	}
}

GL_SHADER VShadeWindowResizablePlane VERTEX_SHADER
{
	vTexCoord:TEXCOORD0
	vVertex:VERTEX
	ProjMx:UIPROJMX
	Rect:MESH_UIREC_VEC4
	Tex:MESH_TEX_VEC4

	programm
	{
		#version 130
		in vec3 vVertex;
		//in vec2 vTexCoord;
		uniform mat4 ProjMx;
		uniform vec4 Rect;
		uniform vec4 Tex;
		varying vec2 texl;

		void main()
		{
			//texl=vTexCoord;

			vec3 Vert=vec3(vVertex.x,vVertex.y,vVertex.z);

			 if(vVertex.x<0.0 && vVertex.y>0.0)
			{
				Vert.x=Rect.x;
				Vert.y=Rect.y;
				texl.x=Tex.x;
				texl.y=Tex.y;
			}else if(vVertex.x<0.0 && vVertex.y<0.0){
				Vert.x=Rect.x;
				Vert.y=Rect.w;
				texl.x=Tex.x;
				texl.y=Tex.w;
			}else if(vVertex.x>0.0 && vVertex.y>0.0){
				Vert.x=Rect.z;
				Vert.y=Rect.y;
				texl.x=Tex.z;
				texl.y=Tex.y;
			}else if(vVertex.x>0.0 && vVertex.y<0.0){
				Vert.x=Rect.z;
				Vert.y=Rect.w;
				texl.x=Tex.z;
				texl.y=Tex.w;
			}
			
			//gl_Position = ProjMx*vec4(vec3(Vert.x,Vert.y,0.0),1.0);
			gl_Position = ProjMx*vec4(vec3(Vert.x,Vert.y,0.0),1.0);
		}
	}
}

GL_SHADER VShadeText VERTEX_SHADER
{
	vTexCoord:TEXCOORD0
	vVertex:VERTEX
	ProjMx:UIPROJMX
	Rect:MESH_UIREC_VEC4
	TexOffs:MESH_TEX_VEC4

	programm
	{
		#version 130
		in vec3 vVertex;
		in vec2 vTexCoord;
		uniform mat4 ProjMx;
		uniform vec4 Rect;
		uniform vec4 TexOffs;
		varying vec2 texl;

		void main()
		{
			texl=vec2(vTexCoord.x+TexOffs.x,vTexCoord.y+TexOffs.y);

			vec3 Vert=vec3(vVertex.x,vVertex.y,vVertex.z);

			if(vVertex.x<0.0 && vVertex.y>0.0)
			{
				Vert.x=Rect.x;
				Vert.y=Rect.y;
			}else if(vVertex.x<0.0 && vVertex.y<0.0){
				Vert.x=Rect.x;
				Vert.y=Rect.w;
			}else if(vVertex.x>0.0 && vVertex.y>0.0){
				Vert.x=Rect.z;
				Vert.y=Rect.y;
			}else if(vVertex.x>0.0 && vVertex.y<0.0){
				Vert.x=Rect.z;
				Vert.y=Rect.w;
			}
			
			gl_Position = ProjMx*vec4(vec3(Vert.x,Vert.y,0.0),1.0);
		}
	}
}

GL_SHADER FShadeText FRAGMENT_SHADER
{
	TexText:SAMPLER0
	TextureSize:VECTOR2_A

	programm
	{
		#version 130
		varying vec2 texl;
		uniform sampler2D TexText;
		uniform vec2 TextureSize;

		void main()
		{
			vec4 resultBt=texture2D(TexText,texl);
			vec4 lpix=texture2D(TexText,vec2(texl.x+(1.0/(TextureSize.x/1.0)),texl.y));
			vec4 tpix=texture2D(TexText,vec2(texl.x,texl.y-(1.0/(TextureSize.y/1.0))));
			vec4 rpix=texture2D(TexText,vec2(texl.x-(1.0/(TextureSize.x/1.0)),texl.y));
			vec4 bpix=texture2D(TexText,vec2(texl.x,texl.y+(1.0/(TextureSize.y/1.0))));

			if(resultBt.a<=0.01)
			{
				if(lpix.a>=0.01)
				{
					resultBt=vec4(vec3(lpix.rgb)/1.5,0.3);
				}else if(tpix.a>=0.01)
				{
					resultBt=vec4(vec3(tpix.rgb)/1.5,0.3);
				}else if(rpix.a>=0.01)
				{
					resultBt=vec4(vec3(rpix.rgb)/1.5,0.3);
				}else if(bpix.a>=0.01)
				{
					resultBt=vec4(vec3(bpix.rgb)/1.5,0.3);
				}else{
					resultBt=vec4(0.0,0.0,0.0,0.0);//empty
				}
			}
/*
			if((lpix.a>=0.01 || tpix.a>=0.01 || rpix.a>=0.01 || bpix.a>=0.01) && resultBt.a<=0.01)
			{
				resultBt.r=resultBt.r/0.5;
				resultBt.g=resultBt.g/0.5;
				resultBt.b=resultBt.b/0.5;
				resultBt.a=1.0;
			}else	if(resultBt.a<=0.01)
			{
				resultBt.r=0.0;
				resultBt.g=0.0;
				resultBt.b=0.0;
				resultBt.a=1.0;
			}
*/
			gl_FragData[0]=resultBt;
		}
	}
}

GL_SHADER VShadeButton VERTEX_SHADER
{
	vTexCoord:TEXCOORD0
	vVertex:VERTEX
	ProjMx:UIPROJMX
	Rect:MESH_UIREC_VEC4
	//Tex:MESH_TEX_VEC4

	programm
	{
		#version 130
		in vec3 vVertex;
		in vec2 vTexCoord;
		uniform mat4 ProjMx;
		uniform vec4 Rect;
		//uniform vec4 Tex;
		varying vec2 texl;

		void main()
		{
			texl=vTexCoord;

			vec3 Vert=vec3(vVertex.x,vVertex.y,vVertex.z);

			if(vVertex.x<0.0 && vVertex.y>0.0)
			{
				Vert.x=Rect.x;
				Vert.y=Rect.y;
				//texl.x=Tex.x;
				//texl.y=Tex.y;
			}else if(vVertex.x<0.0 && vVertex.y<0.0){
				Vert.x=Rect.x;
				Vert.y=Rect.w;
				//texl.x=Tex.x;
				//texl.y=Tex.w;
			}else if(vVertex.x>0.0 && vVertex.y>0.0){
				Vert.x=Rect.z;
				Vert.y=Rect.y;
				//texl.x=Tex.z;
				//texl.y=Tex.y;
			}else if(vVertex.x>0.0 && vVertex.y<0.0){
				Vert.x=Rect.z;
				Vert.y=Rect.w;
				//texl.x=Tex.z;
				//texl.y=Tex.w;
			}
			
			gl_Position = ProjMx*vec4(vec3(Vert.x,Vert.y,0.0),1.0);
		}
	}
}

GL_SHADER FShadeButton FRAGMENT_SHADER
{
	TexButton:SAMPLER0
	TextureSize:VECTOR2_A

	programm
	{
		#version 130
		varying vec2 texl;
		uniform sampler2D TexButton;
		uniform vec2 TextureSize;

		void main()
		{
			vec4 resultBt=texture2D(TexButton,texl);
			
			vec4 lpix=texture2D(TexButton,vec2(texl.x+(1.0/(TextureSize.x/1.0)),texl.y));
			vec4 tpix=texture2D(TexButton,vec2(texl.x,texl.y-(1.0/(TextureSize.y/1.0))));
			vec4 rpix=texture2D(TexButton,vec2(texl.x-(1.0/(TextureSize.x/1.0)),texl.y));
			vec4 bpix=texture2D(TexButton,vec2(texl.x,texl.y+(1.0/(TextureSize.y/1.0))));

			if(resultBt.a<=0.01)
			{
				if(lpix.a>=0.01)
				{
					resultBt=vec4(vec3(lpix.rgb)/1.5,0.3);
				}else if(tpix.a>=0.01)
				{
					resultBt=vec4(vec3(tpix.rgb)/1.5,0.3);
				}else if(rpix.a>=0.01)
				{
					resultBt=vec4(vec3(rpix.rgb)/1.5,0.3);
				}else if(bpix.a>=0.01)
				{
					resultBt=vec4(vec3(bpix.rgb)/1.5,0.3);
				}else{
					resultBt=vec4(1.0,0.0,1.0,0.3);
				}
			}
			/*
			if((lpix.a>=0.01 || tpix.a>=0.01 || rpix.a>=0.01 || bpix.a>=0.01) && resultBt.a<=0.01)
			{
				resultBt.r=resultBt.r/0.5;
				resultBt.g=resultBt.g/0.5;
				resultBt.b=resultBt.b/0.5;
				resultBt.a=1.0;
			}else	if(resultBt.a<=0.01)
			{
				resultBt.r=0.0;
				resultBt.g=0.0;
				resultBt.b=0.0;
				resultBt.a=1.0;
			}
			*/
			gl_FragData[0]=resultBt;
		}
	}
}

GL_SHADER VShadeSizindTexture VERTEX_SHADER
{
	vTexCoord:TEXCOORD0
	vVertex:VERTEX
	ProjMx:PROJMX
	Rect:WEIGHTS
	TexRect:MESH_VEC4_A

	programm
	{
		#version 130
		in vec3 vVertex;
		in vec2 vTexCoord;
		uniform mat4 ProjMx;
		uniform vec4 Rect;
		uniform vec4 TexRect;
		varying vec2 texl;

		void main()
		{
			texl=vTexCoord;

			vec3 Vert=vec3(vVertex.x,vVertex.y,vVertex.z);

						if(vVertex.x<0.0 && vVertex.y>0.0)
			{
				Vert.x=Rect.x;
				Vert.y=Rect.y;
				texl.x=TexRect.x;
				texl.y=TexRect.y;
			}else if(vVertex.x<0.0 && vVertex.y<0.0){
				Vert.x=Rect.x;
				Vert.y=Rect.w;
				texl.x=TexRect.x;
				texl.y=TexRect.w;
			}else if(vVertex.x>0.0 && vVertex.y>0.0){
				Vert.x=Rect.z;
				Vert.y=Rect.y;
				texl.x=TexRect.z;
				texl.y=TexRect.y;
			}else if(vVertex.x>0.0 && vVertex.y<0.0){
				Vert.x=Rect.z;
				Vert.y=Rect.w;
				texl.x=TexRect.z;
				texl.y=TexRect.w;
			}
			
			gl_Position = ProjMx*vec4(vec3(Vert.x,Vert.y,-0.1),1.0);
		}
	}
}

GL_SHADER FShadeBorder FRAGMENT_SHADER
{
	TexBackground:SAMPLER0
	programm
	{
		#version 130
		uniform sampler2D TexBackground;
		varying vec2 texl;
		//varying vec4 out_color;
		void main()
		{
			vec4 resultBg=texture2D(TexBackground,texl);

			gl_FragData[0]=resultBg;
		}
	}
}

GL_SHADER VShadeBorder VERTEX_SHADER
{
	iv:WEIGHTOFS
	vVertex:VERTEX
	vTexCoord:TEXCOORD0
	ProjMx:PROJMX
	WorldMx:WORLDMX
	Rect:WEIGHTS
	Verts:MESH_VEC4_A

	programm
	{
		#version 130
		//in vec3 vVertex;
		//in vec2 vTexCoord;
		in float iv;
		uniform mat4 ProjMx;
		uniform mat4 WorldMx;
		uniform vec4 Rect;
		uniform vec4 Verts[16];
		varying vec2 texl;
		//varying vec4 out_color;

		void main()
		{
			texl=vec2(Verts[int(iv)].z,Verts[int(iv)].w);

			//out_color=vec4(0.0,0.0,0.0,1.0);

			gl_Position = ProjMx*vec4(vec3(Verts[int(iv)].x,Verts[int(iv)].y,-0.1),0.1);
		}
	}
}

technique tech_window_base
{
	pass p0
	{
		VertexShader = compile VShadeWindowResizablePlane();
		FragnentShader = compile FShadeWindowPlane();
	}
}

technique tech_window_text
{
	pass p0
	{
		VertexShader = compile VShadeText();
		FragnentShader = compile FShadeText();
	}
}

technique tech_window_button
{
	pass p0
	{
		VertexShader = compile VShadeButton();
		FragnentShader = compile FShadeButton();
	}
}

technique tech_sizing_texture
{
	pass p0
	{
		VertexShader = compile VShadeBorder();
		FragnentShader = compile FShadeBorder();
	}
}