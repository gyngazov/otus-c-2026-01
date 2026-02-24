char* get_content(FILE *fp)
{
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    char *total = malloc(fsize + 1);
    fseek(fp, 0, SEEK_SET);
    fread(total, fsize, 1, fp);
    fclose(fp);
    total[fsize] = 0;
    return total;
}

