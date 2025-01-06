#version 410 core

in vec3 position;
in vec2 texcoord1;
in vec2 texcoord2;
in mat4 transform;

out vec2 uv1;
out vec2 uv2;

uniform mat4 view_proj;

struct m2_data
{
  vec4 mesh_color;

  int fog_mode;
  int unfogged;
  int unlit;
  int pixel_shader;

  mat4 tex_matrix_1;
  mat4 tex_matrix_2;
  
  uvec2 texture_handle_1;
  uvec2 pad1;
  uvec2 texture_handle_2;
  uvec2 pad2;

  int index_1;
  int index_2;
  ivec2 padding;

  float alpha_test;
  int tex_unit_lookup_1;
  int tex_unit_lookup_2;
  int tex_count;
};

uniform int index;

layout (std140) uniform render_data
{
  m2_data data[192];
};

void main()
{
  gl_Position = view_proj * transform * vec4(position, 1.f);

  uv1 = texcoord1;
  uv2 = texcoord2;
}