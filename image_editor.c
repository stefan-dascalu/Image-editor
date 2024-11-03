//Dascalu Stefan-Nicolae @copyright

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Structure to represent a colored pixel, using double for higher precision in filters
typedef struct {
    double R, G, B;
} RGBPixel;

// Function to swap two integer values
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Clamping function to restrict values between 0 and 255
double clamp(double value) {
    if (value > 255) return 255;
    if (value < 0) return 0;
    return value;
}

// Function to count the number of digits in a number
int count_digits(int number) {
    int count = 0;
    while (number) {
        count++;
        number /= 10;
    }
    return count == 0 ? 1 : count; // Return 1 if the number is zero
}

// Function to load an image based on the specified command
void *load_image(char *command, unsigned char ***image, RGBPixel ***image_RGB, int *width, int *height, int *image_type) {
    // Extract the filename from the command
    char *filename = strtok(command, "\n ");
    filename = strtok(NULL, "\n ");
    
    FILE *inputFile = fopen(filename, "r");
    if (!inputFile) {
        printf("Failed to load %s\n", filename);
        return NULL;
    }

    // Free previously allocated memory for images if necessary
    if (*image) {
        for (int i = 0; i < *height; i++) {
            free((*image)[i]);
        }
        free(*image);
        *image = NULL;
    }

    if (*image_RGB) {
        for (int i = 0; i < *height; i++) {
            free((*image_RGB)[i]);
        }
        free(*image_RGB);
        *image_RGB = NULL;
    }

    char type[3];
    fgets(type, sizeof(type), inputFile); // Read image type
    fscanf(inputFile, "%d %d\n", width, height); // Read dimensions
    int max_value;
    fscanf(inputFile, "%d\n", &max_value); // Read max color value

    // Check the image format and allocate memory accordingly
    if (strcmp(type, "P2") == 0) {
        *image = (unsigned char **)malloc(*height * sizeof(unsigned char *));
        for (int i = 0; i < *height; i++) {
            (*image)[i] = (unsigned char *)malloc(*width * sizeof(unsigned char));
            for (int j = 0; j < *width; j++) {
                fscanf(inputFile, "%hhu", &((*image)[i][j]));
            }
        }
        *image_type = 0; // Grayscale
    } else if (strcmp(type, "P3") == 0) {
        *image_RGB = (RGBPixel **)malloc(*height * sizeof(RGBPixel *));
        for (int i = 0; i < *height; i++) {
            (*image_RGB)[i] = (RGBPixel *)malloc(*width * sizeof(RGBPixel));
            for (int j = 0; j < *width; j++) {
                unsigned char r, g, b;
                fscanf(inputFile, "%hhu %hhu %hhu", &r, &g, &b);
                (*image_RGB)[i][j].R = (double)r;
                (*image_RGB)[i][j].G = (double)g;
                (*image_RGB)[i][j].B = (double)b;
            }
        }
        *image_type = 1; // Color
    } else if (strcmp(type, "P5") == 0) {
        *image = (unsigned char **)malloc(*height * sizeof(unsigned char *));
        for (int i = 0; i < *height; i++) {
            (*image)[i] = (unsigned char *)malloc(*width * sizeof(unsigned char));
            fread((*image)[i], sizeof(unsigned char), *width, inputFile);
        }
        *image_type = 2; // Grayscale binary
    } else if (strcmp(type, "P6") == 0) {
        *image_RGB = (RGBPixel **)malloc(*height * sizeof(RGBPixel *));
        for (int i = 0; i < *height; i++) {
            (*image_RGB)[i] = (RGBPixel *)malloc(*width * sizeof(RGBPixel));
            fread((*image_RGB)[i], sizeof(RGBPixel), *width, inputFile);
        }
        *image_type = 3; // Color binary
    } else {
        printf("Unsupported format\n");
        fclose(inputFile);
        return NULL;
    }
    
    printf("Loaded %s\n", filename); // Inform the user of successful load
    fclose(inputFile); // Close the file
    return (*image) ? *image : *image_RGB; // Return loaded image
}

// Function to select coordinates while enforcing proper order (x1, y1) must be top-left and (x2, y2) bottom-right
void select_coords(int *x1, int *y1, int *x2, int *y2, int new_x1, int new_y1, int new_x2, int new_y2) {
    // Ensure coordinates are in proper order
    if (new_y1 > new_y2) {
        swap(&new_x1, &new_x2);
        swap(&new_y1, &new_y2);
    }
    if (new_x1 > new_x2) {
        swap(&new_x1, &new_x2);
    }
    *x1 = new_x1;
    *y1 = new_y1;
    *x2 = new_x2;
    *y2 = new_y2;
}

