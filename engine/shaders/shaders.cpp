
#include "shaders.h"
#include <engine/common/gl_head.h>
#include <engine/common/exception.h>
#include <engine/mesh/mesh.h>

#include <stdio.h>
#include <string.h>
#include <typeinfo>

#include <glm/gtc/matrix_transform.hpp>

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	GLenum ret = false;
	while((errorCode = glGetError()) != GL_NO_ERROR)
	{
		const char* error="";
		switch(errorCode)
		{
			case GL_INVALID_ENUM:                  error = "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "GL_INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "GL_INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                error = "GL_STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               error = "GL_STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "GL_OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		printf("file:%s line:%d\n%s\n", file, line, error);

		//throw exception_box(__FUNCTION__, __FILE__, __LINE__,"%s" ,error);

		ret = true;
	}
	return ret;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

bool c_function::ValuesUpdate(_vert_def_cont* vdc)
{
	int TexCount=0;
	//printf("c_function::ValuesUpdate(%s) ValueNum:%d\n", Name, ValueNum);
	for(int i=0;i<ValueNum;i++)
	{
		c_shader_value* value=ppSaderValues[i];

		if(ShaderSemantics::values[(int)value->Semantic].ValLevel==_VALUE_LEVEL_NON)
		{
			glEnableVertexAttribArray(pValueLocation[i]);
			glVertexAttribPointer(
				pValueLocation[i],
				ShaderSemantics::vert_def[(int)value->Semantic].part_count,
				GL_FLOAT,
				GL_FALSE,
				vdc->struct_size,
				(GLvoid*)vdc->item_offsets[(int)value->Semantic]
			);

			glCheckError();
			//printf("Attribute:%d part_count:%d struct_size:%d item_offsets:%d\n", value->Semantic, ShaderSemantics::vert_def[(int)value->Semantic].part_count, vdc->struct_size, (GLvoid*)vdc->item_offsets[(int)value->Semantic]);
			
		}else{
			
			switch(ShaderSemantics::values[(int)value->Semantic].ValType)
			{
			case VALTYPE_MATRIX4X4:
				//printf("VALTYPE_MATRIX4X4:%d id:%d %s sem:%s ptr:%p count:%d\n", value->Semantic, i, value->Name, ShaderSemantics::values[(int)value->Semantic].name, value->ShaderValuePtr, value->CountSet);
				glUniformMatrix4fv(pValueLocation[i], value->CountSet, GL_FALSE, (float*)value->ShaderValuePtr);
				glCheckError();
				break;
			case VALTYPE_VEC4:
				//printf("VALTYPE_VEC4:%d id:%d %s sem:%s ptr:%p count:%d\n", value->Semantic, i, value->Name, ShaderSemantics::values[(int)value->Semantic].name, value->ShaderValuePtr, value->CountSet);
				glUniform4fv(pValueLocation[i],value->CountSet, (float*)value->ShaderValuePtr);
				glCheckError();
			break;
			case VALTYPE_REF_VEC4:
				/*if(value->Semantic == _VALUESEMANTIC_SPEC::VALSEM_MESH_REF_VEC4_A){
					printf("VALTYPE_VEC4:%d id:%d %s sem:%s ptr:%p pptr:%p count:%d\n", value->Semantic, i, value->Name, ShaderSemantics::values[(int)value->Semantic].name, value->ShaderValuePtr, *(float**)value->ShaderValuePtr, value->CountSet);
				}*/
				glUniform4fv(pValueLocation[i], value->CountSet, *(float**)value->ShaderValuePtr);
				glCheckError();
				break;
			case VALTYPE_VEC3:
				//printf("VALTYPE_VEC3:%d id:%d %s sem:%s ptr:%p count:%d\n", value->Semantic, i, value->Name, ShaderSemantics::values[(int)value->Semantic].name, value->ShaderValuePtr, value->CountSet);
				glUniform3fv(pValueLocation[i],value->CountSet, (float*)value->ShaderValuePtr);
				glCheckError();
			break;
			case VALTYPE_VEC2:
				//printf("VALTYPE_VEC2:%d id:%d %s sem:%s ptr:%p count:%d\n", value->Semantic, i, value->Name, ShaderSemantics::values[(int)value->Semantic].name, value->ShaderValuePtr, value->CountSet);
				glUniform2fv(pValueLocation[i],value->CountSet, (float*)value->ShaderValuePtr);
				glCheckError();
			break;
			case VALTYPE_SAMPLER:
			{
				if(value->CountSet < 2)
				{

					glActiveTexture(GL_TEXTURE0 + TexCount);

					if(value->ShaderValuePtr){
						//printf("VALTYPE_SAMPLER: id:%d %s ptr:%p val:%d count:%d location:%d\n", i, value->Name, value->ShaderValuePtr, *(int*)value->ShaderValuePtr, value->CountSet, pValueLocation[i]);
						glBindTexture(GL_TEXTURE_2D, *(int*)value->ShaderValuePtr);
					}
					else{
						//printf("VALTYPE_SAMPLER: id:%d %s val:\'0\' count:%d location:%d\n", i, value->Name, value->CountSet, pValueLocation[i]);
						glBindTexture(GL_TEXTURE_2D, 0);
					}

					glUniform1i(pValueLocation[i], TexCount);
					++TexCount;
					if(glCheckError())
					{
						printf("VALTYPE_SAMPLER: id:%d %s val:%d count:%d location:%d\n", i, value->Name, *(int*)value->ShaderValuePtr, value->CountSet, pValueLocation[i]);
					}
				}
				else{
					//printf("VALTYPE_SAMPLER: id:%d %s ptr:%x count:%d\n", i, value->Name, value->ShaderValuePtr, value->CountSet);
					for(int ti = 0; ti < value->CountSet; ++ti)
					{
						glActiveTexture(GL_TEXTURE0 + ((int*)value->ShaderValuePtr)[ti]);
						glBindTexture(GL_TEXTURE_2D, ((int*)value->ShaderValuePtr)[ti]);
					}

					glUniform1iv(pValueLocation[i], value->CountSet, (int*)value->ShaderValuePtr);
					glCheckError();
				}
			}
			break;
			case VALTYPE_TEXTURE:
				/*printf("VALTYPE_TEXTURE:%d\n", value->Semantic);
				if(value->ShaderValuePtr)
				{
					cTexture* ptexture=(cTexture*)value->ShaderValuePtr;

					if(value->CountSet == 0){
						glActiveTexture(GL_TEXTURE0 + TexCount);
						glBindTexture(GL_TEXTURE_2D, ptexture->textureID);
						glTexParameteri	( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ptexture->WrapW );
						glTexParameteri	( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ptexture->WrapH );
						glUniform1i(pValueLocation[i], TexCount++);
					}
					else{
						printf("SAMPLERS:%d\n", value->CountSet);
						int tex_ids[24];
						for(int ti = 0; ti < value->CountSet; ++ti)
						{
							tex_ids[ti] = ti + 1;
							printf("\t%d %d\n", tex_ids[ti], ptexture[ti].textureID);

							int at = GL_TEXTURE1 + TexCount;
							glActiveTexture(at);

							switch(at)
							{
								case GL_TEXTURE0:
									printf("\tGL_TEXTURE0\n");
									break;
								case GL_TEXTURE1:
									printf("\tGL_TEXTURE1\n");
									break;
								case GL_TEXTURE2:
									printf("\tGL_TEXTURE2\n");
									break;
								case GL_TEXTURE3:
									printf("\tGL_TEXTURE3\n");
									break;
							}

							glBindTexture(GL_TEXTURE_2D, ptexture[ti].textureID);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ptexture->WrapW);
							glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ptexture->WrapH);

							TexCount++;
						}

						glUniform1iv(pValueLocation[i], value->CountSet, tex_ids);
					}
				}else{
					//printf("ERROR: empty texture\n");
					glActiveTexture(GL_TEXTURE0 + TexCount);
					glBindTexture(GL_TEXTURE_2D, 0);
					glUniform1i(pValueLocation[i], TexCount++);
				}*/

				break;
			case VALTYPE_FLOAT:
				//float tf;
				//memcpy(&tf, &value->ShaderValuePtr, sizeof(float));
				
				if(value->ShaderValuePtr){
					//printf("VALTYPE_FLOAT: id:%d %s ref:%p val:%f count:%d\n", i, value->Name, value->ShaderValuePtr, *(float*)value->ShaderValuePtr, value->CountSet);
					glUniform1f(pValueLocation[i], *(float*)value->ShaderValuePtr);
				}
				else{
					//printf("VALTYPE_FLOAT: id:%d %s ref:\'0\' count:%d\n", i, value->Name, value->CountSet);
					glUniform1f(pValueLocation[i], 0);
				}

				glCheckError();

				break;
			default:printf("b %s sem:%d type:%d level:%d reg:%d\n",value->Name,value->Semantic, ShaderSemantics::values[(int)value->Semantic].ValType, ShaderSemantics::values[(int)value->Semantic].ValLevel,value->RegisterType);
				break;
			}
		}
	}

	return 0;
}

bool c_pass::Begin(_vert_def_cont* vdc)
{
	if(Func){
		Func->ValuesUpdate(vdc);
	}

//gRenderTarget
	//if(RenderTarget)printf("%s\n",RenderTarget->Name);
	return true;
}

bool shaders_container::Begin(cMesh* pmesh, int tech_id)
{
	//printf("shaders_container::Begin() %s\n",Name);
	glBindBuffer(GL_ARRAY_BUFFER, pmesh->IGLBidV);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pmesh->IGLBidI);
	//printf("\tGL_ARRAY_BUFFER:%d\n", pmesh->IGLBidV);
	//printf("\tGL_ELEMENT_ARRAY_BUFFER:%d\n", pmesh->IGLBidI);

	//printf("tech_id:%d %s\n", tech_id, Tech[tech_id].Name);

	for(int i = 0; i < Tech[tech_id].PassNum; ++i)
	{
		//printf("	Pass:%s\n", Tech[tech_id].Pass[i].Name);
		if(Tech[tech_id].Pass[i].Func)
		{
			//printf("%s\n",Pass[i].Func->Name);

			glValidateProgram(Tech[tech_id].Pass[i].Func->programm);
			glCheckError();

			glUseProgram(Tech[tech_id].Pass[i].Func->programm);
			if(glCheckError() != GL_NO_ERROR){
				printf("programm:%s\n", Tech[tech_id].Pass[i].Func->Name);
			}

			Tech[tech_id].Pass[i].Begin(&pmesh->vert_def);
			glCheckError();

			glDrawElements(GL_TRIANGLES, pmesh->inum, GL_UNSIGNED_SHORT, 0);
			glCheckError();

			/*if(SwitchRT)
			{
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			}*/

			glUseProgram(0);
		}
	}

	return true;// Tech[tech_id].Begin(pmesh);
}

