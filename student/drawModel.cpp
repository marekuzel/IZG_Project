/*!
 * @file
 * @brief This file contains functions for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#include <student/drawModel.hpp>
#include <student/gpu.hpp>

///\endcond


void addDrawCommand(CommandBuffer&cb, bool doubleSided,GPUMemory&mem, uint32_t nofIndices){
  cb.commands[(cb.nofCommands)++].type = CommandType::DRAW;
  if (doubleSided){
    cb.commands[cb.nofCommands-1].data.drawCommand.backfaceCulling = false;
  }
  else{
    cb.commands[cb.nofCommands-1].data.drawCommand.backfaceCulling = true;
  }
  cb.commands[cb.nofCommands-1].data.drawCommand.programID = 0;
  cb.commands[cb.nofCommands-1].data.drawCommand.nofVertices = nofIndices;
}
void prepareNode(GPUMemory&mem,CommandBuffer&cb,Node const&node,Model const&model){
    if(node.mesh >= 0){ 
      Mesh const&mesh = model.meshes[node.mesh];
      bool doubleSided = true;
      if(mesh.doubleSided == false){doubleSided = false;}
      addDrawCommand(cb,doubleSided,mem,mesh.nofIndices);
      
    }
  for(size_t i=0;i<node.children.size();++i){
    prepareNode(mem, cb, node.children[i], model);
  }
 }

/**
 * @brief This function prepares model into memory and creates command buffer
 *
 * @param mem gpu memory
 * @param commandBuffer command buffer
 * @param model model structure
 */
//! [drawModel]
void prepareModel(GPUMemory&mem,CommandBuffer&commandBuffer,Model const&model){
  (void)mem;
  (void)commandBuffer;
  (void)model;
  mem.programs[0].vertexShader= drawModel_vertexShader;
  mem.programs[0].fragmentShader= drawModel_fragmentShader;
  if (commandBuffer.nofCommands == 0){
    commandBuffer.nofCommands =1;
    commandBuffer.commands[0].type =CommandType::CLEAR;
    commandBuffer.commands[0].data.clearCommand.clearColor = true;
    commandBuffer.commands[0].data.clearCommand.clearDepth = true;
    commandBuffer.commands[0].data.clearCommand.color = glm::vec4(0.1f,0.15f,0.1f,1.f);
    commandBuffer.commands[0].data.clearCommand.depth = 1e+11;
  }
  for(int i=0;i<model.roots.size();++i){
      prepareNode(mem,commandBuffer,model.roots[i],model);
  }
  
  for(int i=0;i<model.buffers.size();++i)
    mem.buffers[i] = model.buffers[i];

  for(int i=0;i<model.textures.size();++i)
    mem.textures[i] = model.textures[i];

  mem.programs[0].vs2fs[0] = AttributeType::VEC3;
  mem.programs[0].vs2fs[1] = AttributeType::VEC3;
  mem.programs[0].vs2fs[2] = AttributeType::VEC2;
  mem.programs[0].vs2fs[3] = AttributeType::UINT;
  /// \todo Tato funkce připraví command buffer pro model a nastaví správně pamět grafické karty.<br>
  /// Vaším úkolem je správně projít model a vložit vykreslovací příkazy do commandBufferu.
  /// Zároveň musíte vložit do paměti textury, buffery a uniformní proměnné, které buffer command buffer využívat.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace a v testech.
}
//! [drawModel]

/**
 * @brief This function represents vertex shader of texture rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param si shader interface
 */
//! [drawModel_vs]
void drawModel_vertexShader(OutVertex&outVertex,InVertex const&inVertex,ShaderInterface const&si){
  (void)outVertex;
  (void)inVertex;
  (void)si;
  /// \todo Tato funkce reprezentujte vertex shader.<br>
  /// Vaším úkolem je správně trasnformovat vrcholy modelu.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
}
//! [drawModel_vs]

/**
 * @brief This functionrepresents fragment shader of texture rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param si shader interface
 */
//! [drawModel_fs]
void drawModel_fragmentShader(OutFragment&outFragment,InFragment const&inFragment,ShaderInterface const&si){
  (void)outFragment;
  (void)inFragment;
  (void)si;
  /// \todo Tato funkce reprezentujte fragment shader.<br>
  /// Vaším úkolem je správně obarvit fragmenty a osvětlit je pomocí lambertova osvětlovacího modelu.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
}
//! [drawModel_fs]

