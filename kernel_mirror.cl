__kernel void image_mirror(
	__global unsigned char *image,
	const unsigned int n_rows,
	const unsigned int n_cols){

	int i = get_global_id(0);
	unsigned char color[] = {0, 0, 0};
	unsigned int col = i / n_rows;
	unsigned int row = i % n_rows;

	color[0] = image[i * 3];
	color[1] = image[i * 3 + 1];
	color[2] = image[i * 3 + 2];

	image[i * 3] = image[(n_rows * (n_cols - col - 1) + row) * 3];
	image[i * 3 + 1] = image[(n_rows * (n_cols - col - 1) + row) * 3 + 1];
	image[i * 3 + 2] = image[(n_rows * (n_cols - col - 1) + row) * 3 + 2];

	image[(n_rows * (n_cols - col - 1) + row) * 3] = color[0];
	image[(n_rows * (n_cols - col - 1) + row) * 3 + 1] = color[1];
	image[(n_rows * (n_cols - col - 1) + row) * 3 + 2] = color[2];
}