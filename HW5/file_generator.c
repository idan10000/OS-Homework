//
// Created by ido on 08/06/2021.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[]) {
    FILE *source, *target;
    int num, size;
    char *sname, tname[20], *temp;
    sname = "/dev/urandom";
    assert(argc == 3);
    num = atoi(argv[1]);
    size = atoi(argv[2]);
    sprintf(tname, "random%d.bin", num);
    temp = malloc(size);
    source = fopen(sname, "rb");
    printf("done1\n");
    target = fopen(tname, "wb");
    printf("done2\n");
    fread(temp, 1, size, source);
    printf("done3\n");
    fwrite(temp, 1, size, target);
    printf("done4\n");
    fclose(source);
    fclose(target);
    free(temp);
}

//int main(int argc, char *argv[]) {
//    FILE *target;
//    int num, source;
//    char *sname, tname[20], *temp;
//    sname = "/dev/urandom";
//
//    assert(argc == 2);
//    num = atoi(argv[1]);
//    sprintf(tname, "random%d", num);
//
//    source = open("/dev/urandom", O_RDONLY);
//    printf("done1\n");
//    target = fopen(tname, "wb");
//    printf("done2\n");
//    read(temp, 1, 1000, source);
//    printf("done3\n");
//    fwrite(temp, 1, 1000, target);
//    printf("done4\n");
//    fclose(source);
//    fclose(target);
//}