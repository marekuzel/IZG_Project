/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <stdio.h>
void clear(GPUMemory&mem,ClearCommand cmd){
  
  auto & frame = mem.framebuffer;
    auto nOfPixels = frame.height*frame.width;
    for (int i = 0; i < nOfPixels; ++i){
      if(cmd.clearColor && !(cmd.clearDepth)){  
        mem.framebuffer.color[i*4] = (uint8_t) (cmd.color.r*255.f);
        mem.framebuffer.color[i*4+1] = (uint8_t) (cmd.color.g*255.f);
        mem.framebuffer.color[i*4+2] = (uint8_t) (cmd.color.b*255.f);
        mem.framebuffer.color[i*4+3] = (uint8_t) (cmd.color.a*255.f);
      }
      else if (cmd.clearColor && cmd.clearDepth){
        mem.framebuffer.color[i*4] = (uint8_t) (cmd.color.r*255.f);
        mem.framebuffer.color[i*4+1] = (uint8_t) (cmd.color.g*255.f);
        mem.framebuffer.color[i*4+2] = (uint8_t) (cmd.color.b*255.f);
        mem.framebuffer.color[i*4+3] = (uint8_t) (cmd.color.a*255.f);
        mem.framebuffer.depth[i] = cmd.depth;
      }
      else if (cmd.clearDepth){
        mem.framebuffer.depth[i] = cmd.depth;
      }
  }
}

InVertex runVertexAssembly(InVertex inVertex,VertexArray vao, GPUMemory mem){
  for (int i = 0; i < maxAttributes; ++i){
    auto bufferID = vao.vertexAttrib[i].bufferID;
    uint8_t*  ind = ((uint8_t*)mem.buffers[bufferID].data) + (uint8_t)(vao.vertexAttrib[i].offset + vao.vertexAttrib[i].stride*inVertex.gl_VertexID);
    
    switch (vao.vertexAttrib[i].type){
      case AttributeType::FLOAT:
      {
        inVertex.attributes[i].v1 = *((float*)ind);
        break;
      }
      case AttributeType::VEC2:
        {
          inVertex.attributes[i].v2 = *((glm::vec2*)ind);
        break;
      }
      case AttributeType::VEC3:
      {
        inVertex.attributes[i].v3 = *((glm::vec3*)ind);
        break;
      }
      case AttributeType::VEC4:
      {
        
        inVertex.attributes[i].v4 = *((glm::vec4*)ind);
        break;
      }
      case AttributeType::UINT:
      {
        inVertex.attributes[i].u1 = *((uint32_t*)ind);
        break;
      }
      case AttributeType::UVEC2:
      {
        inVertex.attributes[i].u2 = *((glm::uvec2*)ind);
        break;}
      case AttributeType::UVEC3:
      {
        inVertex.attributes[i].u3 = *((glm::uvec3*)ind);
        break;}
      case AttributeType::UVEC4:
      {
        inVertex.attributes[i].u4 = *((glm::uvec4*)ind);
        break;}
      default:
      {
        continue;
      }
    }
  }
  return inVertex;
  }    

uint32_t computeVertexID(GPUMemory&mem,VertexArray const&vao,uint32_t shaderInvocation){
    if(vao.indexBufferID<0){return shaderInvocation;}

    Buffer indexBuffer = mem.buffers[vao.indexBufferID];
    if(vao.indexType == IndexType::UINT32){
      uint32_t*ind = ((uint32_t*)((uint64_t)indexBuffer.data)+ vao.indexOffset);
      return ind[shaderInvocation];
      
    }
    else if (vao.indexType == IndexType::UINT8){
      uint8_t*ind = ((uint8_t*)((uint64_t)indexBuffer.data + vao.indexOffset));
      return (uint32_t)ind[shaderInvocation];
      
  }
    else if (vao.indexType == IndexType::UINT16){
      uint16_t*ind = (uint16_t*)((uint64_t)indexBuffer.data + vao.indexOffset);
      return (uint32_t)ind[shaderInvocation];
      
  }
  return shaderInvocation; //this line shouldnt be reached
}

void draw(GPUMemory&mem,DrawCommand cmd, int drawId){
  Program prg = mem.programs[cmd.programID];

  InVertex inVertex;
  inVertex.gl_DrawID = drawId;
  OutVertex outVertex;
    for(int i = 0; i < cmd.nofVertices; ++i){
      ShaderInterface si;
      inVertex.gl_VertexID = computeVertexID(mem, cmd.vao, i);
      inVertex = runVertexAssembly(inVertex, cmd.vao, mem);
      prg.vertexShader(outVertex,inVertex,si);
      
    }
}


//! [gpu_execute]
void gpu_execute(GPUMemory&mem,CommandBuffer &cb){
  (void)mem;
  (void)cb;
  int drawId = 0;
  for(uint32_t i=0;i<cb.nofCommands;++i){
    CommandType type = cb.commands[i].type;
    CommandData data = cb.commands[i].data;
    if(type == CommandType::CLEAR){
      clear(mem,data.clearCommand);
    }
    else if (type == CommandType::DRAW){
      draw(mem, data.drawCommand,(drawId)++);
    }
  }
    
  /// \todo Tato funkce reprezentuje funkcionalitu grafické karty.<br>
  /// Měla by umět zpracovat command buffer, čistit framebuffer a kresli.<br>
  /// mem obsahuje paměť grafické karty.
  /// cb obsahuje command buffer pro zpracování.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
}
//! [gpu_execute]

/**
 * @brief This function reads color from texture.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_texture(Texture const&texture,glm::vec2 uv){
  if(!texture.data)return glm::vec4(0.f);
  auto uv1 = glm::fract(uv);
  auto uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
  auto pix = glm::uvec2(uv2);
  //auto t   = glm::fract(uv2);
  glm::vec4 color = glm::vec4(0.f,0.f,0.f,1.f);
  for(uint32_t c=0;c<texture.channels;++c)
    color[c] = texture.data[(pix.y*texture.width+pix.x)*texture.channels+c]/255.f;
  return color;
}

