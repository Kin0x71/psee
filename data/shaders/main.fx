GL_SHADER VShaderVertex VERTEX_SHADER
{
	ProjViewWorldMx:PROJVIEWWORLDMX
	ViewWorldMx:VIEWWORLDMX
	WorldMx : WORLDMX
	
	LightSpaceMatrices:LIGHTSPECMXARRAY

	vVertex:VERTEX
	vNormal:NORMAL
	vTexCoord:TEXCOORD0

	programm
	{
		//#version 130
		#version 330 core
		
		#define MAXLIGHTS 3

		in vec3 vVertex;
		in vec3 vNormal;
		in vec2 vTexCoord;

		uniform vec3 ViewPos;
		uniform mat4 ViewWorldMx;
		uniform mat4 ProjViewWorldMx;
		uniform mat4 WorldMx;
		uniform mat4 LightSpaceMatrices[MAXLIGHTS];
		
		out vec3 WorldPos;
		out vec3 WorldNormal;
		out vec2 TexCoord;
		out vec4 FragPosLightSpaceArray[MAXLIGHTS];
		out vec4 EyeSpacePosition;

		void main()
		{			
			WorldPos=(WorldMx*vec4(vVertex,1.0)).xyz;
			//WorldNormal=normalize((WorldMx*vec4(vNormal,1.0)).xyz);
			WorldNormal=normalize(transpose(inverse(mat3(WorldMx))) * vNormal);
			//WorldNormal=mat3(WorldMx) * vNormal;
			//WorldNormal=vNormal;
			TexCoord=vTexCoord;
			
			for(int i=0;i<MAXLIGHTS;i++)
			{
				FragPosLightSpaceArray[i]=LightSpaceMatrices[i] * vec4(WorldPos,1.0);
			}
			
			EyeSpacePosition = ViewWorldMx * vec4(vVertex, 1.0);
			
			gl_Position = ProjViewWorldMx*vec4(vVertex,1.0);
		}
	}
}

GL_SHADER VShaderVertexNormalmap VERTEX_SHADER
{
	ProjViewWorldMx:PROJVIEWWORLDMX
	ViewWorldMx:VIEWWORLDMX
	WorldMx : WORLDMX
	
	LightSpaceMatrices:LIGHTSPECMXARRAY

	vVertex:VERTEX
	vNormal:NORMAL
	vBinormal:BINORMAL
	vTangent:TANGENT
	vTexCoord:TEXCOORD0

	programm
	{
		//#version 130
		#version 330 core
		
		#define MAXLIGHTS 3

		in vec3 vVertex;
		in vec3 vNormal;
		in vec3 vBinormal;
		in vec3 vTangent;
		in vec2 vTexCoord;

		uniform vec3 ViewPos;
		uniform mat4 ViewWorldMx;
		uniform mat4 ProjViewWorldMx;
		uniform mat4 WorldMx;
		uniform mat4 LightSpaceMatrices[MAXLIGHTS];
		
		out vec3 WorldPos;
		out vec3 WorldNormal;
		out vec3 WorldBinorm;
		out vec3 WorldTangent;
		out vec2 TexCoord;
		out vec4 FragPosLightSpaceArray[MAXLIGHTS];
		out vec4 EyeSpacePosition;

		void main()
		{			
			//WorldPos=(WorldMx*vec4(vVertex,1.0)).xyz;
			//WorldNormal=normalize((WorldMx*vec4(vNormal,1.0)).xyz);
			//WorldNormal=mat3(WorldMx) * vNormal;
			//WorldNormal=vNormal;
			WorldPos=vec3(WorldMx * vec4(vVertex,1.0));
			WorldNormal=normalize(transpose(inverse(mat3(WorldMx))) * vNormal);
			WorldBinorm=normalize(transpose(inverse(mat3(WorldMx))) * vBinormal);
			WorldTangent=normalize(transpose(inverse(mat3(WorldMx))) * vTangent);
			TexCoord=vTexCoord;
			
			for(int i=0;i<MAXLIGHTS;i++)
			{
				FragPosLightSpaceArray[i]=LightSpaceMatrices[i] * vec4(WorldPos,1.0);
			}
			
			EyeSpacePosition = ViewWorldMx * vec4(vVertex, 1.0);
			
			gl_Position = ProjViewWorldMx*vec4(vVertex,1.0);
		}
	}
}

