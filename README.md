# Dascalu Stefan-Nicolae
# Image Editor in C

## Overview

This project is a C-based image manipulation tool capable of processing both grayscale and RGB images. The program provides functionalities to load, transform, and save images, with support for ASCII and binary formats. Key features include rotating, cropping, histogram generation, and applying visual effects such as edge detection, sharpening, and blurring.

## Program Flow

The project is structured into several essential functions that cover the entire workflow of image manipulation. These functions include loading an image, selecting and transforming specific regions, applying effects, and saving the processed result.

### Main Workflow

1. **Loading Images**  
   The workflow begins by loading an image from a specified file path. The program supports four primary image formats:
   - **P2**: ASCII grayscale
   - **P3**: ASCII RGB
   - **P5**: Binary grayscale
   - **P6**: Binary RGB

   During this step, the program reads the file header to determine the image format, dimensions, and maximum color value, and then allocates memory to store the pixel data.

2. **Selecting Regions**  
   After loading, users can specify a region of the image to be manipulated. The program ensures that selected coordinates follow the correct order (top-left to bottom-right). This selection step allows transformations and effects to be applied only to a specific part of the image if desired.

3. **Image Transformations**  
   The program includes various transformation functions, such as:
   - **Rotation**: Rotates the entire image or a selected region by multiples of 90 degrees.
   - **Cropping**: Crops the image to a specified rectangular region, adjusting the image dimensions accordingly.
   
   These transformations provide flexible options for adjusting the appearance and focus of the image.

4. **Applying Effects**  
   The program supports several visual effects, each applied to the RGB channels of the selected region. Available effects include:
   - **Edge Detection**: Enhances the edges in the image to highlight boundaries.
   - **Sharpening**: Makes details in the image more pronounced.
   - **Blur**: Smoothens the image to reduce noise or create a softened effect.
   - **Gaussian Blur**: Applies a more complex blur that preserves some sharpness in high-frequency areas.

   Effects are applied by using predefined matrices for each filter type. The program makes a copy of the selected region to apply the effect before updating the original image.

5. **Saving Images**  
   Once transformations and effects are complete, the processed image can be saved in either ASCII or binary format. For grayscale images, the output is in P2 or P5 formats, while RGB images are saved in P3 or P6 formats. The program ensures that ASCII files do not exceed line length limits by controlling the number of characters per line.

6. **Histogram Generation**  
   For grayscale images, the program can generate a histogram representing pixel intensity distribution across specified regions. This feature is useful for analyzing the tonal range and contrast of the image. The histogram is displayed as a textual representation, with scaled asterisks indicating the relative frequency of intensity levels.

## Summary of Features

- **Image Loading**: Supports four image formats for flexible input options.
- **Region Selection**: Allows users to specify areas for targeted transformations.
- **Transformations**: Includes rotation and cropping functions.
- **Effects**: Supports edge detection, sharpening, blur, and Gaussian blur for enhancing image quality.
- **Saving**: Outputs images in ASCII or binary formats.
- **Histogram**: Generates histograms for grayscale images to visualize intensity distributions.

This modular approach allows each function to operate independently, enabling efficient and flexible image manipulation.
