GL_SHADER VShaderVertex VERTEX_SHADER
{
	ViewProjWorldMx:VIEWPROJWORLDMX
	WorldMx : WORLDMX

	vVertex:VERTEX
	vNormal:NORMAL
	vColor:COLOR
	vTexCoord:TEXCOORD0

	programm
	{
		#version 130

		in vec3 vVertex;
		in vec3 vNormal;
		in vec3 vColor;
		in vec2 vTexCoord;

		out vec3 WorldPos;
		out vec3 WorldNormal;
		//out vec3 Color;
		out vec2 TexCoord;

		uniform vec3 ViewPos;
		uniform mat4 ViewProjWorldMx;
		uniform mat4 WorldMx;

		void main()
		{
			//Color=vColor;
			
			WorldPos=(WorldMx*vec4(vVertex,1.0)).xyz;
			WorldNormal=(WorldMx*vec4(vNormal,1.0)).xyz;

			TexCoord=vTexCoord;
			gl_Position = ViewProjWorldMx*vec4(vVertex,1.0);
		}
	}
}

GL_SHADER FShaderVertex FRAGMENT_SHADER
{
	tex : SAMPLER0

	LightPosCone:LIGHTPOSCONE
	LightColorPower:LIGHTCOLORPOWER
	LightDirSpread:LIGHTDIRSPREAD

	programm
	{
		#define MAXLIGHTS 1

		varying vec2 TexCoord;
		varying vec3 WorldNormal;
		varying vec3 WorldPos;
		//varying vec3 Color;

		uniform sampler2D tex;

		uniform vec4 LightPosCone[MAXLIGHTS];
		uniform vec4 LightColorPower[MAXLIGHTS];
		uniform vec4 LightDirSpread[MAXLIGHTS];

		float Distance(vec3 vPoint1, vec3 vPoint2)
		{
			float dist = sqrt( (vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
				(vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) +
				(vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z) );

			return dist;
		}

		void main()
		{
			vec4 map=texture2D(tex,TexCoord);

			vec3 diffContrib=vec3(0.0,0.0,0.0);
			
			for(int i=0;i<MAXLIGHTS;i++)
			{
				vec3 LightVec=normalize(LightPosCone[i].xyz-WorldPos);
				
				float spot = 1.0;
				if(LightPosCone[i].w!=-1.0)
				{
					float d=dot(
						-LightVec,
						normalize(LightDirSpread[i].xyz)
						);

					float spread=LightDirSpread[i].w;

					spot = clamp(
						pow(
							clamp(d/cos(LightPosCone[i].w),0.0,1.0),
							spread
							),
					0.0,1.0);
				}
				
				float dist=Distance(WorldPos,LightPosCone[i].xyz);
				
				//diffContrib+=((LightColorPower[i].rgb*LightColorPower[i].w)/dist)*dot(WorldNormal,LightVec);
				diffContrib+=(LightColorPower[i].rgb)*dot(WorldNormal,LightVec);
			}

			//gl_FragColor=vec4(map.rgb * diffContrib,map.a);
			gl_FragColor=vec4(diffContrib,1.0);
			//gl_FragColor=vec4(WorldNormal,1.0);
		}
	}
}

GL_SHADER VShadeSkinnedWT VERTEX_SHADER
{
	ViewProjWorldMx:VIEWPROJWORLDMX
	WorldMx : WORLDMX
	WorldDirMx:WORLDDIRMX
	vVertex:VERTEX
	vNormal:NORMAL
	vTexCoord:TEXCOORD0
	Wofs:WEIGHTOFS;

	ViewPos:VIEWPOS

	LightViewPos:LIGHTVIEWPOS;

	WeightsTex : WEIGHTTEX
	
	WeightsSize: WEIGHTSIZE

	Bones:BONES;

	programm
	{
		#version 130

		//#define MAXLIGHTS 2

		in vec3 vVertex;
		in vec3 vNormal;
		in vec2 vTexCoord;
		in float Wofs;

		varying vec2 TexCoord;
		varying vec3 WorldNormal;
		varying vec3 WorldPos;

		uniform vec3 ViewPos;

		uniform sampler2D WeightsTex;
		uniform float WeightsSize;

		uniform mat4 ViewProjWorldMx;
		uniform mat4 WorldMx;
		uniform mat4 WorldDirMx;

		uniform mat4 Bones[200];
		
		void main()
		{
			vec4 WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+0.5)),0.0));//ofsA*(WeightsSize/1000.0)
			
			int Bn=int(WBuff.x);
			
			vec3 Pos=vec3(0,0,0);
			vec3 Normal=vec3(0,0,0);

			int BId=0;
			float Weight=0.0;
			
			switch(Bn)
			{
			case 0:
				Pos=vVertex;
				Normal=vNormal;
			break;
			case 1:
				BId=int(WBuff.y);
				Weight=WBuff.z;
				Pos=((vec4(vVertex,1.0)*Bones[BId])*Weight).xyz;
				Normal=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
			break;
			case 2:
				BId=int(WBuff.y);
				Weight=WBuff.z;

				Pos=((vec4(vVertex,1.0)*Bones[BId])*Weight).xyz;
				Normal=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;

				BId=int(WBuff.w);

				WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+1.5)),0.0));
				Weight=WBuff.x;
				Pos+=((vec4(vVertex,1.0)*Bones[BId])*Weight).xyz;
				Normal+=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
			break;
			case 3:
				BId=int(WBuff.y);
				Weight=WBuff.z;

				Pos=((vec4(vVertex,1.0)*Bones[BId])*Weight).xyz;
				Normal=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;

				BId=int(WBuff.w);

				WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+1.5)),0.0));
				Weight=WBuff.x;

				Pos+=((vec4(vVertex,1.0)*Bones[BId])*Weight).xyz;
				Normal+=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;

				BId=int(WBuff.y);
				Weight=WBuff.z;
				Pos+=((vec4(vVertex,1.0)*Bones[BId])*Weight).xyz;
				Normal+=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
			break;
			case 4:
				BId=int(WBuff.y);
				Weight=WBuff.z;

				Pos=((vec4(vVertex,1.0)*Bones[BId])*Weight).xyz;
				Normal=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;

				BId=int(WBuff.w);

				WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+1.5)),0.0));
				Weight=WBuff.x;

				Pos+=((vec4(vVertex,1.0)*Bones[BId])*Weight).xyz;
				Normal+=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;

				BId=int(WBuff.y);
				Weight=WBuff.z;
				Pos+=((vec4(vVertex,1.0)*Bones[BId])*Weight).xyz;
				Normal+=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;

				BId=int(WBuff.w);

				WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+2.5)),0.0));

				Weight=WBuff.x;

				Pos+=((vec4(vVertex,1.0)*Bones[BId])*Weight).xyz;
				Normal+=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
			break;
			default:
				Pos=vVertex;
				Normal=vNormal;
			}

			//WorldNormal=normalize((WorldDirMx*vec4(vNormal,1.0)).xyz);
			WorldNormal=normalize((WorldMx*vec4(Normal,1.0)).xyz);

			WorldPos=(WorldMx*vec4(Pos,1.0)).xyz;
			TexCoord=vTexCoord;
			
			gl_Position = ViewProjWorldMx*vec4(Pos,1.0);
		}
	}
}