GL_SHADER VShadeSkinnedWT VERTEX_SHADER
{
	ProjViewWorldMx:PROJVIEWWORLDMX
	ViewWorldMx:VIEWWORLDMX
	WorldMx:WORLDMX
	vVertex:VERTEX
	vNormal:NORMAL
	vTexCoord:TEXCOORD0
	Wofs:WEIGHTOFS;
	
	LightSpaceMatrices:LIGHTSPECMXARRAY

	ViewPos:VIEWPOS

	LightViewPos:LIGHTVIEWPOS;

	WeightsTex : WEIGHTTEX
	
	WeightsSize: WEIGHTSIZE

	Bones:BONES;

	programm
	{
		//#version 130
		#version 330 core
		
		#define MAXLIGHTS 3

		in vec3 vVertex;
		in vec3 vNormal;
		in vec2 vTexCoord;
		in float Wofs;

		uniform vec3 ViewPos;

		uniform sampler2D WeightsTex;
		uniform float WeightsSize;

		uniform mat4 ProjViewWorldMx;
		uniform mat4 ViewWorldMx;
		uniform mat4 WorldMx;
		uniform mat4 LightSpaceMatrices[MAXLIGHTS];

		uniform mat4 Bones[200];
		
		out vec2 TexCoord;
		out vec3 WorldNormal;
		out vec3 WorldPos;
		out vec4 EyeSpacePosition;
		out vec4 FragPosLightSpaceArray[MAXLIGHTS];
		
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
				Pos=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
				//Normal=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
				Normal=((Bones[BId]*vec4(vNormal,1.0))*Weight).xyz;
			break;
			case 2:
				BId=int(WBuff.y);
				Weight=WBuff.z;

				Pos=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
				//Normal=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
				Normal=((Bones[BId]*vec4(vNormal,1.0))*Weight).xyz;

				BId=int(WBuff.w);

				WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+1.5)),0.0));
				Weight=WBuff.x;
				Pos+=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
				//Normal+=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
				Normal+=((Bones[BId]*vec4(vNormal,1.0))*Weight).xyz;
			break;
			case 3:
				BId=int(WBuff.y);
				Weight=WBuff.z;

				Pos=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
				//Normal=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
				Normal=((Bones[BId]*vec4(vNormal,1.0))*Weight).xyz;

				BId=int(WBuff.w);

				WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+1.5)),0.0));
				Weight=WBuff.x;

				Pos+=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
				//Normal+=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
				Normal+=((Bones[BId]*vec4(vNormal,1.0))*Weight).xyz;

				BId=int(WBuff.y);
				
				Weight=WBuff.z;
				
				Pos+=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
				//Normal+=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
				Normal+=((Bones[BId]*vec4(vNormal,1.0))*Weight).xyz;
			break;
			case 4:
				BId=int(WBuff.y);
				Weight=WBuff.z;

				Pos=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
				//Normal=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
				Normal=((Bones[BId]*vec4(vNormal,1.0))*Weight).xyz;

				BId=int(WBuff.w);

				WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+1.5)),0.0));
				Weight=WBuff.x;

				Pos+=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
				//Normal+=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
				Normal+=((Bones[BId]*vec4(vNormal,1.0))*Weight).xyz;

				BId=int(WBuff.y);
				Weight=WBuff.z;
				Pos+=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
				//Normal+=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
				Normal+=((Bones[BId]*vec4(vNormal,1.0))*Weight).xyz;

				BId=int(WBuff.w);

				WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+2.5)),0.0));

				Weight=WBuff.x;

				Pos+=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
				//Normal+=((vec4(vNormal,1.0)*Bones[BId])*Weight).xyz;
				Normal+=((Bones[BId]*vec4(vNormal,1.0))*Weight).xyz;
			break;
			default:
				Pos=vVertex;
				Normal=vNormal;
			}
			
			//Pos=vVertex;
			//Normal=vNormal;

			WorldPos=vec3(WorldMx * vec4(Pos,1.0));
			WorldNormal=normalize(transpose(inverse(mat3(WorldMx))) * Normal);
			
			TexCoord=vTexCoord;
			
			for(int i=0;i<MAXLIGHTS;i++)
			{
				FragPosLightSpaceArray[i] = LightSpaceMatrices[i] * vec4(WorldPos,1.0);
			}
			
			EyeSpacePosition = ViewWorldMx * vec4(Pos, 1.0);
			
			gl_Position = ProjViewWorldMx * vec4(Pos,1.0);
		}
	}
}

