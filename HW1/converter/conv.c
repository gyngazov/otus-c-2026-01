
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define CP "CP-1251"
#define KOI "KOI8-R"
#define ISO "ISO-8859-5"

size_t codepoint_to_utf8(uint32_t cp, unsigned char* buf);
uint32_t *cp_koi();
uint32_t *cp_cp();
uint32_t *cp_iso();
void f_close(FILE *fp);
size_t f_write(unsigned char *buff, size_t size, size_t nmemb, FILE *fp);

int main(int argc, char **argv)
{
    if (argc != 4) {
        printf("usage: %s input_file code_page output_file\n", argv[0]);
        return 4;
    }
    FILE *input_fp = fopen(argv[1], "rb");
    if (input_fp == NULL) {
        perror("Error opening input file");
        return 1;
    }
    FILE *fp;
    fp = fopen(argv[3], "wb");
    if (fp == NULL) {
        perror("Error opening output file");
        f_close(input_fp);
        return 2;
    }
    char* cpage = argv[2];
    uint32_t *code_page;
    if (strcmp(cpage, KOI) == 0)
        code_page = cp_koi();
    else if (strcmp(cpage, CP) == 0)
        code_page = cp_cp();
    else if (strcmp(cpage, ISO) == 0)
        code_page = cp_iso();
    else {
        printf("acceptable code pages: CP-1251, KOI8-R, ISO-8859-5\n");
        return 3;
    }
    unsigned char utf8_buffer[4];
    size_t len;
    int byte;
    while ((byte = fgetc(input_fp)) != EOF) {
        if (byte < 128) 
            fputc((unsigned char) byte, fp);
        else {
            len = codepoint_to_utf8(code_page[byte], utf8_buffer);
            f_write(utf8_buffer, 1, len, fp);
        }
    }
    f_close(fp);
    f_close(input_fp);

    return 0;
}

size_t f_write(unsigned char *buff, size_t size, size_t nmemb, FILE *fp)
{
    size_t written = fwrite(buff, size, nmemb, fp);
    int errnum = ferror(fp);
    if (errnum != 0) {
        printf("Ошибка %d\n", errnum);
        exit(errnum);
    } else if (written != nmemb) {
        printf("Ошибка записи.");
        exit(101);
    }
    return written;
}

void f_close(FILE *fp)
{
    if (fclose(fp) != 0) {
        printf("%s\n", strerror(errno));
        exit(errno);
    }
}