// Function to rotate the image counter-clockwise
void rotate_left(unsigned char ***image, RGBPixel ***image_RGB, int type, int x1, int y1, int *x2, int *y2, int *width, int *height) {
    if (type == 0 || type == 2) { // For grayscale images
        unsigned char **rotated_image = (unsigned char **)malloc(*width * sizeof(unsigned char *));
        for (int i = 0; i < *width; i++) {
            rotated_image[i] = (unsigned char *)malloc(*height * sizeof(unsigned char));
        }

        for (int i = 0; i < *height; i++) {
            for (int j = 0; j < *width; j++) {
                rotated_image[*width - j - 1][i] = (*image)[i][j];
            }
        }

        // Free old image memory and update pointer
        for (int i = 0; i < *height; i++) {
            free((*image)[i]);
        }
        free(*image);
        *image = rotated_image;
        swap(width, height); // Swap dimensions
        swap(x2, y2); // Update bounding coordinates
    } else { // For RGB images
        RGBPixel **rotated_image = (RGBPixel **)malloc(*width * sizeof(RGBPixel *));
        for (int i = 0; i < *width; i++) {
            rotated_image[i] = (RGBPixel *)malloc(*height * sizeof(RGBPixel));
        }

        for (int i = 0; i < *height; i++) {
            for (int j = 0; j < *width; j++) {
                rotated_image[*width - j - 1][i] = (*image_RGB)[i][j];
            }
        }

        // Free old image memory and update pointer
        for (int i = 0; i < *height; i++) {
            free((*image_RGB)[i]);
        }
        free(*image_RGB);
        *image_RGB = rotated_image;
        swap(width, height); // Swap dimensions
        swap(x2, y2); // Update bounding coordinates
    }
}

// Function to rotate the image by a specified angle
void rotate_image(unsigned char ***image, RGBPixel ***image_RGB, int type, int x1, int y1, int *x2, int *y2, int *width, int *height, int angle) {
    // Rotate the image based on the given angle
    int numRotations = (angle / 90) % 4; // Determine the number of 90-degree rotations
    for (int i = 0; i < numRotations; i++) {
        rotate_left(image, image_RGB, type, x1, y1, x2, y2, width, height);
    }
}

// Function to save the image in ASCII format
void save_ascii(unsigned char **image, RGBPixel **image_RGB, int type, int width, int height, const char *filename) {
    FILE *outputFile = fopen(filename, "w");
    if (type == 0 || type == 2) { // Grayscale (P2 or P5)
        fprintf(outputFile, "P2\n%d %d\n255\n", width, height);
        for (int i = 0; i < height; i++) {
            int char_count_line = 0; // For limiting characters per line
            for (int j = 0; j < width; j++) {
                char_count_line += 1 + count_digits(image[i][j]);
                if (char_count_line > 2046) {
                    fprintf(outputFile, "\n");
                    char_count_line = 1 + count_digits(image[i][j]);
                }
                fprintf(outputFile, "%hhu ", image[i][j]);
            }
            fputc('\n', outputFile); // New line after each row
        }
    } else { // Color image (P3)
        fprintf(outputFile, "P3\n%d %d\n255\n", width, height);
        for (int i = 0; i < height; i++) {
            int char_count_line = 0; // For limiting characters per line
            for (int j = 0; j < width; j++) {
                unsigned char r = (unsigned char)round(image_RGB[i][j].R);
                unsigned char g = (unsigned char)round(image_RGB[i][j].G);
                unsigned char b = (unsigned char)round(image_RGB[i][j].B);
                char_count_line += 3 + count_digits((int)r) + count_digits((int)g) + count_digits((int)b);
                if (char_count_line > 2046) {
                    fprintf(outputFile, "\n");
                    char_count_line = 3 + count_digits((int)r) + count_digits((int)g) + count_digits((int)b);
                }
                fprintf(outputFile, "%hhu %hhu %hhu ", r, g, b); // Save RGB values
            }
            fputc('\n', outputFile); // New line after each row
        }
    }
    fclose(outputFile); // Close the output file
}

