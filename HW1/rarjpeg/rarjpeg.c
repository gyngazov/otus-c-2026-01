#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define EOCD_MIN_LEN 22
#define EOCD_SIGNATURE 0x06054b50
#define LOCAL_FILE_HEADER_SIGNATURE 0x04034b50
#define SHORT_FILE_LEN 4L

struct CDFH {
    uint32_t signature;
    uint16_t versionMadeBy;
    uint16_t versionToExtract;
    uint16_t generalPurposeBitFlag;
    uint16_t compressionMethod;
    uint16_t modificationTime;
    uint16_t modificationDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t filenameLength;
    uint16_t extraFieldLength;
    uint16_t fileCommentLength;
    uint16_t diskNumber;
    uint16_t internalFileAttributes;
    uint32_t externalFileAttributes;
    uint32_t localFileHeaderOffset;
} __attribute__((packed));

struct EOCD {
    uint16_t diskNumber;
    uint16_t startDiskNumber;
    uint16_t numberCentralDirectoryRecord;
    uint16_t totalCentralDirectoryRecord;
    uint32_t sizeOfCentralDirectory;
    uint32_t centralDirectoryOffset;
    uint16_t commentLength;
} __attribute__((packed));

long eocd_offset(FILE *fp);
bool is_jpeg_begun(FILE *fp);
bool is_jpeg_end(FILE *fp);
bool is_short(FILE *fp); 
bool is_zip_begun(FILE *fp);
void collect_file_names(FILE *fp, long eocd_offset, long jpeg_offset);
void get_file_name(FILE *fp);
uint32_t get32(FILE *fp);
long get_jpeg_end(FILE *fp);
int finish(int ret_val, char *descr, FILE *fp);
int bad_file(FILE *fp);
int main (int argc, char** argv)
{
    if (argc != 2) {
        printf("usage: %s input_file\n", argv[0]);
        return 8;
    }
    FILE *fp;
	fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        perror("Error opening input file");
        return 7;
    }
    if (is_short(fp)) 
        return bad_file(fp);
    bool is_jpeg_start = is_jpeg_begun(fp);
    if (is_jpeg_end(fp) && is_jpeg_start)
        return finish(5, "файл jpeg", fp);
    long offset = eocd_offset(fp); 
    if (offset == -1L) 
        return bad_file(fp);
    if (is_jpeg_start) {
        long jpeg_end = get_jpeg_end(fp);
        if (LOCAL_FILE_HEADER_SIGNATURE == get32(fp)) {
            collect_file_names(fp, offset, jpeg_end);
            return finish(4, "файл rarjpeg", fp);
        } else
            return bad_file(fp);
    } else if (is_zip_begun(fp)) {
        collect_file_names(fp, offset, 0);
        return finish(3, "файл архив", fp);
    } else 
        return bad_file(fp);
}

int bad_file(FILE *fp)
{
    return finish(6, "файл не rarjpeg", fp);
}
// определить совсем короткие файлы
bool is_short(FILE *fp)
{
    fseek(fp, 0, SEEK_END);
    return ftell(fp) < SHORT_FILE_LEN;
}
// поискать eocd в конце файла
// предположить eocd без комента
// вернуть офсет после сигнатуры
long eocd_offset(FILE *fp) 
{
    fseek(fp, 0, SEEK_END);
    size_t zip_len = ftell(fp);
    for (size_t offset = zip_len - EOCD_MIN_LEN; offset != 0; offset--) {
        fseek(fp, offset, SEEK_SET);
        if (EOCD_SIGNATURE == get32(fp)) {
            return ftell(fp);
        }
    }
    return -1L;
}
// получить офсет первого cdfh и их количество 
// обойти все cdfh
// извлекать имя файла
// eocd_offset запомнили после поиска eocd->signature
void collect_file_names(FILE *fp, long eocd_offset, long jpeg_offset)
{
    fseek(fp, eocd_offset, SEEK_SET);
    struct EOCD *eocd = (struct EOCD *) malloc(sizeof(struct EOCD));
    fread((char *) eocd, 1, sizeof(struct EOCD), fp);
    uint16_t ncdr = eocd->numberCentralDirectoryRecord;
    uint32_t cdo = eocd->centralDirectoryOffset;
    free(eocd);
    // jpeg_offset надо отмотать до начала zip
    fseek(fp, jpeg_offset + cdo, SEEK_SET);
    printf("Список файлов архива:\n");
    for (int i = 0; i < ncdr; i++)
        get_file_name(fp);    
}
// имя файла:
// - найти в cdr
// - вычитать
// - печатать
// дочитать cdr до конца
void get_file_name(FILE *fp)
{
    struct CDFH *cdfh = (struct CDFH *) malloc(sizeof(struct CDFH));
    fread((char *) cdfh, 1, sizeof(struct CDFH), fp);
    uint16_t file_len = cdfh->filenameLength;
    uint16_t extra_len = cdfh->extraFieldLength;
    uint16_t comm_len = cdfh->fileCommentLength;
    free(cdfh);
    if (file_len) {
        char file_name[file_len];
        fread(file_name, 1, file_len, fp);
        printf("%s\n", file_name);
    }
    if (extra_len)
        fseek(fp, extra_len, SEEK_CUR);
    if (comm_len)
        fseek(fp, comm_len, SEEK_CUR);
}

// если FFD8 в начале, то начался jpeg
bool is_jpeg_begun(FILE *fp) 
{
    fseek(fp, 0, SEEK_SET);
    return fgetc(fp) == 255 && fgetc(fp) == 216;
}

// если FFD9 в конце, то весь файл - jpeg
bool is_jpeg_end(FILE *fp)
{
    fseek(fp, -2L, SEEK_END);
    return fgetc(fp) == 255 && fgetc(fp) == 217;
}
long get_jpeg_end(FILE *fp)
{
    fseek(fp, 0, SEEK_SET);
    while (!(fgetc(fp) == 255 && fgetc(fp) == 217));
    return ftell(fp);
}
// начался ли файл , как зип
bool is_zip_begun(FILE *fp)
{
    fseek(fp, 0, SEEK_SET);
    return LOCAL_FILE_HEADER_SIGNATURE == get32(fp);
}

int finish(int ret_val, char *descr, FILE *fp)
{
    fclose(fp);
    printf("%s\n", descr);
    return ret_val;
}

uint32_t get32(FILE *fp)
{
    uint32_t value = 0;
    fread((unsigned char *) &value, 1, 4, fp);
    return value;
}

