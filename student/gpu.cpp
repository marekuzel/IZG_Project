/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>

void clear(GPUMemory&mem,ClearCommand cmd){
  
  auto & frame = mem.framebuffer;
    auto nOfPixels = frame.height*frame.width;
    for (int i = 0; i < nOfPixels; ++i){
      if(cmd.clearColor && !(cmd.clearDepth)){  
        float red = cmd.color.r;
        mem.framebuffer.color[i*4] = (uint8_t)red*255.f;
        float green = cmd.color.g;
        mem.framebuffer.color[i*4+1] = (uint8_t)green*255.f;
        float blue = cmd.color.b;
        mem.framebuffer.color[i*4+2] = (uint8_t)blue*255.f;
        float aplha = cmd.color.a;
        mem.framebuffer.color[i*4+3] = (uint8_t)aplha*255.f;
        
      }
      else if (cmd.clearColor && cmd.clearDepth){
        float red = cmd.color.r;
        mem.framebuffer.color[i*4] = (uint8_t)red*255.f;
        float green = cmd.color.g;
        mem.framebuffer.color[i*4+1] = (uint8_t)green*255.f;
        float blue = cmd.color.b;
        mem.framebuffer.color[i*4+2] = (uint8_t)blue*255.f;
        float aplha = cmd.color.a;
        mem.framebuffer.color[i*4+3] = (uint8_t)aplha*255.f;
        mem.framebuffer.depth[i] = cmd.depth;
      }
      else{
        mem.framebuffer.depth[i] = cmd.depth;
      }
  }
}

//! [gpu_execute]
void gpu_execute(GPUMemory&mem,CommandBuffer &cb){
  (void)mem;
  (void)cb;
  for(uint32_t i=0;i<cb.nofCommands;++i){
    CommandType type = cb.commands[i].type;
    CommandData data = cb.commands[i].data;
    if(type == CommandType::CLEAR){
      clear(mem,data.clearCommand);
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