int shaders_container::RegisterValueLevel_Ex(cList<_shader_values_level_item*>* plevelvalues,char* name,int id,_VALUESEMANTIC_SPEC sem)
{
	_shader_values_level_item* new_item = new _shader_values_level_item();

	int len=strlen(name);
	new_item->name=new char[len+1];
	memcpy(new_item->name,name,len);
	new_item->name[len]=0;

	new_item->id=id;
	new_item->sem=sem;

	plevelvalues->push(new_item);

	return 0;
}

int shaders_container::RegisterValueLevel(int level,char* name,int id,_VALUESEMANTIC_SPEC sem)
{
	switch(level)
	{
	case 0:
		return RegisterValueLevel_Ex(&valuesL_engine,name,id,sem);
	case 1:
		return RegisterValueLevel_Ex(&valuesL_model,name,id,sem);
	case 2:
		return RegisterValueLevel_Ex(&valuesL_mesh,name,id,sem);
	}
	return 0;
}

int shaders_container::GetValueByName(char* name)
{
	for(uint i = 0; i < ValueNum; ++i)
	{
		if(!strcmp(Values[i].Name, name))return i;
	}

	return -1;
}

int shaders_container::GetValueByName(int tech,int pass,char* name)
{
	c_function* pfunc=Tech[tech].Pass[pass].Func;

	if(pfunc)
	{
		for(int i = 0; i < pfunc->ValueNum; ++i)
		{
			if(!strcmp(pfunc->ppSaderValues[i]->Name, name))return pfunc->ppSaderValues[i]->GId;
		}
	}
	return -1;
}