GL_SHADER FShaderVertex FRAGMENT_SHADER
{
	tex : SAMPLER
	
	viewPos:VIEWPOS
	LightPosCone:LIGHTPOSCONE
	LightColorPower:LIGHTCOLORPOWER
	LightDirSpread:LIGHTDIRSPREAD
	ShadowMaps:SHADOWMAPS

	programm
	{
		#version 330 core
	
		#define MAXLIGHTS 3

		uniform sampler2D tex;
		uniform sampler2D ShadowMaps[MAXLIGHTS];
		
		uniform vec3 viewPos;
		uniform vec4 LightPosCone[MAXLIGHTS];
		uniform vec4 LightColorPower[MAXLIGHTS];
		uniform vec4 LightDirSpread[MAXLIGHTS];
		
		in vec2 TexCoord;
		in vec3 WorldNormal;
		in vec3 WorldPos;
		in vec4 FragPosLightSpaceArray[MAXLIGHTS];
		in vec4 EyeSpacePosition;
		
		float getFogFactor(float fogCoordinate)
		{
			float Fog_linearStart=1000.0;
			float Fog_linearEnd=2000.0;
			float Fog_density=0.0035;			
			int Fog_equation=2;
			
			float result = 0.0;
			if(Fog_equation == 0)
			{
				float fogLength = Fog_linearEnd - Fog_linearStart;
				result = (Fog_linearEnd - fogCoordinate) / fogLength;
			}
			else if(Fog_equation == 1) {
				result = exp(-Fog_density * fogCoordinate);
			}
			else if(Fog_equation == 2) {
				result = exp(-pow(Fog_density * fogCoordinate, 2.0));
			}
			
			result = 1.0 - clamp(result, 0.0, 1.0);
			return result;
		}

		float Distance(vec3 vPoint1, vec3 vPoint2)
		{
			float dist = sqrt( (vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
				(vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) +
				(vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z) );

			return dist;
		}
		
		float GetDepth(int li)
		{			
			vec3 projCoords = FragPosLightSpaceArray[li].xyz / FragPosLightSpaceArray[li].w;
			projCoords = projCoords * 0.5 + 0.5;
			
			if(projCoords.x<0.01 || projCoords.y<0.01 || projCoords.x>0.99 || projCoords.y>0.99)
			{			
				return 1.0;
			}
			
			vec3 normal = normalize(WorldNormal);
			vec3 lightDir = normalize(LightPosCone[li].xyz - WorldPos);
			float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
			
			if(bias<0.009)return 1.0;
			
			return texture(ShadowMaps[li], projCoords.xy).r;
		}
		
		float ShadowCalculation(int li)
		{
			vec3 projCoords = FragPosLightSpaceArray[li].xyz / FragPosLightSpaceArray[li].w;
			
			projCoords = projCoords * 0.5 + 0.5;
			
			if(projCoords.x<0.01 || projCoords.y<0.01 || projCoords.x>0.99 || projCoords.y>0.99)
			{			
				return 0.0;
			}
			
			//float closestDepth = texture(ShadowMaps[li], projCoords.xy).r;
			
			float currentDepth = projCoords.z;//+0.1;
			
			vec3 normal = normalize(WorldNormal);
			vec3 lightDir = normalize(LightPosCone[li].xyz - WorldPos);
			float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
			
			//if(bias<0.009)return 0.0;
			
			float shadow = 0.0;
			vec2 texelSize = 1.0 / textureSize(ShadowMaps[li], 0);
			for(int x = -1; x <= 1; ++x)
			{
				for(int y = -1; y <= 1; ++y)
				{
					float pcfDepth = texture(ShadowMaps[li], projCoords.xy + vec2(x, y) * texelSize).r; 
					//shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
					//shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
					//shadow += currentDepth - bias > pcfDepth ? 0.0 : 1.0;
					//shadow += currentDepth - bias;
					shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
				}    
			}
			shadow /= 9.0;
			
			//if(projCoords.z > 0.9)shadow = 0.0;
			
			return shadow;
		}

		void main()
		{
			vec3 color = texture(tex, TexCoord).rgb;
			vec3 normal = normalize(WorldNormal);

			vec3 lighting=vec3(0.0,0.0,0.0);
			float shadow=0.0;
			
			for(int li=0; li < MAXLIGHTS; ++li)
			{
				vec3 lightDir = normalize(LightPosCone[li].xyz - WorldPos);
				float diff = max(dot(lightDir, normal), 0.0);
				vec3 diffuse = diff * LightColorPower[li].rgb;
				//vec3 diffuse = LightColorPower[li].rgb;
				
				vec3 viewDir = normalize(viewPos - WorldPos);
				vec3 reflectDir = reflect(-lightDir, normal);
				float spec = 0.0;
				vec3 halfwayDir = normalize(lightDir + viewDir);
				spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
				vec3 specular = spec * LightColorPower[li].rgb;
				
				/*float shadow_depth=ShadowCalculation(li);
				//float shadow_depth=GetDepth(li);
				if(shadow_depth>0.0)
				{
					shadow=max(shadow,shadow_depth);
				}*/
				shadow += ShadowCalculation(li);
				lighting += (diffuse + specular);
				//lighting += diffuse;
			}
			
			//if(shadow>0.5)shadow=0.5;
			//shadow/=MAXLIGHTS;
			
			vec3 ambient = vec3(0.3,0.3,0.3);
			gl_FragColor = vec4(max(ambient, lighting*(1.0-shadow))*color, 1.0);
			
			float fogCoordinate = abs(EyeSpacePosition.z / EyeSpacePosition.w);
			vec3 Fog_color=vec3(0.5,0.7,0.8);
			gl_FragColor = mix(gl_FragColor, vec4(Fog_color, 1.0), getFogFactor(fogCoordinate));
		}
	}
}

GL_SHADER FShaderVertexNormalmap FRAGMENT_SHADER
{
	tex : SAMPLER
	normal_map : NORMALMAP
	
	viewPos:VIEWPOS
	LightPosCone:LIGHTPOSCONE
	LightColorPower:LIGHTCOLORPOWER
	LightDirSpread:LIGHTDIRSPREAD
	ShadowMaps:SHADOWMAPS

	programm
	{
		#version 330 core
	
		#define MAXLIGHTS 3

		uniform sampler2D tex;
		uniform sampler2D normal_map;
		uniform sampler2D ShadowMaps[MAXLIGHTS];
		
		uniform vec3 viewPos;
		uniform vec4 LightPosCone[MAXLIGHTS];
		uniform vec4 LightColorPower[MAXLIGHTS];
		uniform vec4 LightDirSpread[MAXLIGHTS];
		
		in vec2 TexCoord;
		in vec3 WorldNormal;
		in vec3 WorldBinorm;
		in vec3 WorldTangent;
		in vec3 WorldPos;
		in vec4 EyeSpacePosition;
		in vec4 FragPosLightSpaceArray[MAXLIGHTS];
		
		float getFogFactor(float fogCoordinate)
		{
			float Fog_linearStart=1.0;
			float Fog_linearEnd=5.0;
			float Fog_density=0.0035;			
			int Fog_equation=2;
			
			float result = 0.0;
			if(Fog_equation == 0)
			{
				float fogLength = Fog_linearEnd - Fog_linearStart;
				result = (Fog_linearEnd - fogCoordinate) / fogLength;
			}
			else if(Fog_equation == 1) {
				result = exp(-Fog_density * fogCoordinate);
			}
			else if(Fog_equation == 2) {
				result = exp(-pow(Fog_density * fogCoordinate, 2.0));
			}
			
			result = 1.0 - clamp(result, 0.0, 1.0);
			return result;
		}

		float Distance(vec3 vPoint1, vec3 vPoint2)
		{
			float dist = sqrt( (vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
				(vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) +
				(vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z) );

			return dist;
		}
		
		float GetDepth(int li)
		{			
			vec3 projCoords = FragPosLightSpaceArray[li].xyz / FragPosLightSpaceArray[li].w;
			projCoords = projCoords * 0.5 + 0.5;
			
			float closestDepth = texture(ShadowMaps[li], projCoords.xy).r;
			float currentDepth = projCoords.z;
	
			if(currentDepth > closestDepth || projCoords.x<0.01 || projCoords.y<0.01 || projCoords.x>0.99 || projCoords.y>0.99)
			{			
				return -1.0;
			}
			
			return closestDepth;
		}

		void main()
		{
			vec4 color = texture2D(tex, TexCoord);
			vec3 bumps=texture2D(normal_map,TexCoord).xyz * 2.0 - 1.0;
			
			vec3 Nb = normalize(WorldNormal + (bumps.x * WorldTangent + bumps.y * WorldBinorm));

			vec3 lighting=vec3(0.0,0.0,0.0);
			float shadow = 1.0;
			
			vec3 test_color=vec3(0.0,0.0,0.0);
			
			for(int li=0; li < MAXLIGHTS; ++li)
			{
				vec3 lightDir = normalize(LightPosCone[li].xyz - WorldPos);
				float diff = max(dot(lightDir, WorldNormal), 0.0);
				vec3 diffuse = /*diff **/ LightColorPower[li].rgb;
				
				vec3 viewDir = normalize(viewPos - WorldPos);
				vec3 reflectDir = reflect(-lightDir, WorldNormal);
				float spec = 0.0;
				vec3 halfwayDir = normalize(lightDir + viewDir);  
				spec = pow(max(dot(WorldNormal, halfwayDir), 0.0), 64.0);
				vec3 specular = spec * LightColorPower[li].rgb;
				
				float spot = 1.0;
				if(LightPosCone[li].w!=-1.0)
				{
					float d=dot(
						-lightDir,
						normalize(LightDirSpread[li].xyz)
						);

					float spread=LightDirSpread[li].w;

					spot = clamp(
						pow(
								clamp(d/cos(LightPosCone[li].w),0.0,1.0),
								spread
							),
						0.0,1.0
					);
				}
				
				float ldn = dot(Nb,lightDir);
				
				float sdn = pow( clamp( dot(reflect(Nb, lightDir), lightDir ) ,0.0, 1.0 ), Distance(WorldPos,LightPosCone[li].xyz) );
				
				lighting+=LightColorPower[li].rgb*(dot(WorldNormal,lightDir)+ldn+sdn+specular)*LightColorPower[li].w*spot;
				
				//shadow = max(shadow,ShadowCalculation(li));
				//shadow+=ShadowCalculation(li);
				//if(dot(WorldNormal,lightDir)<0.0)
				/*{
					float shadow_depth=GetDepth(li);
					if(shadow_depth>0.5 && shadow_depth<0.52){
						shadow=0.5;
					}
				}*/
				/*float shadow_depth=GetDepth(li);
				if(shadow_depth>0.0 && shadow_depth<1.0)
				{
					shadow=max(shadow,GetDepth(li));
				}*/
				
				float shadow_depth=GetDepth(li);
				if(shadow_depth>0.0 && shadow_depth<0.99)
				{
					shadow=min(shadow,shadow_depth);
				}
			}
			
			/*vec3 ambient = vec3(0.3,0.3,0.3);
			gl_FragColor = vec4((max(ambient, lighting*shadow)*color.rgb), color.a);
			
			float fogCoordinate = abs(EyeSpacePosition.z / EyeSpacePosition.w);
			vec3 Fog_color=vec3(0.5,0.7,0.8);
			gl_FragColor = mix(gl_FragColor, vec4(Fog_color, 1.0), getFogFactor(fogCoordinate));*/
			
			gl_FragColor = vec4(shadow,shadow,shadow,1.0);
		}
	}
}

GL_SHADER VShaderShadowDepth VERTEX_SHADER
{
	WorldMx : WORLDMX

	vVertex:VERTEX
	
	LightSpaceMatrix:LIGHTSPECMX

	programm
	{
		//#version 130
		#version 330 core

		in vec3 vVertex;

		uniform mat4 WorldMx;
		uniform mat4 LightSpaceMatrix;
		
		void main()
		{			
			gl_Position = LightSpaceMatrix * WorldMx * vec4(vVertex,1.0);
			//gl_Position = vec4(0.0,0.0,1.0,1.0);
		}
	}
}

GL_SHADER VShaderShadowDepthWT VERTEX_SHADER
{
	WorldMx:WORLDMX
	vVertex:VERTEX
	Wofs:WEIGHTOFS;

	WeightsTex : WEIGHTTEX
	
	WeightsSize: WEIGHTSIZE

	Bones:BONES;
	
	LightSpaceMatrix:LIGHTSPECMX

	programm
	{
		//#version 130
		#version 330 core

		in vec3 vVertex;
		in float Wofs;

		uniform sampler2D WeightsTex;
		uniform float WeightsSize;

		uniform mat4 LightSpaceMatrix;
		uniform mat4 WorldMx;

		uniform mat4 Bones[200];
		
		void main()
		{
			vec4 WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+0.5)),0.0));//ofsA*(WeightsSize/1000.0)
			
			int Bn=int(WBuff.x);
			
			vec3 Pos=vec3(0,0,0);

			int BId=0;
			float Weight=0.0;
			
			switch(Bn)
			{
			case 0:
				Pos=vVertex;
			break;
			case 1:
				BId=int(WBuff.y);
				Weight=WBuff.z;
				Pos=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
			break;
			case 2:
				BId=int(WBuff.y);
				Weight=WBuff.z;

				Pos=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;

				BId=int(WBuff.w);

				WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+1.5)),0.0));
				Weight=WBuff.x;
				Pos+=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
			break;
			case 3:
				BId=int(WBuff.y);
				Weight=WBuff.z;

				Pos=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;

				BId=int(WBuff.w);

				WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+1.5)),0.0));
				Weight=WBuff.x;

				Pos+=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;

				BId=int(WBuff.y);
				
				Weight=WBuff.z;
				
				Pos+=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
			break;
			case 4:
				BId=int(WBuff.y);
				Weight=WBuff.z;

				Pos=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;

				BId=int(WBuff.w);

				WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+1.5)),0.0));
				Weight=WBuff.x;

				Pos+=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;

				BId=int(WBuff.y);
				Weight=WBuff.z;
				Pos+=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;

				BId=int(WBuff.w);

				WBuff=texture2D(WeightsTex, vec2(1.0/(WeightsSize/(Wofs+2.5)),0.0));

				Weight=WBuff.x;

				Pos+=((Bones[BId]*vec4(vVertex,1.0))*Weight).xyz;
			break;
			default:
				Pos=vVertex;
			}
			
			gl_Position = LightSpaceMatrix*WorldMx*vec4(Pos,1.0);
		}
	}
}

