#include <stdio.h> 
#include <unistd.h> 

void main(int argc, char *argv[]) 
{
    int opt;
    while((opt = getopt(argc, argv, "cf")) != -1) { 
        switch(opt) { 
            case 'c':
                printf("count: %s\n", optarg); 
                break;
            case 'f':
                printf("filename: %s\n", optarg); 
                break;
            default: 
                printf("unknown option: %c\n", opt);
                printf("unknown arg: %s\n", optarg);
                break; 
        } 
    }
} 