int shaders_container::GetValueBySemantic(_VALUESEMANTIC_SPEC valsem)
{
	for(uint i = 0; i < ValueNum; ++i)
	{
		if(Values[i].Semantic == valsem)return i;
	}

	return -1;
}

int shaders_container::GetTechniqueByName(char* name)
{
	for(uint i = 0; i < TechNum; i++)
	{
		if(!strcmp(Tech[i].Name, name))return i;
	}

	return -1;
}

int shaders_container::GetPassByName(int tech,char* name)
{
	for(int i=0;i<Tech[tech].PassNum;i++)
	{
		if(!strcmp(Tech[tech].Pass[i].Name,name))return i;
	}

	return -1;
}

bool shaders_container::SetMatrix(uint id, mat4* pmx)
{
	Values[id].ShaderValuePtr = (float*)pmx;
	Values[id].CountSet = 1;
	return true;
}
/*
bool shaders_container::SetWeightsBuffer(uint id,int iw)
{
	Values[id].Value=(float*)iw;
	return true;
}
*/
bool shaders_container::SetVertexAttribPointer(uint id)
{
	return true;
}

bool shaders_container::SetFloat(uint id,float f)
{
	//Values[id].Value=(float*)(double)f;
	memcpy(&Values[id].ShaderValuePtr,&f,sizeof(float));
	/*
	float tf=f;
	uint dw=0;

	memcpy(&dw,&tf,4);
	printf("%f",f);
	printf(" [%x]",dw);

	memcpy(&dw,&Values[id].Value,4);
	printf(" [%x]",dw);
	printf("\n");*/

	return true;
}