GL_SHADER FShader FRAGMENT_SHADER
{
	tex : SAMPLER0

	LightPosCone:LIGHTPOSCONE
	LightColorPower:LIGHTCOLORPOWER
	LightDirSpread:LIGHTDIRSPREAD

	WorldMx : WORLDMX
	ViewInvMx:VIEWINVMX

	programm
	{
		#version 120

		#define MAXLIGHTS 1

		varying vec2 TexCoord;
		varying vec3 WorldNormal;
		varying vec3 WorldPos;

		uniform sampler2D tex;

		uniform vec3 ViewPos;

		uniform vec4 LightPosCone[MAXLIGHTS];
		uniform vec4 LightColorPower[MAXLIGHTS];
		uniform vec4 LightDirSpread[MAXLIGHTS];

		uniform mat4 WorldMx;
		uniform mat4 ViewInvMx;

		float Distance(vec3 vPoint1, vec3 vPoint2)
		{
			float dist = sqrt( (vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
				(vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) +
				(vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z) );

			return dist;
		}

		void main()
		{
			vec4 map =texture2D(tex,TexCoord);
			
			vec3 ViewVec=normalize(vec3(ViewInvMx[3][0],ViewInvMx[3][1],ViewInvMx[3][2])-WorldPos);

			vec3 specContrib=vec3(0.0,0.0,0.0);
			vec3 diffContrib=vec3(0.0,0.0,0.0);
			vec3 testContrib=vec3(0.0,0.0,0.0);
			float specA=0;
			for(int i=0;i<MAXLIGHTS;i++)
			{
				vec3 LightVec=normalize(LightPosCone[i].xyz-WorldPos);

				float spot = 1.0;
				if(LightPosCone[i].w!=-1.0)
				{
					float d=dot(
						-LightVec,
						normalize(LightDirSpread[i].xyz)
						);

					float spread=LightDirSpread[i].w;

					spot = clamp(
						pow(
							clamp(d/cos(LightPosCone[i].w),0.0,1.0),//LightsCone[i]
							spread
							),
					0.0,1.0);
				}
				
				float ldn = dot(LightVec,WorldNormal);
				
				diffContrib += (LightColorPower[i].rgb*(max(ldn, 0.0)*spot))*LightColorPower[i].w;
				
				specContrib += LightColorPower[i].rgb* pow( clamp( dot(reflect( -ViewVec, WorldNormal ), LightVec ) ,0.0, 1.0 ), Distance(WorldPos,LightPosCone[i].xyz) )*spot;
			}

			vec3 ambient=vec3(0.25,0.25,0.25);
			//vec3 result=map.rgb*max((diffContrib + specContrib),ambient);
			vec3 result=vec3(1.0,1.0,1.0)*max((diffContrib + specContrib),ambient);

			gl_FragColor=vec4(result.rgb,map.a);
			//gl_FragColor=vec4(map.rgb,map.a);
		}
	}
}

technique tech_vertex
{
	pass p0
	{
		VertexShader = compile VShaderVertex();
		FragnentShader = compile FShaderVertex();
	}
}

technique tech_skinnwt
{	
	pass p0
	{
		VertexShader = compile VShadeSkinnedWT();
		FragnentShader = compile FShader();
	}
}



