// Function to save the image in binary format
void save_binary(unsigned char **image, RGBPixel **image_RGB, int type, int width, int height, const char *filename) {
    FILE *outputFile = fopen(filename, "wb");
    if (type == 0 || type == 2) { // Grayscale (P5)
        fprintf(outputFile, "P5\n%d %d\n255\n", width, height);
        for (int i = 0; i < height; i++) {
            fwrite(image[i], sizeof(unsigned char), width, outputFile);
        }
    } else { // Color (P6)
        fprintf(outputFile, "P6\n%d %d\n255\n", width, height);
        for (int i = 0; i < height; i++) {
            fwrite(image_RGB[i], sizeof(RGBPixel), width, outputFile);
        }
    }
    fclose(outputFile); // Close the output file
}

// Function to crop the image based on selected coordinates
void crop(unsigned char ***image, RGBPixel ***image_RGB, int type, int x1, int y1, int x2, int y2, int *width, int *height) {
    // Allocate memory for the cropped image
    if (type == 0 || type == 2) { // Grayscale
        unsigned char **temp = (unsigned char **)malloc((y2 - y1) * sizeof(unsigned char *));
        for (int i = 0; i < (y2 - y1); i++) {
            temp[i] = (unsigned char *)malloc((x2 - x1) * sizeof(unsigned char));
            for (int j = x1; j < x2; j++) {
                temp[i][j - x1] = (*image)[i + y1][j]; // Copy pixel values
            }
        }

        // Free old image memory
        for (int i = 0; i < *height; i++) {
            free((*image)[i]);
        }
        free(*image);

        *image = temp; // Update image pointer
        *width = x2 - x1; // Update width
        *height = y2 - y1; // Update height
    } else { // RGB
        RGBPixel **temp = (RGBPixel **)malloc((y2 - y1) * sizeof(RGBPixel *));
        for (int i = 0; i < (y2 - y1); i++) {
            temp[i] = (RGBPixel *)malloc((x2 - x1) * sizeof(RGBPixel));
            for (int j = x1; j < x2; j++) {
                temp[i][j - x1] = (*image_RGB)[i + y1][j]; // Copy pixel values
            }
        }

        // Free old image memory
        for (int i = 0; i < *height; i++) {
            free((*image_RGB)[i]);
        }
        free(*image_RGB);

        *image_RGB = temp; // Update image pointer
        *width = x2 - x1; // Update width
        *height = y2 - y1; // Update height
    }
}

// Effect calculation functions for each channel
double effect_calculation_int_channel(RGBPixel **image, int i, int j, int effect[3][3], char channel) {
    double value = 0.0;
    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            if (channel == 'R') {
                value += image[i + di][j + dj].R * effect[di + 1][dj + 1];
            } else if (channel == 'G') {
                value += image[i + di][j + dj].G * effect[di + 1][dj + 1];
            } else if (channel == 'B') {
                value += image[i + di][j + dj].B * effect[di + 1][dj + 1];
            }
        }
    }
    return clamp(value); // Clamp the calculated value
}

// Display a histogram based on pixel intensity
void print_histogram(unsigned char **image, int x1, int y1, int x2, int y2, int x, int y) {
    int occurrences[y]; // Count occurrences for histogram
    for (int i = 0; i < y; i++) occurrences[i] = 0; // Initialize occurrences to zero
    int division_size = 256 / y; // Compute division size

    // Count pixel values
    for (int i = y1; i < y2; i++) {
        for (int j = x1; j < x2; j++) {
            occurrences[image[i][j] / division_size]++;
        }
    }

    // Find the maximum value for scaling purposes
    int max_occurrences = 0;
    for (int i = 0; i < y; i++) {
        if (occurrences[i] > max_occurrences) {
            max_occurrences = occurrences[i];
        }
    }

    // Print the histogram
    for (int i = 0; i < y; i++) {
        int stars = floor((double)occurrences[i] / max_occurrences * x); // Scale the stars
        printf("%d\t|\t", stars);
        for (int j = 0; j < stars; j++) {
            printf("*"); // Print stars according to the count
        }
        printf("\n");
    }
}

