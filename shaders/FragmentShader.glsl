#version 330

// Input
// TODO(student): Get color value from vertex shader
in vec3 color;
in float hp_out;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Write pixel out color
    if (hp_out != -1) {
        out_color = vec4 (color.x + (100 - hp_out) / 100, color.y, color.z, 1.0);
    }
    else {
		out_color = vec4 (color, 1.0);
	}
}