GL_SHADER FEmptyShader FRAGMENT_SHADER
{
	programm
	{
		void main()
		{
			/*if(gl_FragCoord.z>0.0){
				gl_FragCoord.z-=0.005;
			}
			
			gl_FragDepth = gl_FragCoord.z;*/
			//gl_FragColor=vec4(0.0,1.0,0.0,1.0);
		}
	}
}

GL_SHADER VShaderViewDepthBuffer VERTEX_SHADER
{
	depth_shader_model_matrix : WORLDMX

	aPos:VERTEX
	aTexCoords:TEXCOORD0

	programm
	{
		#version 330 core
		in vec3 aPos;
		in vec2 aTexCoords;

		uniform mat4 depth_shader_model_matrix;

		out vec2 TexCoord;

		void main()
		{
			TexCoord = aTexCoords;
			gl_Position = depth_shader_model_matrix * vec4(aPos.x,aPos.y,0.01, 1.0);
		}
	}
}

GL_SHADER FShaderViewDepthBuffer FRAGMENT_SHADER
{
	depthMap:SAMPLER
	
	programm
	{
		#version 330 core
		out vec4 FragColor;

		in vec2 TexCoord;

		uniform sampler2D depthMap;
		//uniform float near_plane;
		//uniform float far_plane;

		// required when using a perspective projection matrix
		/*float LinearizeDepth(float depth)
		{
			float z = depth * 2.0 - 1.0; // Back to NDC 
			return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
		}*/

		void main()
		{
			float depthValue = texture(depthMap, TexCoord).r;
			//FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
			
			/*if(depthValue==0.0){
				FragColor = vec4(1.0, 0.0, 0.0, 1.0); // orthographic
			}else{
				FragColor = vec4(vec3(depthValue), 1.0); // orthographic
			}*/
			
			FragColor = vec4(depthValue,depthValue,depthValue, 1.0);
		}
	}
}

