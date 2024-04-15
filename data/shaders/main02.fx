GL_SHADER VShaderVertex VERTEX_SHADER
{
	ProjViewWorldMx:PROJVIEWWORLDMX
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
		uniform mat4 ProjViewWorldMx;
		uniform mat4 WorldMx;
		uniform mat4 LightSpaceMatrices[MAXLIGHTS];
		
		out vec3 WorldPos;
		out vec3 WorldNormal;
		out vec2 TexCoord;
		out vec4 FragPosLightSpaceArray[MAXLIGHTS];

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
			
			gl_Position = ProjViewWorldMx*vec4(vVertex,1.0);
		}
	}
}

GL_SHADER VShaderVertexNormalmap VERTEX_SHADER
{
	ProjViewWorldMx:PROJVIEWWORLDMX
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
		uniform mat4 ProjViewWorldMx;
		uniform mat4 WorldMx;
		uniform mat4 LightSpaceMatrices[MAXLIGHTS];
		
		out vec3 WorldPos;
		out vec3 WorldNormal;
		out vec3 WorldBinorm;
		out vec3 WorldTangent;
		out vec2 TexCoord;
		out vec4 FragPosLightSpaceArray[MAXLIGHTS];

		void main()
		{			
			WorldPos=(WorldMx*vec4(vVertex,1.0)).xyz;
			//WorldNormal=normalize((WorldMx*vec4(vNormal,1.0)).xyz);
			//WorldNormal=mat3(WorldMx) * vNormal;
			//WorldNormal=vNormal;
			WorldNormal=normalize(transpose(inverse(mat3(WorldMx))) * vNormal);
			WorldBinorm=normalize(transpose(inverse(mat3(WorldMx))) * vBinormal);
			WorldTangent=normalize(transpose(inverse(mat3(WorldMx))) * vTangent);
			TexCoord=vTexCoord;
			
			for(int i=0;i<MAXLIGHTS;i++)
			{
				FragPosLightSpaceArray[i]=LightSpaceMatrices[i] * vec4(WorldPos,1.0);
			}
			
			gl_Position = ProjViewWorldMx*vec4(vVertex,1.0);
		}
	}
}

GL_SHADER VShadeSkinnedWT VERTEX_SHADER
{
	ProjViewWorldMx:PROJVIEWWORLDMX
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
		uniform mat4 WorldMx;
		uniform mat4 LightSpaceMatrices[MAXLIGHTS];

		uniform mat4 Bones[200];
		
		out vec2 TexCoord;
		out vec3 WorldNormal;
		out vec3 WorldPos;
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

			WorldPos=vec3(WorldMx * vec4(Pos,1.0));
			WorldNormal=normalize(transpose(inverse(mat3(WorldMx))) * Normal);
			
			TexCoord=vTexCoord;
			
			for(int i=0;i<MAXLIGHTS;i++)
			{
				FragPosLightSpaceArray[i] = LightSpaceMatrices[i] * vec4(WorldPos,1.0);
			}
			
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

		float Distance(vec3 vPoint1, vec3 vPoint2)
		{
			float dist = sqrt( (vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
				(vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) +
				(vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z) );

			return dist;
		}
				
		float ShadowCalculation(int li)
		{
			/*vec3 projCoords = FragPosLightSpaceArray[li].xyz / FragPosLightSpaceArray[li].w;
		
			projCoords = projCoords * 0.5 + 0.5;
			
			float closestDepth = texture(ShadowMaps[li], projCoords.xy).r;
			
			float currentDepth = projCoords.z;
			
			vec3 lightDir = normalize(LightPosCone[li].xyz - WorldPos);
			float bias = max(0.05 * (1.0 - dot(WorldNormal, lightDir)), 0.005);
			
			float shadow = 0.0;
			vec2 texelSize = 1.0 / textureSize(ShadowMaps[li], 0);
			for(int x = -1; x <= 1; ++x)
			{
				for(int y = -1; y <= 1; ++y)
				{
					float pcfDepth = texture(ShadowMaps[li], projCoords.xy + vec2(x, y) * texelSize).r; 
					shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
				}    
			}
			shadow /= 9.0;
			
			if(projCoords.z > 0.9)shadow = 0.0;*/
			
			vec3 projCoords = FragPosLightSpaceArray[li].xyz / FragPosLightSpaceArray[li].w;
			// transform to [0,1] range
			projCoords = projCoords * 0.5 + 0.5;
			// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
			float closestDepth = texture(ShadowMaps[li], projCoords.xy).r; 
			// get depth of current fragment from light's perspective
			float currentDepth = projCoords.z;
			// check whether current frag pos is in shadow
			float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
				
			return shadow;
		}

		void main()
		{
			vec4 color = texture(tex, TexCoord);

			vec3 lighting=vec3(0.0,0.0,0.0);
			float shadow = 0.0;
			
			vec3 shadow_map=vec3(0.0,0.0,0.0);
			
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
				
				shadow = max(shadow,ShadowCalculation(li));
				
				//lighting += (diffuse + specular)*spot;
				//lighting += diffuse*spot;
				lighting+=LightColorPower[li].rgb*(dot(WorldNormal,lightDir)+specular)*LightColorPower[li].w*spot;
				
				/*vec3 projCoords = FragPosLightSpaceArray[li].xyz / FragPosLightSpaceArray[li].w;
				projCoords = projCoords * 0.5 + 0.5;
				shadow+=texture(ShadowMaps[li], projCoords.xy).r;*/
			}
			
			if(shadow>0.5)shadow=0.5;
			
			vec3 ambient = vec3(0.3,0.3,0.3);
			gl_FragColor = vec4((max(ambient, lighting*(1.0-shadow))*color.rgb), color.a);
			//gl_FragColor = vec4(1.0,0.0,0.0, 1.0);
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
		in vec4 FragPosLightSpaceArray[MAXLIGHTS];

		float Distance(vec3 vPoint1, vec3 vPoint2)
		{
			float dist = sqrt( (vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
				(vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) +
				(vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z) );

			return dist;
		}
				
		float ShadowCalculation(int li)
		{			
			vec3 projCoords = FragPosLightSpaceArray[li].xyz / FragPosLightSpaceArray[li].w;
			projCoords = projCoords * 0.5 + 0.5;
			float closestDepth = texture(ShadowMaps[li], projCoords.xy).r;
			float currentDepth = projCoords.z;
			float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
				
			return shadow;
		}

		void main()
		{
			vec4 color = texture2D(tex, TexCoord);
			vec3 bumps=texture2D(normal_map,TexCoord).xyz * 2.0 - 1.0;
			
			vec3 Nb = normalize(WorldNormal + (bumps.x * WorldTangent + bumps.y * WorldBinorm));

			vec3 lighting=vec3(0.0,0.0,0.0);
			float shadow = 0.0;
			
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
				
				shadow = max(shadow,ShadowCalculation(li));
			}
			
			if(shadow>0.5)shadow=0.5;
			
			vec3 ambient = vec3(0.3,0.3,0.3);
			gl_FragColor = vec4((max(ambient, lighting*(1.0-shadow))*color.rgb), color.a);
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
			gl_FragDepth = gl_FragCoord.z;
			//gl_FragColor=vec4(gl_FragCoord.z,0.0,0.0,1.0);
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

