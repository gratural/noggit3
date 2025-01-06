// This file is part of Noggit3, licensed under GNU General Public License (version 3).
#version 410 core

#ifdef use_bindless
#extension GL_ARB_bindless_texture : require
#else
uniform sampler2DArray array_0;
#endif




struct batch_uniforms
{
  uvec2 texture_1;
  uvec2 padding_1;
  uvec2 texture_2;
  uvec2 padding_2;

  int texture_index_1;
  int texture_index_2;
  int use_vertex_color;
  int exterior_lit;

  int shader_id;
  int unfogged;
  int unlit;
  float alpha_test;
};


flat in int index;

layout (std140) uniform render_data
{
  batch_uniforms data[96];
};

in vec2 f_texcoord;

void main()
{
  vec4 out_color;

#ifdef use_bindless
  vec4 tex = texture(sampler2DArray(data[index].texture_1), vec3(f_texcoord, data[index].texture_index_1));
#else
  vec4 tex = texture(array_0, vec3(f_texcoord, data[index].texture_index_1));
#endif
  
  int shader = data[index].shader_id;

  // only those shaders use the texture for alpha testing
  if((shader == 1 || shader == 2) && (tex.a < data[index].alpha_test))
  {
    discard;
  }
}