GL_SHADER VSSkyBox VERTEX_SHADER
{
	ProjViewWorldMx:PROJVIEWWORLDMX
	
	vVertex:VERTEX
	vNormal:NORMAL
	vTexCoord:TEXCOORD0

	programm
	{
		#version 330 core
		
		#define MAXLIGHTS 3

		in vec3 vVertex;
		in vec3 vNormal;
		in vec2 vTexCoord;

		uniform mat4 ProjViewWorldMx;
		
		out vec2 TexCoord;

		void main()
		{
			TexCoord=vTexCoord;			
			gl_Position = ProjViewWorldMx*vec4(vVertex,1.0);
		}
	}
}

GL_SHADER FSSkyBox FRAGMENT_SHADER
{
	tex : SAMPLER
	
	programm
	{
		#version 330 core
	
		#define MAXLIGHTS 3

		uniform sampler2D tex;
		
		in vec2 TexCoord;
		
		void main()
		{
			vec4 color = texture(tex, TexCoord);
			gl_FragColor = vec4(color.rgb, 1.0);
		}
	}
}

GL_SHADER VShadeText VERTEX_SHADER
{
	ProjViewWorldMx:PROJVIEWWORLDMX
	vVertex:VERTEX
	vVertInf:NORMAL
	vTexCoord:TEXCOORD0
	TexCoords:MESH_REF_VEC4_A
	FontInfo:WEIGHTS

	programm
	{
		#version 130

		in vec3 vVertex;
		in vec3 vVertInf;
		in vec2 vTexCoord;
		uniform mat4 ProjViewWorldMx;
		uniform vec4 TexCoords[512];
		uniform vec4 FontInfo;
		varying vec2 TexCoord;
		varying vec3 TexColor;
		void main()
		{
			TexCoord=vec2(0.0,0.0);
			
			int ofs=int(vVertInf.x*2.0);
			
			switch(int(vVertInf.y))
			{
			case 0:
				TexCoord=TexCoords[ofs].xy;
			break;
			case 1:
				TexCoord=TexCoords[ofs].zw;
			break;
			case 2:
				TexCoord=TexCoords[ofs+1].xy;
			break;
			case 3:
				TexCoord=TexCoords[ofs+1].zw;
			break;
			}
			
			TexColor=vec3(FontInfo.y,FontInfo.z,FontInfo.w);
			
			gl_Position = ProjViewWorldMx*vec4((vVertex.x*FontInfo.x),vVertex.y,0.0,1.0);
		}
	}
}

