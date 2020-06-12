#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <algorithm>


using namespace std;

int a, widht, height, gamma;

void convert_to_y(unsigned char* pix){
    int ind_r = 0;
    int ind_g = 1;
    int ind_b = 2;
    while (ind_r < 3 * widht * height) {
        double y = 0.299 * double(pix[ind_r]) + 0.587 * double(pix[ind_g]) + 0.114 * double(pix[ind_b]);
        double cb = 128. - 0.168736 * double(pix[ind_r]) - 0.331264 * double(pix[ind_g]) + 0.5 * double(pix[ind_b]);
        double cr = 128. + 0.5 * double(pix[ind_r]) - 0.418688 * double(pix[ind_g]) - 0.081312 * double(pix[ind_b]);
        pix[ind_r] = (unsigned char)(min(max(round(y), 0.), 255.));
        pix[ind_g] = (unsigned char)(min(max(round(cb), 0.), 255.));
        pix[ind_b] = (unsigned char)(min(max(round(cr), 0.), 255.));
        ind_r += 3; ind_g += 3; ind_b += 3;
    }
}

void convert_back_to_rgb(unsigned char* pix) {
    int ind_y = 0;
    int ind_cb = 1;
    int ind_cr = 2;
    while (ind_y < 3 * widht * height) {
        double r = double(pix[ind_y]) +  1.402  * (pix[ind_cr] - 128.);
        double g = double(pix[ind_y]) -  0.34414 * double(pix[ind_cb] - 128.) - 0.71414  * double(pix[ind_cr] - 128.);
        double b = double(pix[ind_y]) +  1.772 * double(pix[ind_cb] - 128.);
        pix[ind_y] = (unsigned char)(min(max(round(r), 0.), 255.));
        pix[ind_cb] = (unsigned char)(min(max(round(g), 0.), 255.));
        pix[ind_cr] = (unsigned char)(min(max(round(b), 0.), 255.));
        ind_y += 3; ind_cb += 3; ind_cr += 3;
    }

}

void action (int offset, double multiplier, int number_action, unsigned char* arr_pixels) {
    if (a == 5) {
        for (int i = 0; i < widht * height; ++i) {
            double y = multiplier * (arr_pixels[i] - offset);
            arr_pixels[i] = (unsigned char) min(max(y, 0.), 255.);
        }
    } else {
        if (number_action == 1) {
            convert_to_y(arr_pixels);
        }
        for (int i = 0; i < 3 * widht * height; ++i) {
            double y = multiplier * (arr_pixels[i] - offset);
            arr_pixels[i] = (unsigned char) min(max(y, 0.), 255.);
        }
        if (number_action == 1) {
            convert_back_to_rgb(arr_pixels);
        }
    }
}

pair<double, double> min_max(const unsigned char* mas, int sz) {
    double maxx = mas[0]; double minn = mas[0];
    for (int i = 0; i < sz; ++i) {
        if (double(mas[i]) > maxx) {
            maxx = double(mas[i]);
        }
        if (double(mas[i]) < minn) {
            minn = double(mas[i]);
        }
    }
    cout << minn << ' ' << 255. / double(maxx - minn);
    return {minn, maxx};
}

void action_1 (int number_action, unsigned char* arr_pixels) {
    if (a == 5) {
        auto min_max5 = min_max(arr_pixels, widht * height);
        if (number_action == 2 || number_action == 3) {
            for (int i = 0; i < widht * height; ++i) {
                double y = (arr_pixels[i] - min_max5.first) * 255 / (min_max5.second - min_max5.first);
                arr_pixels[i] = (unsigned char) min(max(y, 0.), 255.);
            }
        }
    } else { // a = 6
        if (number_action == 3) {
            convert_to_y(arr_pixels);
        }
        auto min_max6 = min_max(arr_pixels, 3 * widht * height);
        for (int i = 0; i < 3 * widht * height; ++i) {
            double y = (arr_pixels[i] - min_max6.first) * 255 / (min_max6.second - min_max6.first);
            arr_pixels[i] = (unsigned char) min(max(y, 0.), 255.);
        }
        if (number_action == 3) {
            convert_back_to_rgb(arr_pixels);
        }
    }
}

int main(int argc, char * argv[]) {
    if ((argc < 4) || (argc > 6)){
        cerr << "input data can't be accepted \n";
        return 1;
    }
    FILE * input_file; FILE * output_file;
    input_file = fopen(argv[1], "rb");
    if (input_file == NULL){ //не удалось открыть файл
        cerr << "the file was not found \n";
        return 1;
    }

    if (fscanf(input_file, "P%i%i%i%i\n", &a, &widht, &height, &gamma) != 4) {
        cerr << "invalid file \n"; //нет 4х аргументов файла
        fclose(input_file);
        return 1;
    }

    if ((a != 6) && (a != 5)) {
        cerr << "unsupported type of file \n";
        fclose(input_file);
        return 1;
    }
    unsigned char * arr_pixels;
    if (a == 5) {
        arr_pixels = (unsigned char *) malloc(sizeof(unsigned char) * widht * height);
        if (arr_pixels == NULL) {
            cerr << "allocation memory failed \n";
            fclose(input_file);
            free(arr_pixels);
            return 1;
        }
        if (fread(arr_pixels, sizeof(unsigned char), widht * height, input_file) != widht * height){
            cerr << "incorrect size of data in input file \n"; //некоректный размер массива данных
            fclose(input_file);
            free(arr_pixels);
            return 1;
        }
    } else {
        arr_pixels = (unsigned char *) malloc(sizeof(unsigned char) * 3 *widht * height);
        if (arr_pixels == NULL) {
            cerr << "allocation memory failed \n";
            fclose(input_file);
            free(arr_pixels);
            return 1;
        }
        if (fread(arr_pixels, sizeof(unsigned char),  3 * widht * height, input_file) != 3 * widht * height){
            cerr << "incorrect size of data in input file \n"; //некоректный размер массива данных
            fclose(input_file);
            free(arr_pixels);
            return 1;
        }
    }

    int number_action = atoi(argv[3]);
    int offset; double multiplier;

    if (number_action == 0 || number_action == 1) {
        if (argc != 6) {
            cerr << "input data can't be accepted \n"; return 1;
        }
        offset = atoi(argv[4]);
        multiplier = atof(argv[5]);
    } else {
        if (argc != 4) {
            cerr << "input data can't be accepted \n"; return 1;
        }
    }
    if (number_action == 0 || number_action == 1) {
        action(offset, multiplier, number_action, arr_pixels);
    } else {
        action_1(number_action, arr_pixels);
    }

    output_file = fopen(argv[2], "wb");
    if (output_file == NULL){
        cerr << "the output file was not found \n"; //не удалось открыть файл
        fclose(input_file);
        free(arr_pixels);
        return 1;
    }

    fprintf(output_file, "P%i\n%i %i\n%i\n", a, widht, height, gamma);
    if (a == 5) {
        fwrite(arr_pixels, sizeof(unsigned char), widht * height, output_file);
    } else {
        fwrite(arr_pixels, sizeof(unsigned char), 3 * widht * height, output_file);
    }
    free(arr_pixels);
    fclose(output_file);
    fclose(input_file);
    return 0;
}