// преобразовать коде пойнт в массив байт utf8
size_t codepoint_to_utf8(uint32_t cp, unsigned char* buffer) {
    if (cp <= 0x7F) {
        buffer[0] = (unsigned char) cp;
        return 1;
    } else if (cp <= 0x7FF) {
        buffer[0] = (unsigned char)(0xC0 | (cp >> 6));
        buffer[1] = (unsigned char)(0x80 | (cp & 0x3F));
        return 2;
    } else if (cp <= 0xFFFF) {
        if (cp >= 0xD800 && cp <= 0xDFFF) return 0;
        buffer[0] = (unsigned char)(0xE0 | (cp >> 12));
        buffer[1] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
        buffer[2] = (unsigned char)(0x80 | (cp & 0x3F));
        return 3;
    } else if (cp <= 0x10FFFF) {
        buffer[0] = (unsigned char)(0xF0 | (cp >> 18));
        buffer[1] = (unsigned char)(0x80 | ((cp >> 12) & 0x3F));
        buffer[2] = (unsigned char)(0x80 | ((cp >> 6) & 0x3F));
        buffer[3] = (unsigned char)(0x80 | (cp & 0x3F));
        return 4;
    } else {
        return 0;
    }
}
//кодовые страницы
//сопоставлен байт и кодепойнт для данной кодировки
//байт перекинуть в инт
//найти в массиве КП по данному инту
uint32_t *cp_koi()
{
    uint32_t *code_page = malloc(sizeof(uint32_t) * 256);
    code_page[128] = 0x2500;
    code_page[129] = 0x2502;
    code_page[130] = 0x250C;
    code_page[131] = 0x2510;
    code_page[132] = 0x2514;
    code_page[133] = 0x2518;
    code_page[134] = 0x251C;
    code_page[135] = 0x2524;
    code_page[136] = 0x252C;
    code_page[137] = 0x2534;
    code_page[138] = 0x253C;
    code_page[139] = 0x2580;
    code_page[140] = 0x2584;
    code_page[141] = 0x2588;
    code_page[142] = 0x258C;
    code_page[143] = 0x2590;
    code_page[144] = 0x2591;
    code_page[145] = 0x2592;
    code_page[146] = 0x2593;
    code_page[147] = 0x2320;
    code_page[148] = 0x25A0;
    code_page[149] = 0x2219;
    code_page[150] = 0x221A;
    code_page[151] = 0x2248;
    code_page[152] = 0x2264;
    code_page[153] = 0x2265;
    code_page[154] = 0xA0;
    code_page[155] = 0x2321;
    code_page[156] = 0xB0;
    code_page[157] = 0xB2;
    code_page[158] = 0xB7;
    code_page[159] = 0xF7;
    code_page[160] = 0x2550;
    code_page[161] = 0x2551;
    code_page[162] = 0x2552;
    code_page[163] = 0x451;
    code_page[164] = 0x2553;
    code_page[165] = 0x2554;
    code_page[166] = 0x2555;
    code_page[167] = 0x2556;
    code_page[168] = 0x2557;
    code_page[169] = 0x2558;
    code_page[170] = 0x2559;
    code_page[171] = 0x255A;
    code_page[172] = 0x255B;
    code_page[173] = 0x255C;
    code_page[174] = 0x255D;
    code_page[175] = 0x255E;
    code_page[176] = 0x255F;
    code_page[177] = 0x2560;
    code_page[178] = 0x2561;
    code_page[179] = 0x401;
    code_page[180] = 0x2562;
    code_page[181] = 0x2563;
    code_page[182] = 0x2564;
    code_page[183] = 0x2565;
    code_page[184] = 0x2566;
    code_page[185] = 0x2567;
    code_page[186] = 0x2568;
    code_page[187] = 0x2569;
    code_page[188] = 0x256A;
    code_page[189] = 0x256B;
    code_page[190] = 0x256C;
    code_page[191] = 0xA9;
    code_page[192] = 0x44E;
    code_page[193] = 0x430;
    code_page[194] = 0x431;
    code_page[195] = 0x446;
    code_page[196] = 0x434;
    code_page[197] = 0x435;
    code_page[198] = 0x444;
    code_page[199] = 0x433;
    code_page[200] = 0x445;
    code_page[201] = 0x438;
    code_page[202] = 0x439;
    code_page[203] = 0x43A;
    code_page[204] = 0x43B;
    code_page[205] = 0x43C;
    code_page[206] = 0x43D;
    code_page[207] = 0x43E;
    code_page[208] = 0x43F;
    code_page[209] = 0x44F;
    code_page[210] = 0x440;
    code_page[211] = 0x441;
    code_page[212] = 0x442;
    code_page[213] = 0x443;
    code_page[214] = 0x436;
    code_page[215] = 0x432;
    code_page[216] = 0x44C;
    code_page[217] = 0x44B;
    code_page[218] = 0x437;
    code_page[219] = 0x448;
    code_page[220] = 0x44D;
    code_page[221] = 0x449;
    code_page[222] = 0x447;
    code_page[223] = 0x44A;
    code_page[224] = 0x42E;
    code_page[225] = 0x410;
    code_page[226] = 0x411;
    code_page[227] = 0x426;
    code_page[228] = 0x414;
    code_page[229] = 0x415;
    code_page[230] = 0x424;
    code_page[231] = 0x413;
    code_page[232] = 0x425;
    code_page[233] = 0x418;
    code_page[234] = 0x419;
    code_page[235] = 0x41A;
    code_page[236] = 0x41B;
    code_page[237] = 0x41C;
    code_page[238] = 0x41D;
    code_page[239] = 0x41E;
    code_page[240] = 0x41F;
    code_page[241] = 0x42F;
    code_page[242] = 0x420;
    code_page[243] = 0x421;
    code_page[244] = 0x422;
    code_page[245] = 0x423;
    code_page[246] = 0x416;
    code_page[247] = 0x412;
    code_page[248] = 0x42C;
    code_page[249] = 0x42B;
    code_page[250] = 0x417;
    code_page[251] = 0x428;
    code_page[252] = 0x42D;
    code_page[253] = 0x429;
    code_page[254] = 0x427;
    code_page[255] = 0x42A;
    return code_page;
}
uint32_t *cp_cp()
{
    uint32_t *code_page = malloc(sizeof(uint32_t) * 256);
    code_page[128] = 0x0402;
    code_page[129] = 0x0403;
    code_page[130] = 0x201A;
    code_page[131] = 0x0453;
    code_page[132] = 0x201E;
    code_page[133] = 0x2026;
    code_page[134] = 0x2020;
    code_page[135] = 0x2021;
    code_page[136] = 0x20AC;
    code_page[137] = 0x2030;
    code_page[138] = 0x0409;
    code_page[139] = 0x2039;
    code_page[140] = 0x040A;
    code_page[141] = 0x040C;
    code_page[142] = 0x040B;
    code_page[143] = 0x040F;
    code_page[144] = 0x0452;
    code_page[145] = 0x2018;
    code_page[146] = 0x2019;
    code_page[147] = 0x201C;
    code_page[148] = 0x201D;
    code_page[149] = 0x2022;
    code_page[150] = 0x2013;
    code_page[151] = 0x2014;
    code_page[152] = 0xFFFD;
    code_page[153] = 0x2122;
    code_page[154] = 0x0459;
    code_page[155] = 0x203A;
    code_page[156] = 0x045A;
    code_page[157] = 0x045C;
    code_page[158] = 0x045B;
    code_page[159] = 0x045F;
    code_page[160] = 0x00A0;
    code_page[161] = 0x040E;
    code_page[162] = 0x045E;
    code_page[163] = 0x0408;
    code_page[164] = 0x00A4;
    code_page[165] = 0x0490;
    code_page[166] = 0x00A6;
    code_page[167] = 0x00A7;
    code_page[168] = 0x0401;
    code_page[169] = 0x00A9;
    code_page[170] = 0x0404;
    code_page[171] = 0x00AB;
    code_page[172] = 0x00AC;
    code_page[173] = 0x00AD;
    code_page[174] = 0x00AE;
    code_page[175] = 0x0407;
    code_page[176] = 0x00B0;
    code_page[177] = 0x00B1;
    code_page[178] = 0x0406;
    code_page[179] = 0x0456;
    code_page[180] = 0x0491;
    code_page[181] = 0x00B5;
    code_page[182] = 0x00B6;
    code_page[183] = 0x00B7;
    code_page[184] = 0x0451;
    code_page[185] = 0x2116;
    code_page[186] = 0x0454;
    code_page[187] = 0x00BB;
    code_page[188] = 0x0458;
    code_page[189] = 0x0405;
    code_page[190] = 0x0455;
    code_page[191] = 0x0457;
    code_page[192] = 0x0410;
    code_page[193] = 0x0411;
    code_page[194] = 0x0412;
    code_page[195] = 0x0413;
    code_page[196] = 0x0414;
    code_page[197] = 0x0415;
    code_page[198] = 0x0416;
    code_page[199] = 0x0417;
    code_page[200] = 0x0418;
    code_page[201] = 0x0419;
    code_page[202] = 0x041A;
    code_page[203] = 0x041B;
    code_page[204] = 0x041C;
    code_page[205] = 0x041D;
    code_page[206] = 0x041E;
    code_page[207] = 0x041F;
    code_page[208] = 0x0420;
    code_page[209] = 0x0421;
    code_page[210] = 0x0422;
    code_page[211] = 0x0423;
    code_page[212] = 0x0424;
    code_page[213] = 0x0425;
    code_page[214] = 0x0426;
    code_page[215] = 0x0427;
    code_page[216] = 0x0428;
    code_page[217] = 0x0429;
    code_page[218] = 0x042A;
    code_page[219] = 0x042B;
    code_page[220] = 0x042C;
    code_page[221] = 0x042D;
    code_page[222] = 0x042E;
    code_page[223] = 0x042F;
    code_page[224] = 0x0430;
    code_page[225] = 0x0431;
    code_page[226] = 0x0432;
    code_page[227] = 0x0433;
    code_page[228] = 0x0434;
    code_page[229] = 0x0435;
    code_page[230] = 0x0436;
    code_page[231] = 0x0437;
    code_page[232] = 0x0438;
    code_page[233] = 0x0439;
    code_page[234] = 0x043A;
    code_page[235] = 0x043B;
    code_page[236] = 0x043C;
    code_page[237] = 0x043D;
    code_page[238] = 0x043E;
    code_page[239] = 0x043F;
    code_page[240] = 0x0440;
    code_page[241] = 0x0441;
    code_page[242] = 0x0442;
    code_page[243] = 0x0443;
    code_page[244] = 0x0444;
    code_page[245] = 0x0445;
    code_page[246] = 0x0446;
    code_page[247] = 0x0447;
    code_page[248] = 0x0448;
    code_page[249] = 0x0449;
    code_page[250] = 0x044A;
    code_page[251] = 0x044B;
    code_page[252] = 0x044C;
    code_page[253] = 0x044D;
    code_page[254] = 0x044E;
    code_page[255] = 0x044F;
    return code_page;
}
uint32_t *cp_iso()
{
    uint32_t *code_page = malloc(sizeof(uint32_t) * 256);
    code_page[128] = 0x0080;
    code_page[129] = 0x0081;
    code_page[130] = 0x0082;
    code_page[131] = 0x0083;
    code_page[132] = 0x0084;
    code_page[133] = 0x0085;
    code_page[134] = 0x0086;
    code_page[135] = 0x0087;
    code_page[136] = 0x0088;
    code_page[137] = 0x0089;
    code_page[138] = 0x008A;
    code_page[139] = 0x008B;
    code_page[140] = 0x008C;
    code_page[141] = 0x008D;
    code_page[142] = 0x008E;
    code_page[143] = 0x008F;
    code_page[144] = 0x0090;
    code_page[145] = 0x0091;
    code_page[146] = 0x0092;
    code_page[147] = 0x0093;
    code_page[148] = 0x0094;
    code_page[149] = 0x0095;
    code_page[150] = 0x0096;
    code_page[151] = 0x0097;
    code_page[152] = 0x0098;
    code_page[153] = 0x0099;
    code_page[154] = 0x009A;
    code_page[155] = 0x009B;
    code_page[156] = 0x009C;
    code_page[157] = 0x009D;
    code_page[158] = 0x009E;
    code_page[159] = 0x009F;
    code_page[160] = 0x00A0;
    code_page[161] = 0x0401;
    code_page[162] = 0x0402;
    code_page[163] = 0x0403;
    code_page[164] = 0x0404;
    code_page[165] = 0x0405;
    code_page[166] = 0x0406;
    code_page[167] = 0x0407;
    code_page[168] = 0x0408;
    code_page[169] = 0x0409;
    code_page[170] = 0x040A;
    code_page[171] = 0x040B;
    code_page[172] = 0x040C;
    code_page[173] = 0x00AD;
    code_page[174] = 0x040E;
    code_page[175] = 0x040F;
    code_page[176] = 0x0410;
    code_page[177] = 0x0411;
    code_page[178] = 0x0412;
    code_page[179] = 0x0413;
    code_page[180] = 0x0414;
    code_page[181] = 0x0415;
    code_page[182] = 0x0416;
    code_page[183] = 0x0417;
    code_page[184] = 0x0418;
    code_page[185] = 0x0419;
    code_page[186] = 0x041A;
    code_page[187] = 0x041B;
    code_page[188] = 0x041C;
    code_page[189] = 0x041D;
    code_page[190] = 0x041E;
    code_page[191] = 0x041F;
    code_page[192] = 0x0420;
    code_page[193] = 0x0421;
    code_page[194] = 0x0422;
    code_page[195] = 0x0423;
    code_page[196] = 0x0424;
    code_page[197] = 0x0425;
    code_page[198] = 0x0426;
    code_page[199] = 0x0427;
    code_page[200] = 0x0428;
    code_page[201] = 0x0429;
    code_page[202] = 0x042A;
    code_page[203] = 0x042B;
    code_page[204] = 0x042C;
    code_page[205] = 0x042D;
    code_page[206] = 0x042E;
    code_page[207] = 0x042F;
    code_page[208] = 0x0430;
    code_page[209] = 0x0431;
    code_page[210] = 0x0432;
    code_page[211] = 0x0433;
    code_page[212] = 0x0434;
    code_page[213] = 0x0435;
    code_page[214] = 0x0436;
    code_page[215] = 0x0437;
    code_page[216] = 0x0438;
    code_page[217] = 0x0439;
    code_page[218] = 0x043A;
    code_page[219] = 0x043B;
    code_page[220] = 0x043C;
    code_page[221] = 0x043D;
    code_page[222] = 0x043E;
    code_page[223] = 0x043F;
    code_page[224] = 0x0440;
    code_page[225] = 0x0441;
    code_page[226] = 0x0442;
    code_page[227] = 0x0443;
    code_page[228] = 0x0444;
    code_page[229] = 0x0445;
    code_page[230] = 0x0446;
    code_page[231] = 0x0447;
    code_page[232] = 0x0448;
    code_page[233] = 0x0449;
    code_page[234] = 0x044A;
    code_page[235] = 0x044B;
    code_page[236] = 0x044C;
    code_page[237] = 0x044D;
    code_page[238] = 0x044E;
    code_page[239] = 0x044F;
    code_page[240] = 0x2116;
    code_page[241] = 0x0451;
    code_page[242] = 0x0452;
    code_page[243] = 0x0453;
    code_page[244] = 0x0454;
    code_page[245] = 0x0455;
    code_page[246] = 0x0456;
    code_page[247] = 0x0457;
    code_page[248] = 0x0458;
    code_page[249] = 0x0459;
    code_page[250] = 0x045A;
    code_page[251] = 0x045B;
    code_page[252] = 0x045C;
    code_page[253] = 0x00A7;
    code_page[254] = 0x045E;
    code_page[255] = 0x045F;
    return code_page;
}