GL_SHADER FShadeText FRAGMENT_SHADER
{
	FontTex : SAMPLER
	programm
	{
		varying vec2 TexCoord;
		varying vec3 TexColor;
		uniform sampler2D FontTex;
		void main()
		{
			vec4 color=texture2D(FontTex,vec2(TexCoord.x,TexCoord.y));
			gl_FragColor=vec4(color.rgb*TexColor,color.a);
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

technique tech_vertex_shadows_depth
{	
	pass p0
	{
		VertexShader = compile VShaderShadowDepth();
		FragnentShader = compile FEmptyShader();
	}
}

technique tech_skinnwt
{	
	pass p0
	{
		VertexShader = compile VShadeSkinnedWT();
		FragnentShader = compile FShaderVertex();
	}
}

technique tech_skinnwt_shadows_depth
{
	pass p0
	{
		VertexShader = compile VShaderShadowDepthWT();
		FragnentShader = compile FEmptyShader();
	}
}

technique tech_vertex_normalmap
{
	pass p0
	{
		VertexShader = compile VShaderVertexNormalmap();
		FragnentShader = compile FShaderVertexNormalmap();
	}
}

technique tech_view_depth_buffer
{
	pass p0
	{
		VertexShader = compile VShaderViewDepthBuffer();
		FragnentShader = compile FShaderViewDepthBuffer();
	}
}

technique tech_sky_box
{
	pass p0
	{
		VertexShader = compile VSSkyBox();
		FragnentShader = compile FSSkyBox();
	}
}

technique tech_text
{
	pass p0
	{
		VertexShader = compile VShadeText();
		FragnentShader = compile FShadeText();
	}
}