// Perform histogram equalization on the image
void equalize(unsigned char **image, RGBPixel **image_RGB, int type, int width, int height) {
    if (!image && !image_RGB) {
        printf("No image loaded\n");
        return;
    }
    if (type == 1 || type == 3) {
        printf("Black and white image needed\n");
        return;
    }
    int histogram[256] = {0}; // Initialize histogram
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            histogram[image[i][j]]++; // Count pixel values
        }
    }

    // Compute the cumulative histogram
    int cumulative_sum[256] = {0};
    for (int i = 0; i < 256; i++) {
        cumulative_sum[i] = histogram[i];
        if (i > 0) {
            cumulative_sum[i] += cumulative_sum[i - 1];
        }
    }

    // Update the image with equalized values
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j] = clamp(round((double)255 / (width * height) * cumulative_sum[image[i][j]]));
        }
    }
    printf("Equalization completed\n");
}

// Apply specified effects to the image
void apply_effect(RGBPixel **image, int x1, int y1, int x2, int y2, int width, int height, char *effect) {
    // Define effect matrices
    int edge[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}}; // Edge detection
    int sharpen[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}}; // Sharpen
    double blur[3][3] = {{1.0/9, 1.0/9, 1.0/9}, {1.0/9, 1.0/9, 1.0/9}, {1.0/9, 1.0/9, 1.0/9}}; // Blur
    double gaussian_blur[3][3] = {{1.0/16, 1.0/8, 1.0/16}, {1.0/8, 1.0/4, 1.0/8}, {1.0/16, 1.0/8, 1.0/16}}; // Gaussian blur

    // Create a copy of the selected area to apply effects
    RGBPixel **copy = (RGBPixel **)malloc((y2 - y1) * sizeof(RGBPixel *));
    for (int i = 0; i < (y2 - y1); i++) {
        copy[i] = (RGBPixel *)malloc((x2 - x1) * sizeof(RGBPixel));
        for (int j = 0; j < (x2 - x1); j++) {
            copy[i][j] = image[i + y1][j + x1]; // Copy pixel values
        }
    }

    // Apply the specified effect
    for (int i = y1; i < y2; i++) {
        for (int j = x1; j < x2; j++) {
            if (i == 0 || j == 0 || i == height - 1 || j == width - 1) continue; // Skip borders
            if (strcmp(effect, "EDGE") == 0) {
                copy[i - y1][j - x1].R = effect_calculation_int_channel(image, i, j, edge, 'R');
                copy[i - y1][j - x1].G = effect_calculation_int_channel(image, i, j, edge, 'G');
                copy[i - y1][j - x1].B = effect_calculation_int_channel(image, i, j, edge, 'B');
            } else if (strcmp(effect, "SHARPEN") == 0) {
                copy[i - y1][j - x1].R = effect_calculation_int_channel(image, i, j, sharpen, 'R');
                copy[i - y1][j - x1].G = effect_calculation_int_channel(image, i, j, sharpen, 'G');
                copy[i - y1][j - x1].B = effect_calculation_int_channel(image, i, j, sharpen, 'B');
            } else if (strcmp(effect, "BLUR") == 0) {
                copy[i - y1][j - x1].R = effect_calculation_int_channel(image, i, j, blur, 'R');
                copy[i - y1][j - x1].G = effect_calculation_int_channel(image, i, j, blur, 'G');
                copy[i - y1][j - x1].B = effect_calculation_int_channel(image, i, j, blur, 'B');
            } else { // For Gaussian blur
                copy[i - y1][j - x1].R = effect_calculation_int_channel(image, i, j, gaussian_blur, 'R');
                copy[i - y1][j - x1].G = effect_calculation_int_channel(image, i, j, gaussian_blur, 'G');
                copy[i - y1][j - x1].B = effect_calculation_int_channel(image, i, j, gaussian_blur, 'B');
            }
        }
    }

    // Copy modified pixels back to the original image
    for (int i = y1; i < y2; i++) {
        for (int j = x1; j < x2; j++) {
            image[i][j] = copy[i - y1][j - x1];
        }
    }

    // Free memory allocated for the copied image
    for (int i = 0; i < (y2 - y1); i++) {
        free(copy[i]);
    }
    free(copy);
}

// Function to parse and load an image
void parse_load(char *command, unsigned char ***image, RGBPixel ***image_RGB, int *width, int *height, int *type, int *x1, int *y1, int *x2, int *y2) {
    // Load the image and handle memory accordingly
    void *loaded_image = load_image(command, image, image_RGB, width, height, type);
    if (loaded_image) {
        if (*type == 0 || *type == 2) {
            *image = (unsigned char **)loaded_image;
        } else {
            *image_RGB = (RGBPixel **)loaded_image;
        }
    } else {
        // If loading failed, reset memory
        if (*image) {
            for (int i = 0; i < *height; i++) {
                free((*image)[i]);
            }
            free(*image);
            *image = NULL;
        }
        if (*image_RGB) {
            for (int i = 0; i < *height; i++) {
                free((*image_RGB)[i]);
            }
            free(*image_RGB);
            *image_RGB = NULL;
        }
    }

    // Initialize selection coordinates to cover the entire image
    *x1 = 0;
    *y1 = 0;
    *x2 = *width;
    *y2 = *height;
}

