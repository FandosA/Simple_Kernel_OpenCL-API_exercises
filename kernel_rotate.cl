__kernel void image_rotation(
    __global unsigned char *image_in,
    __global unsigned char *image_out,
    const float angle,
    const unsigned int n_rows,
    const unsigned int n_cols){

    int i = get_global_id(0);

    int x0 = n_rows / 2;
    int y0 = n_cols / 2;

    int x1 = i / n_cols;
    int y1 = i % n_cols;

    int x2 = cos(angle) * (x1 - x0) - sin(angle) * (y1 - y0) + x0;
    int y2 = sin(angle) * (x1 - x0) + cos(angle) * (y1 - y0) + y0;

    if(x2 < n_rows && y2 < n_cols && x2 >= 0 && y2 >= 0){
        image_out[(x2 * n_cols + y2) * 3] = image_in[i*3];
        image_out[(x2 * n_cols + y2) * 3 + 1] = image_in[i*3+1];
        image_out[(x2 * n_cols + y2) * 3 + 2] = image_in[i*3+2];
    }
}
