// This file is part of Noggit3, licensed under GNU General Public License (version 3).
#version 410 core

in vec4 position;
in vec2 uv1;
in int id;

in mat4 transform;

out vec2 f_texcoord;

flat out int index;

uniform mat4 view_proj;

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


layout (std140) uniform render_data
{
  batch_uniforms data[96];
};

void main()
{
  index = id;
  f_texcoord = uv1;

  gl_Position = view_proj * transform * position;
}
