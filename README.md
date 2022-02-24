# Simple Kernel OpenCL-API exercises
These are the first OpenCL exercises I did during my Master, in the subject _Programming and Architecture of Computing Systems_, with which I was introduced to this type of programming.

At the beginning, a very basic kernel was given, the kernel ``kernel_pow2.cl``. This kernel creats an array and puts in there the squares of the numbers of an input array. Firstly, the kernel is loaded and converted to an array of characters to be processed by the OpenCL runtime. Then the kernel source is read into buffer and finally the program is created from this buffer. Once this is done, the source code is written in the executable file, ``kernel_pow2.c``, for a given device (CPU, GPU…) using _clBuildProgram_. Now it is necessary to create and initialize all variables and arrays that are going to be used in the kernel and build the corresponding device objects (buffers) for each element that is going to be stored in the device memory. To do this, the OpenCL function _clCreateBuffer_ is used, and then the elements of the input array at the host memory are copied to the corresponding input array at the device memory using _clEnqueueWriteBuffer_.

Before launching the kernel, all arguments that are going to be used in it have to be set. A pointer to the input memory at the device will be used, which should hold the source values on which the kernel operation will be performed, and another pointer to the output memory at the device, which should hold the result values after finishing the computations in the computation device. The rest of the variables that are going to be used do not have to be a pointer because they are not going to be modified.

Finally, the kernel is launched to perform the function to be executed at the computing device using _clEnqueueNDRangeKernel_. To finish the program, the results of the calculations from the output device memory object have to be read and copied to the host memory output array with _clEnqueueReadBuffer_, and all the OpenCL memory objects allocated along this program are released.

The kernels ``kernel_mirror.cl`` and ``kernel_rotate.cl`` flip and rotate an image, respectively. Images below show an example of each of the kernels.

<img src="https://user-images.githubusercontent.com/71872419/155582219-76b9675a-f3b8-4726-8d11-901188d308b7.jpg" width="477" height="318"> <img src="https://user-images.githubusercontent.com/71872419/155582268-6f20af23-d38e-49b8-99b4-fcaa28f6e204.jpg" width="477" height="318">

                        Original image                                                    Flipped image

<img src="https://user-images.githubusercontent.com/71872419/155582501-32a5088d-7d10-4ae8-a3c9-722391189509.jpg" width="477" height="477"> <img src="https://user-images.githubusercontent.com/71872419/155582545-946abcbd-ff4c-404b-8c73-a13657e6abce.jpg" width="477" height="477">

                        Original image                                                    Rotated image
