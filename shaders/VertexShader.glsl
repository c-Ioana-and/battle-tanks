#version 330

// Input
// spatiul obiect, initial
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Uniforms for light properties
// coordonatele scenei, spatiul lume, global
uniform vec3 light_position;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;

uniform float hp;

// Output value to fragment shader
out vec3 color;
out float hp_out;


void main()
{
    // TODO(student): Compute world space vectors
    vec3 world_position = (Model * vec4(v_position,1)).xyz;
    vec3 world_normal = normalize( mat3(Model) * v_normal );

    vec3 L = normalize(light_position - world_position);
    vec3 R = reflect(-L, world_normal);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);

    // Define ambient light component
    float ambient_light = material_kd * 0.25;

    // Compute diffuse light component
    float diffuse_light = material_kd * max(0, dot(world_normal, L));

    // Compute specular light component
    float specular_light = 0;

    if (diffuse_light > 0)
    {
        // primeste lumina
        specular_light = material_ks * pow(max(0, dot(R, L)), material_shininess);

        // varianta cu vectorul median
        //float specular_light = material_ks * pow(max(0, dot(R, normalize(-world_position))), material_shininess);

    }

    // Send color light output to fragment shader
    float distance = length(light_position - world_position);
    float factorAtenuare = 1.0 / (1.0 + 0.3 * distance + 0.5 * distance * distance);
    color = object_color * (ambient_light + (diffuse_light + specular_light) * factorAtenuare);

    hp_out = hp;
    // change the position of the vertex based on hp

    vec3 new_position = v_position;
    if (hp != -1) {
        new_position.y -= (100 - hp) / 100.0;
    }

    gl_Position = Projection * View * Model * vec4(new_position, 1.0);
}
