#pragma once

// Component that holds the data for rendering particles
struct render_data {
	unsigned int vertex_buffer = 0;
	unsigned int color_buffer = 0;
	unsigned int vertex_array = 0;
	unsigned int vertex_shader = 0;
	unsigned int fragment_shader = 0;
	unsigned int program = 0;
	int mvp_location = 0;
	int vpos_location = 0;
	int vcol_location = 0;
};
