#include <stdio.h>
#include <stdlib.h>

#define BS 0x400

static char const hdu[] = "0123456789ABCDEF";
static char const hdl[] = "0123456789abcdef";

typedef struct opts_s opts_t;
struct opts_s
{
    unsigned int width;
    char help;
    char lowercase;
};

/* parse_opts ***************************************************************/
int parse_opts (opts_t * opts, int argc, char const * const * argv)
{
    int i, j;
    char * t;

    opts->help = 0;
    opts->width = 0;
    opts->lowercase = 0;

    for (i = 1; i < argc; ++i)
    {
        if (argv[i][0] != '-')
        {
            fprintf(stderr, 
                    "invoke error: non-option argument '%s' not accepted\n",
                    argv[i]);
            return 1;
        }
        for (j = 1; argv[i][j]; ++j)
        {
            switch (argv[i][j])
            {
            case 'w':
                if (!argv[i][j + 1]) opts->width = 16;
                else
                {
                    opts->width = strtoul(&argv[i][j + 1], &t, 0);
                    if (*t)
                    {
                        fprintf(stderr,
                                "invoke error: width value trailing chars: %s\n", 
                                argv[i]);
                        return 1;
                    }
                    j = t - argv[i] - 1;
                }
                break;
            case 'h':
                opts->help = 1;
                break;
            case 'l':
                opts->lowercase = 1;
                break;
            case 'u':
                opts->lowercase = 0;
                break;
            default:
                fprintf(stderr,
                        "invoke error: unknown option '%c' in argument '%s'\n",
                        argv[i][j], argv[i]);
                return 1;
            }
        }
    }
    return 0;
}

/* process ******************************************************************/
int process (FILE * fi, FILE * fo, opts_t * opts)
{
    unsigned char bi[BS];
    char bo[BS * 3];
    size_t si, so, ofs;
    int fie;
    char const * hd;

    ofs = 0;
    hd = opts->lowercase ? hdl : hdu;
    while (!(fie = ferror(fi)) && (si = fread(bi, 1, sizeof(bi), fi)))
    {
        size_t i;
        char * o = &bo[0];
        for (i = 0; i < si; ++i, ++ofs)
        {
            *o++ = hd[bi[i] >> 4];
            *o++ = hd[bi[i] & 15];
            if (opts->width && (ofs + 1) % opts->width == 0) 
            { *o++ = '\n'; ofs = 0; }
        }
        so = o - &bo[0];
        if (fwrite(bo, 1, so, fo) != so)
        {
            fprintf(stderr, "output error!\n");
            return 2;
        }
    }
    if (fie)
    {
        fprintf(stderr, "input error!\n");
        return 1;
    }
    if (opts->width && ofs % opts->width)
    {
        if (fwrite("\n", 1, 1, fo) != 1)
        {
            fprintf(stderr, "output error!\n");
            return 2;
        }
    }
    return 0;
}

/* main *********************************************************************/
int main (int argc, char const * const * argv)
{
    opts_t opts;
    if (parse_opts(&opts, argc, argv)) return 3;
    if (opts.help)
    {
        printf("bin_to_hex [OPTS]\n"
               "Options:\n"
               " -h     this help\n"
               " -w[N]  outputs a newline after each N[=10] bytes or at the end\n"
               " -l     output hex in lowercase\n"
               " -u     output hex in uppercase\n"
               );
        return 0;
    }

    return process(stdin, stdout, &opts);
}

