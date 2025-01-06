#version 410 core

in vec3 f_position;
in vec4 f_light_position;
in vec3 f_normal;

uniform sampler2DShadow depth_texture;
uniform vec3 light_dir;

out vec4 shadow;

void main()
{
  vec3 proj_coords = (f_light_position.xyz / f_light_position.w) * 0.5 + 0.5;
  float shadow_bias = 0.0001;
  float bias = max(shadow_bias * (1.0 - dot(f_normal, light_dir)), shadow_bias);

  proj_coords.z -= bias;

  // shadow outside the light's frustrum
  if (proj_coords.z > 1.0)
  {
    shadow = vec4(0., 0., 0., 1.);
  }
  else
  {
    float closest_depth = texture(depth_texture, proj_coords.xyz);
    shadow = vec4(1. - closest_depth, 0., 0., 1.);
  }
}