// Function to parse and display a histogram based on the image
void parse_histogram(char *command, unsigned char **image, RGBPixel **image_RGB, int type, int x1, int y1, int x2, int y2) {
    if (!image && !image_RGB) {
        printf("No image loaded\n");
    } else {
        // Extract parameters for histogram
        char *token = strtok(command, "\n ");
        token = strtok(NULL, "\n ");
        if (!token) {
            printf("Invalid command\n");
            return;
        }
        if ((token[0] < '0' || token[0] > '9')) {
            printf("Invalid command\n");
            return;
        }
        int x = atoi(token);
        token = strtok(NULL, "\n ");
        if (!token) {
            printf("Invalid command\n");
            return;
        }
        if ((token[0] < '0' || token[0] > '9')) {
            printf("Invalid command\n");
            return;
        }
        int y = atoi(token);
        token = strtok(NULL, "\n ");
        if (token) {
            printf("Invalid command\n");
            return;
        }
        if (type == 1 || type == 3) {
            printf("A black and white image is required.\n");
        } else if (y != 2 && y != 4 && y != 8 && y != 16 && y != 32 && y != 64 && y != 128 && y != 256) {
            printf("Invalid parameter set.\n");
        } else {
            print_histogram(image, x1, y1, x2, y2, x, y); // Display histogram
        }
    }
}

// Function to parse and manage the SELECT command for specific coordinates
void parse_select(char *command, unsigned char **image, RGBPixel **image_RGB, int *x1, int *y1, int *x2, int *y2, int width, int height) {
    if (!image && !image_RGB) {
        printf("No image loaded\n");
    } else {
        // Retrieve coordinates from the command
        char *token = strtok(command, "\n ");
        token = strtok(NULL, "\n ");
        if (!token) {
            printf("Invalid command\n");
            return;
        }
        if ((token[0] < '0' || token[0] > '9') && token[0] != '-') {
            printf("Invalid command\n");
            return;
        }
        int new_x1 = atoi(token);
        token = strtok(NULL, "\n ");
        if (!token) {
            printf("Invalid command\n");
            return;
        }
        if ((token[0] < '0' || token[0] > '9') && token[0] != '-') {
            printf("Invalid command\n");
            return;
        }
        int new_y1 = atoi(token);
        token = strtok(NULL, "\n ");
        if (!token) {
            printf("Invalid command\n");
            return;
        }
        if ((token[0] < '0' || token[0] > '9') && token[0] != '-') {
            printf("Invalid command\n");
            return;
        }
        int new_x2 = atoi(token);
        token = strtok(NULL, "\n ");
        if (!token) {
            printf("Invalid command\n");
            return;
        }
        if ((token[0] < '0' || token[0] > '9') && token[0] != '-') {
            printf("Invalid command\n");
            return;
        }
        int new_y2 = atoi(token);

        // Check for valid coordinates
        if (new_x1 < 0 || new_x1 > width || new_x2 < 0 || new_x2 > width ||
            new_y1 < 0 || new_y1 > height || new_y2 < 0 || new_y2 > height ||
            new_x1 == new_x2 || new_y1 == new_y2) {
            printf("Invalid set of coordinates\n");
        } else {
            select_coords(x1, y1, x2, y2, new_x1, new_y1, new_x2, new_y2);
            printf("Selected %d %d %d %d\n", *x1, *y1, *x2, *y2);
        }
    }
}

// Function to parse and execute a ROTATE command for the image
void parse_rotate(char *command, unsigned char ***image, RGBPixel ***image_RGB, int type, int x1, int y1, int *x2, int *y2, int *width, int *height) {
    if (!(*image) && !(*image_RGB)) {
        printf("No image loaded\n");
    } else if (*x2 - x1 != *y2 - y1 && !(x1 == 0 && y1 == 0 && *x2 == *width && *y2 == *height)) {
        printf("The selection must be square\n");
    } else {
        // Extract the angle
        char *token = strtok(command, "\n ");
        token = strtok(NULL, "\n ");
        if (!token) {
            printf("Invalid command\n");
            return;
        }
        int angle = atoi(token);
        // Validate angle
        if (angle % 90 != 0 || angle > 360 || angle < -360) {
            printf("Unsupported rotation angle\n");
        } else {
            rotate_image(image, image_RGB, type, x1, y1, x2, y2, width, height, angle);
            printf("Rotated %d degrees\n", angle);
        }
    }
}

