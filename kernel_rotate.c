////////////////////////////////////////////////////////////////////
//File: kernel_rotate.c
//
//Description: base file for environment exercises with openCL
//
// 
////////////////////////////////////////////////////////////////////

#include "CImg.h"
#include <atomic>
#include <cmath>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <limits>
#include <iomanip>
#include <utility>
#include <numeric>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __APPLE__
  #include <OpenCL/opencl.h>
#else
  #include <CL/cl.h>
#endif

#define PI 3.14159265

using namespace cimg_library;
  
// check error, in such a case, it exits

void cl_error(cl_int code, const char *string){
	if (code != CL_SUCCESS){
		printf("%d - %s\n", code, string);
	    exit(-1);
	}
}
////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
    auto start_program = std::chrono::high_resolution_clock::now();

    int err;                            	// error code returned from api calls
    size_t t_buf = 50;			// size of str_buffer
    char str_buffer[t_buf];		// auxiliary buffer	
    size_t e_buf;				// effective size of str_buffer in use
	    
    size_t global_size;                      	// global domain size for our calculation
    size_t local_size;                       	// local domain size for our calculation

    const cl_uint num_platforms_ids = 10;				// max of allocatable platforms
    cl_platform_id platforms_ids[num_platforms_ids];		// array of platforms
    cl_uint n_platforms;						// effective number of platforms in use
    const cl_uint num_devices_ids = 10;				// max of allocatable devices
    cl_device_id devices_ids[num_platforms_ids][num_devices_ids];	// array of devices
    cl_uint n_devices[num_platforms_ids];				// effective number of devices in use for each platform
	
    cl_device_id device_id;             				// compute device id 
    cl_context context;                 				// compute context
    cl_command_queue command_queue;     				// compute command queue
    

    // 1. Scan the available platforms:
    err = clGetPlatformIDs (num_platforms_ids, platforms_ids, &n_platforms);
    cl_error(err, "Error: Failed to Scan for Platforms IDs");
    printf("Number of available platforms: %d\n", n_platforms);

    for (int i = 0; i < n_platforms; i++ ){
        err = clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_NAME, t_buf, str_buffer, &e_buf);
        cl_error(err, "Error: Failed to get info of the platform\n");
        printf("\t[%d]-Platform Name: %s\n", i, str_buffer);

        err = clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_VENDOR, t_buf, str_buffer, &e_buf);
        cl_error(err, "Error: Failed to get info of the platform\n");
        printf("\t[%d]-Platform Vendor: %s\n", i, str_buffer);

        err = clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_VERSION, t_buf, str_buffer, &e_buf);
        cl_error(err, "Error: Failed to get info of the platform\n");
        printf("\t[%d]-Platform Version: %s\n", i, str_buffer);

        err = clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_PROFILE, t_buf, str_buffer, &e_buf);
        cl_error(err, "Error: Failed to get info of the platform\n");
        printf("\t[%d]-Platform Profile: %s\n", i, str_buffer);

	cl_ulong host_timer_resolution;
	err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_HOST_TIMER_RESOLUTION, t_buf, str_buffer, &host_timer_resolution);
        cl_error(err, "Error: Failed to get info of the platform\n");
        printf("\t[%d]-Platform host timer resolution: %d\n", i, host_timer_resolution);

        // Aquí salta el error
        //err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_EXTENSIONS, t_buf, str_buffer, &e_buf);
        //cl_error (err, "Error: Failed to get info of the platform\n");
        //printf( "\t[%d]-Platform Extensions: %s\n", i, str_buffer);
    }
    printf("\n");
    // ***Task***: print on the screen the name, host_timer_resolution, vendor, versionm, ...
	
    // 2. Scan for devices in each platform
    for (int i = 0; i < n_platforms; i++){
    err = clGetDeviceIDs(platforms_ids[i], CL_DEVICE_TYPE_ALL, num_devices_ids, devices_ids[i], &(n_devices[i]));
    cl_error(err, "Error: Failed to Scan for Devices IDs");
    printf("\t[%d]-Platform. Number of available devices: %d\n", i, n_devices[i]);

        for(int j = 0; j < n_devices[i]; j++){
            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_NAME, sizeof(str_buffer), &str_buffer, NULL);
            cl_error(err, "clGetDeviceInfo: Getting device name");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_NAME: %s\n", i, j, str_buffer);

            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_PROFILE, sizeof(str_buffer), &str_buffer, NULL);
            cl_error(err, "clGetDeviceInfo: Getting device profile");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_PROFILE: %s\n", i, j, str_buffer);

            cl_ulong profiling_timer_resolution;
      	    err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(profiling_timer_resolution), &profiling_timer_resolution, NULL);            cl_error(err, "clGetDeviceInfo: Getting device profiling timer resolution");
            cl_error(err, "clGetDeviceInfo: Getting profiling timer resolution");
	    printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_PROFILING_TIMER_RESOLUTION: %d\n", i, j, profiling_timer_resolution);

            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_VENDOR, sizeof(str_buffer), &str_buffer, NULL);
            cl_error(err, "clGetDeviceInfo: Getting device vendor");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_VENDOR: %s\n", i, j, str_buffer);

            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_VENDOR_ID, sizeof(str_buffer), &str_buffer, NULL);
            cl_error(err, "clGetDeviceInfo: Getting device vendor_id");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_VENDOR_ID: %s\n", i, j, str_buffer);

            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_VERSION, sizeof(str_buffer), &str_buffer, NULL);
            cl_error(err, "clGetDeviceInfo: Getting device version");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_VERSION: %s\n", i, j, str_buffer);

            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_OPENCL_C_VERSION, sizeof(str_buffer), &str_buffer, NULL);
            cl_error(err, "clGetDeviceInfo: Getting openCL C version");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_OPENCL_C_VERSION: %s\n", i, j, str_buffer);

            cl_uint max_compute_units_available;
            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units_available), &max_compute_units_available, NULL);
            cl_error(err, "clGetDeviceInfo: Getting device max compute units available");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_COMPUTE_UNITS: %d\n", i, j, max_compute_units_available);

            cl_uint max_work_item_dimensions;
            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(max_work_item_dimensions), &max_work_item_dimensions, NULL);
            cl_error(err, "clGetDeviceInfo: Getting device max work item dimensions");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %d\n", i, j, max_work_item_dimensions);

            size_t* max_work_item_sizes = (size_t*)malloc(sizeof(size_t) * max_work_item_dimensions);
            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * max_work_item_dimensions, max_work_item_sizes, NULL);
            cl_error(err, "clGetDeviceInfo: Getting device max work item sizes");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_WORK_ITEM_SIZES: %d\n", i, j, max_work_item_sizes);

            size_t max_work_group_size;
            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size), &max_work_group_size, NULL);
            cl_error(err, "clGetDeviceInfo: Getting device max work group size");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_WORK_GROUP_SIZE: %d\n", i, j, max_work_group_size);

            cl_uint max_clock_frequency;
            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(max_clock_frequency), &max_clock_frequency, NULL);
            cl_error(err, "clGetDeviceInfo: Getting mac clock frequency");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_CLOCK_FREQUENCY: %d\n", i, j, max_clock_frequency);

            cl_ulong max_mem_alloc_size;
            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_mem_alloc_size), &max_mem_alloc_size, NULL);
            cl_error(err, "clGetDeviceInfo: Getting max mem alloc size");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_MEM_ALLOC_SIZE: %d\n", i, j, max_mem_alloc_size);

            size_t max_parameter_size;
            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(max_parameter_size), &max_parameter_size, NULL);
            cl_error(err, "clGetDeviceInfo: Getting max parameter size");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_PARAMETER_SIZE: %d\n", i, j, max_parameter_size);

            cl_ulong global_mem_cache_size;
            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(global_mem_cache_size), &global_mem_cache_size, NULL);
            cl_error(err, "clGetDeviceInfo: Getting global mem cache size");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_GLOBAL_MEM_CACHE_SIZE: %d\n", i, j, global_mem_cache_size);

            cl_ulong global_mem_size;
            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(global_mem_size), &global_mem_size, NULL);
            cl_error(err, "clGetDeviceInfo: Getting global mem size");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_GLOBAL_MEM_SIZE: %d\n", i, j, global_mem_size);

            cl_ulong max_constant_buffer_size;
            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(max_constant_buffer_size), &max_constant_buffer_size, NULL);
            cl_error(err, "clGetDeviceInfo: Getting max constant buffer size");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE: %d\n", i, j, max_constant_buffer_size);

            cl_uint max_constant_args;
            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(max_constant_args), &max_constant_args, NULL);
            cl_error(err, "clGetDeviceInfo: Getting max constant args");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_CONSTANT_ARGS: %d\n", i, j, max_constant_args);

            cl_device_id parent_device;
            err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_PARENT_DEVICE, sizeof(parent_device), &parent_device, NULL);
            cl_error(err, "clGetDeviceInfo: Getting parent device");
            printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_PARENT_DEVICE: %d\n", i, j, parent_device);

            // Salta el error
            //err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_EXTENSIONS, sizeof(str_buffer), &str_buffer, NULL);
            //cl_error(err, "clGetDeviceInfo: Getting device extensions");
            //printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_EXTENSIONS: %s\n", i, j,str_buffer);

        }
    }
    // ***Task***: print on the screen the cache size, global mem size, local memsize, max work group size, profiling timer resolution and ... of each device
   
    // Create a context, with a device
    cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platforms_ids[0], 0}; // Modificado
    context = clCreateContext(properties, n_devices[0], devices_ids[0], NULL, NULL, &err);
    cl_error(err, "Failed to create a compute context\n");
    
    // 4Create a command queue
    cl_command_queue_properties proprt[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
    command_queue = clCreateCommandQueueWithProperties(context, devices_ids[0][0], proprt, &err);
    cl_error(err, "Failed to create a command queue\n");

    // 2. Calculate size of the file
    FILE* fileHandler = fopen("kernel_rotate.cl", "r");
    if (fileHandler == NULL) {
	fprintf(stderr, "Could not open the kernel\n");
	exit(1);
    }

    fseek(fileHandler, 0, SEEK_END);
    size_t fileSize = ftell(fileHandler);
    rewind(fileHandler);

    // read kernel source into buffer
    char* sourceCode = (char*)malloc(fileSize + 1);
    sourceCode[fileSize] = '\0';
    fread(sourceCode, sizeof(char), fileSize, fileHandler);
    fclose(fileHandler);

    // Create program from buffer
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&sourceCode, &fileSize, &err);
    cl_error(err, "Failed to create program with source\n");
    free(sourceCode);

    // 3. Build the executable and check errors
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS){
        size_t len;
        char buffer[302048];
        printf("Error: Some error at building process.\n");
        clGetProgramBuildInfo(program, devices_ids[0][0], CL_PROGRAM_BUILD_LOG, 302048, buffer, &len);
        printf("%s\n", buffer);
        exit(-1);
    }

    // 4. Create a compute kernel with the program we want to run
    cl_kernel kernel = clCreateKernel(program, "image_rotation", &err);
    cl_error(err, "Failed to create kernel from the program\n");

    // 5. Create input and output arrays

    CImg<unsigned char> img("image.jpg");
  
    int n_rows = img.height();
    int n_cols = img.width();
    int size = n_rows * n_cols * 3;
    unsigned char imagen_in[size];
    unsigned char imagen_out[size];

    float angle = 90.0; // Put it in degrees
    angle = angle * 3.1415926535 / 180;

    int pos_img = 0;

    for(int rows = 0; rows < img.height(); rows++)
		for (int cols = 0; cols < img.width(); cols++) {

			imagen_in[pos_img] = img(cols, rows, 0, 0); //R
			imagen_out[pos_img] = 0; //R
			pos_img++;

			imagen_in[pos_img] = img(cols, rows, 0, 1); //G
			imagen_out[pos_img] = 0; //G
			pos_img++;

			imagen_in[pos_img] = img(cols, rows, 0, 2); //B
			imagen_out[pos_img] = 0; //B
			pos_img++;
		}

    // 6. Create OpenCL buffer visible to the OpenCl runtime
    cl_mem in_device_object = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_char) * size, NULL, &err);
    cl_error(err, "Failed to create memory buffer at device\n");
    cl_mem out_device_object = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_char) * size, NULL, &err);
    cl_error(err, "Failed to create memory buffer at device\n");


    // 7. Write date into the memory object 
    auto start_commandqueue_hostDevice = std::chrono::high_resolution_clock::now();
    err = clEnqueueWriteBuffer(command_queue, in_device_object, CL_TRUE, 0, sizeof(unsigned char) * size, imagen_in, 0, NULL, NULL);
    auto end_commandqueue_hostDevice = std::chrono::high_resolution_clock::now();
    cl_error(err, "Failed to enqueue a write command\n");
    std::chrono::duration<double> commandqueue_duration_hostDevice = end_commandqueue_hostDevice - start_commandqueue_hostDevice;


    // 8. Set the arguments to the kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &in_device_object);
    cl_error(err, "Failed to set argument 0\n");
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &out_device_object);
    cl_error(err, "Failed to set argument 1\n");
    err = clSetKernelArg(kernel, 2, sizeof(angle), &angle);
    cl_error(err, "Failed to set argument 2\n");
    err = clSetKernelArg(kernel, 3, sizeof(n_rows), &n_rows);
    cl_error(err, "Failed to set argument 3\n");
    err = clSetKernelArg(kernel, 4, sizeof(n_cols), &n_cols);
    cl_error(err, "Failed to set argument 4\n");


    // 9. Launch Kernel
    local_size = 1; // size/27
    global_size = size / 3;

    cl_event event;
    err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, &event);
    cl_error(err, "Failed to launch kernel to the device\n");

    clFinish(command_queue);
    //clWaitForEvents(1, &event);


    // 10. Read data form device memory back to host memory
    auto start_commandqueue_deviceHost = std::chrono::high_resolution_clock::now();
    err = clEnqueueReadBuffer(command_queue, out_device_object, CL_TRUE, 0, sizeof(unsigned char) * size, &imagen_out, 0, NULL, NULL);
    auto end_commandqueue_deviceHost = std::chrono::high_resolution_clock::now();
    cl_error(err, "Failed to enqueue a read command\n");
    std::chrono::duration<double> commandqueue_duration_deviceHost = end_commandqueue_deviceHost - start_commandqueue_deviceHost;

    // 11. Check the results
    pos_img = 0;
    for(int rows = 0; rows < img.height(); rows++)
	for (int cols = 0; cols < img.width(); cols++) {

	    img(cols, rows, 0, 0) = imagen_out[pos_img]; //R
	    pos_img++;

	    img(cols, rows, 0, 1) = imagen_out[pos_img]; //G
	    pos_img++;

	    img(cols, rows, 0, 2) = imagen_out[pos_img]; //B
	    pos_img++;
	}

    //img.display("result.jpg"); // Display the image in a display window
    img.save("rotated90.jpg");
    
    cl_ulong time_start;
    cl_ulong time_end;
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
    double nanoSeconds = time_end - time_start; 

    size_t memory_footPrint = 0;
    err = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &memory_footPrint, NULL);
    cl_error(err, "Failed to get program info\n");

    // 12
    clReleaseMemObject(in_device_object);
    clReleaseMemObject(out_device_object);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);

    std::cout << "\nAll memory objects released with success." << std::endl;

    auto end_program = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> program_duration = end_program - start_program;


    std::cout << "Execution time of the entire program: " << program_duration.count()*1000 << " milliseconds\n";
    printf("Execution time of the kernel: %0.3f milliseconds \n",nanoSeconds / 1000000.0);
    std::cout << "The memory footprint is: " << memory_footPrint << std::endl;
    std::cout << "Bandwith host-device -> sizeof(cl_char) * size / time needed to pass the data: " << sizeof(cl_char) * size / commandqueue_duration_hostDevice.count() << " bytes/seconds\n";
    std::cout << "Bandwith device-host -> sizeof(cl_char) * size / time needed to pass the data: " << sizeof(cl_char) * size / commandqueue_duration_deviceHost.count() << " bytes/seconds\n" << std::endl;
    std::cout << "Throughput of the kernel: " << img.width()*img.height() / (nanoSeconds / 1000000000.0) << " pixels per second" << std::endl;
    std::cout << "Kernel does 3 loadings and 3 storages each time it is launched, " <<  
	"so the kernel bandwith would be: " << 6*sizeof(cl_uchar)*size/3 / (nanoSeconds / 1000000000.0) << " bytes/sec\n" << std::endl;

    
    return 0;
}