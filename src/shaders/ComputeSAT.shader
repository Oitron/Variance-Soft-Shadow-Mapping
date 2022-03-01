
//Example code of compute summed area tables (SAT) from OpenGL Superbible, Chapter 10
//Check this book out -> https://books.google.fr/books/about/OpenGL_Superbible.html?id=Nwo0CgAAQBAJ&redir_esc=y

#version 430 core

precision highp float;
precision highp int;


layout(local_size_x = 1024) in;

shared vec2 shared_data[gl_WorkGroupSize.x * 2];


layout(rg32f, binding = 0) readonly uniform image2D input_image;
layout(rg32f, binding = 1) writeonly uniform image2D output_image;


void main(void)
{
	uint id = gl_LocalInvocationID.x;
	uint rd_id;
	uint wr_id;
	uint mask;
	ivec2 P = ivec2(id * 2, gl_WorkGroupID.x);
	const uint steps = uint(log2(gl_WorkGroupSize.x)) + 1;
	uint step = 0;
	shared_data[id * 2] = imageLoad(input_image, P).rg;
	shared_data[id * 2 + 1] = imageLoad(input_image, P + ivec2(1, 0)).rg;

	barrier();
	memoryBarrierShared();

	for (step = 0; step < steps; step++)
	{
		mask = (1 << step) - 1;
		rd_id = ((id >> step) << (step + 1)) + mask;
		wr_id = rd_id + 1 + (id & mask);
		shared_data[wr_id] += shared_data[rd_id];

		barrier();
		memoryBarrierShared();
	}

	imageStore(output_image, P.yx, vec4(shared_data[id * 2], 0.0, 0.0));
	imageStore(output_image, P.yx + ivec2(0, 1), vec4(shared_data[id * 2 + 1], 0.0, 0.0));
}