// Function to parse and execute an APPLY command
void parse_apply(char *command, unsigned char **image, RGBPixel **image_RGB, int type, int x1, int y1, int x2, int y2, int width, int height) {
    if (!image && !image_RGB) {
        printf("No image loaded\n");
        return;
    }
    // Extract effect type from command
    char *token = strtok(command, "\n ");
    token = strtok(NULL, "\n ");
    if (!token) {
        printf("Invalid command\n");
    } else if (strcmp(token, "EDGE") != 0 && strcmp(token, "SHARPEN") != 0 && strcmp(token, "BLUR") != 0 && strcmp(token, "GAUSSIAN_BLUR") != 0) {
        printf("APPLY parameter invalid\n");
    } else if (type == 0 || type == 2) {
        printf("Effects can only be applied to color images.\n");
    } else {
        apply_effect(image_RGB, x1, y1, x2, y2, width, height, token);
        printf("Effect %s applied.\n", token);
    }
}

// Main function to handle user interaction and execute commands
int main(void) {
    char command[100];
    unsigned char **image = NULL;
    RGBPixel **image_RGB = NULL;
    int width = 0, height = 0, x1 = 0, y1 = 0, x2 = 0, y2 = 0, type = 0; // Image types: 0 for P2, 1 for P3, 2 for P5, 3 for P6

    // Read commands from input
    while (1) {
        fgets(command, sizeof(command), stdin);
        if (strncmp(command, "EXIT", 4) == 0) break; // Exit command
        else if (strncmp(command, "LOAD", 4) == 0) {
            parse_load(command, &image, &image_RGB, &width, &height, &type, &x1, &y1, &x2, &y2);
        } else if (strncmp(command, "SELECT ALL", 10) == 0) {
            if (!image && !image_RGB) {
                printf("No image loaded\n");
            } else {
                x1 = 0;
                y1 = 0;
                x2 = width;
                y2 = height;
                printf("Selected ALL\n");
            }
        } else if (strncmp(command, "SELECT", 6) == 0) {
            parse_select(command, image, image_RGB, &x1, &y1, &x2, &y2, width, height);
        } else if (strncmp(command, "ROTATE", 6) == 0) {
            parse_rotate(command, &image, &image_RGB, type, x1, y1, &x2, &y2, &width, &height);
        } else if (strncmp(command, "EQUALIZE", 8) == 0) {
            equalize(image, image_RGB, type, width, height);
        } else if (strncmp(command, "CROP", 4) == 0) {
            if (!image && !image_RGB) {
                printf("No image loaded\n");
            } else {
                crop(&image, &image_RGB, type, x1, y1, x2, y2, &width, &height);
                x1 = 0;
                y1 = 0;
                x2 = width;
                y2 = height;
                printf("Image cropped\n");
            }
        } else if (strncmp(command, "APPLY", 5) == 0) {
            parse_apply(command, image, image_RGB, type, x1, y1, x2, y2, width, height);
        } else if (strncmp(command, "HISTOGRAM", 9) == 0) {
            parse_histogram(command, image, image_RGB, type, x1, y1, x2, y2);
        } else if (strncmp(command, "SAVE", 4) == 0) {
            if (!image && !image_RGB) {
                printf("No image loaded\n");
            } else {
                char *token = strtok(command, "\n ");
                token = strtok(NULL, "\n ");
                if (strtok(NULL, "\n ")) {
                    save_ascii(image, image_RGB, type, width, height, token);
                } else {
                    save_binary(image, image_RGB, type, width, height, token);
                }
                printf("Saved %s\n", token);
            }
        } else {
            printf("Invalid command\n");
        }
    }

    // Cleanup memory before exiting
    if (!image && !image_RGB) {
        printf("No image loaded\n");
    }
    if (image) {
        for (int i = 0; i < height; i++) {
            free(image[i]);
        }
        free(image);
    }
    if (image_RGB) {
        for (int i = 0; i < height; i++) {
            free(image_RGB[i]);
        }
        free(image_RGB);
    }
    return 0;
}