bool shaders_container::SetVector3(uint id,float x,float y,float z)
{
	float *pf=(float*)Values[id].ShaderValuePtr;
	pf[0]=x;
	pf[1]=y;
	pf[2]=z;

	Values[id].CountSet=1;
	return true;
}

bool shaders_container::SetInt(uint id,int i)
{
	Values[id].ShaderValuePtr =(float*)i;
	Values[id].CountSet=1;

	return true;
}

bool shaders_container::SetVector(uint id,vec4* pvec)
{
	Values[id].ShaderValuePtr =(float*)pvec;
	Values[id].CountSet=1;

	return true;
}

bool shaders_container::SetVector(uint id,vec3* pvec)
{
	Values[id].ShaderValuePtr =(float*)pvec;
	Values[id].CountSet=1;

	return true;
}

bool shaders_container::SetVector(uint id,vec2* pvec)
{
	Values[id].ShaderValuePtr =(float*)pvec;
	Values[id].CountSet=1;

	return true;
}

bool shaders_container::SetTexture(uint id,cTexture* ptexture)
{
	Values[id].ShaderValuePtr =(float*)ptexture;
	Values[id].CountSet = 0;
	return true;
}

bool shaders_container::SetSampler(uint id, uint texid)
{
	Values[id].ShaderValuePtr = (float*)texid;
	Values[id].CountSet = 1;
	return true;
}

bool shaders_container::SetVectorArray(uint id,vec4* pvec,uint count)
{
	Values[id].ShaderValuePtr =(float*)pvec;
	Values[id].CountSet=count;
	return true;
}

bool shaders_container::SetVectorArray(uint id,vec3* pvec,uint count)
{
	Values[id].ShaderValuePtr =(float*)pvec;
	Values[id].CountSet=count;
	return true;
}

bool shaders_container::SetVectorArray(uint id,vec2* pvec,uint count)
{
	Values[id].ShaderValuePtr =(float*)pvec;
	Values[id].CountSet=count;
	return true;
}

bool shaders_container::SetMatrixArray(uint id, mat4* pmx, uint count)
{
	Values[id].ShaderValuePtr = (float*)pmx;
	Values[id].CountSet = count;
	return true;
}

bool shaders_container::SetFloatArray(uint id, float* pf, uint count)
{
	Values[id].ShaderValuePtr = (float*)pf;
	Values[id].CountSet = count;
	return true;
}

bool shaders_container::SetTextureArray(uint id, cTexture* ptexture, uint count)
{
	Values[id].ShaderValuePtr = (float*)ptexture;
	Values[id].CountSet = count;
	return true;
}

bool shaders_container::SetSamplerArray(uint id, uint* psamplers, uint count)
{
	Values[id].ShaderValuePtr = (float*)psamplers;
	Values[id].CountSet = count;
	return true;
}