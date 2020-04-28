#pragma once

// Component that holds the data for rendering particles
struct render_data {
	unsigned int vertex_buffer;
	unsigned int vertex_array;
	unsigned int vertex_shader;
	unsigned int fragment_shader;
	unsigned int program;
	int mvp_location;
	int vpos_location;
	int vcol_location;
};
