#include <getopt.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
 * Check if the given string representing the file name ends with ".md"
 * Shortest filename is a.md, length of 4.
 */
inline static int
ismarkdownfile(char *s)
{
	const size_t len = strlen(s);
	if (len < 4)
	{
		return 0;
	}
	return (s[len-3] == '.' && s[len-2] == 'm' && s[len-1] == 'd');
}

/*
 * Print the usage help.
 */
static void
helpme(char *program)
{
	printf("Usage: %s [OPTIONS] PROJECT_DIRECTORY OUTPUT_DIRECTORY\n", program);
	puts("Options:");
	puts("  -h\t Print usage help.");
}

/*
 * Return pointer of the first non-whitespace character in the given string.
 * Example: str="  lzd", trim(str) would return str+2, since the first
 * non-whitespace character is 'l', offset of 2.
 */
static char *
trim(char *str)
{
	char *trimmed = str;
	while (trimmed[0] == ' ' || trimmed[0] == '\t')
	{
		trimmed++;
	}
	return trimmed;
}

/*
 * Define the data used to make the HTML files.
 * PAGE_WIDTH defines the maximum width of the line.
 * TEXT_COLOUR defines the colour of the text.
 */
#define PAGE_WIDTH "42em"
#define TEXT_COLOUR "#fff2da"
#define LINK_COLOUR "#fa4040"
#define LINK_VISITCOLOUR "#aa2040"
#define BACKGROUND_COLOUR "#353C51"
static char *style =
"html{background:url(\"res/qiron_seam.jpg\");background-color:"BACKGROUND_COLOUR";background-repeat:repeat;font-family:\"Linux Libertine O\", \"Liberation Serif\", \"Times New Roman\", Times, serif;}\n\
body{margin:2em auto;max-width:"PAGE_WIDTH";line-height:1.6;font-size:1.2em;color:"TEXT_COLOUR";padding:0 10px;image-rendering:pixelated;image-rendering:crisp-edges}\
h1, h2{line-height:1.2; text-shadow: 2px 3px 0 #111}\n\
h1{font-size:2.5em}\n\
h2{font-size:2em;padding-top:1em}\n\
h3{padding-right: 1em; text-shadow: 1px 2px 0 #111}\n\
hr{margin-top: 2em; margin-bottom: 2em;border-color:#cfc2aa;color:#cfc2aa}\n\
img{width:100%;height:auto}\n\
a{color:"LINK_COLOUR"}\n\
a:visited{color:"LINK_VISITCOLOUR"}";
static char *keywords         = "Hydra,Software";
static char *author           = "Aleksandar";
static char *description      = "(denote void)";
static char *javascriptpath   = "res/main.js";
static void
convert(const char *path)
{
	size_t len;
	char outpath[256], line[512], title[64];
	FILE *out, *in;
	
	/* Output path should replace suffix ".md" to ".html" */
	strcpy(outpath, path);
	len = strlen(outpath);
	outpath[len-2] = '\0'; /* replace 'm' in ".md" with NULL. */
	strcat(outpath, "html");
	printf("Working with \"%s\".\n", path);
	printf("Output is at \"%s\".\n", outpath);

	in  = fopen(path, "r");
	out = fopen(outpath, "w");
	strcpy(title, "No title!!!");
	
	/* Head */
	fputs("<!DOCTYPE html>\n", out);
	fputs("<html>\n", out);
	fputs("<head>\n", out);
	fputs("<meta charset=\"UTF-8\">\n", out);
	fputs("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n", out);
	fprintf(out, "<meta name=\"theme-color\" content=\"%s\">\n", BACKGROUND_COLOUR);
	fprintf(out, "<meta name=\"keywords\" content=\"%s\">\n", keywords);
	fprintf(out, "<meta name=\"author\" content=\"%s\">\n", author);
	fprintf(out, "<meta name=\"description\" content=\"%s\">\n", description);
	fprintf(out, "<style>%s</style>\n", style);
	fprintf(out, "<title>%s</title>\n", title);
	fprintf(out, "<script src=\"%s\" defer></script>\n", javascriptpath);
	fputs("</head>\n", out);
	
	/* Body */
	fputs("<body onload=\"bodyloaded()\">\n", out);
	while (fgets(line, sizeof(line), in) != NULL)
	{
		char *line_trimmed;
		line[strcspn(line, "\n")] = '\0';
		
		/* Skip empty lines. */
		if (line[0] == '\0')
		{
			continue;
		}

		/* Headers */
		if (line[0] == '#')
		{
			if (line[1] == '#')
			{
				if (line[2] == '#')
				{
					if (line[3] == '#')
					{
						line_trimmed = trim(line+4);
						fprintf(out, "<h4>%s</h4>\n", line_trimmed);
					}
					else
					{
						line_trimmed = trim(line+3);
						fprintf(out, "<h3>%s</h3>\n", line_trimmed);
					}
				}
				else
				{
					line_trimmed = trim(line+2);
					fprintf(out, "<h2>%s</h2>\n", line_trimmed);
				}
			}
			else
			{
				line_trimmed = trim(line+1);
				fprintf(out, "<h1>%s</h1>\n", line_trimmed);
			}
		}
		/* Images */
		else if (line[0] == '!')
		{
			char alttext[64], srcpath[64];
			size_t i, j, len;
			/*
			 * ![alternative text](relative/path.gif)
			 * xx^---------------^
			 * Starting from offset of 2, find the first occurance of ']'.
			 * This segment is the alternative text for the image tag.
			 */
			len = strlen(line+2);
			j   = 0;
			for (i = 2; i < len; i++)
			{
				if (line[i] == ']')
				{
					break;
				}
				alttext[j] = line[i];
				j++;
			}
			alttext[j] = '\0';
			/*
			 * ![alternative text](relative/path.gif)
			 *                   xx^----------------^
			 * Skip two characters denoting the end and start of the new segment.
			 * Image path is everthing before the letter ')'.
			 */
			j  = 0;
			i += 2; /* Skip ']' and '(' */
			for (; i <= len; i++)
			{
				if (line[i] == ')')
				{
					break;
				}
				srcpath[j] = line[i];
				j++;
			}
			srcpath[j] = '\0';
			fprintf(out, "<img src=\"%s\" alt=\"%s\">\n", srcpath, alttext);
		}
		/* Paragraphs */
		else
		{
			fprintf(out, "<p>%s</p>\n", line);
		}
	}
	fputs("</body>\n", out);
	fputs("</html>\n", out);
}

/*
 * Find .md files in the current directory and generate appropriate html file.
 * Recursively seek into other directories.
 * Failure to open a file is ignored.
 */
static void
seekdirectory(const char *path)
{
	DIR *directory;
	struct dirent *entry;

	if ((directory = opendir(path)) == NULL)
	{
		fprintf(stderr, "Directory \"%s\" can't be open.\n", path);
	}

	while ((entry = readdir(directory)) != NULL)
	{
		char relativefile[512];
		struct stat filestat;

		/* Skip dot files. */
		if (entry->d_name[0] == '.')
		{
			continue;
		}
		
		/* Relative path to the found file is stored in relativefile. */
		snprintf(relativefile, 512, "%s/%s", path, entry->d_name);

		/* Find out if the file is a directory. If so, enter it. */
		if (stat(relativefile, &filestat ) == -1)
		{
			fprintf(stderr, "Could not stat file \"%s\".\n", relativefile);
		}
		if (S_ISDIR(filestat.st_mode))
		{
			printf("Entering directory \"%s\".\n", relativefile);
			seekdirectory(relativefile);
			continue;
		}

		/*
		 * Markdown files generate html files.
		 * Full relative path is not needed.
		 */
		if (ismarkdownfile(entry->d_name))
		{
			convert(relativefile);
		}
	}
	printf("Leaving directory \"%s\".\n", path);
}

int
main(int argc, char **argv)
{
	int opt;
	char *projectdirectory;

	/* Parse arguments. */
	while ((opt = getopt(argc, argv, "h")) != -1)
	{
		switch (opt)
		{
		case 'h':
			helpme(argv[0]);
			break;
		default:
			helpme(argv[0]);
			return 1;
		}
	}
	if (optind >= argc)
	{
		puts("No project directory given. Exiting.");
		helpme(argv[0]);
		return 0;
	}
	if (optind+1 >= argc)
	{
		puts("No output directory given. Exiting.");
		helpme(argv[0]);
		return 0;
	}

	/*
	 * The leftover arguments are found at index of `optind`.
	 * Only the first and second argument is required, others are ignored.
	 */
	projectdirectory = argv[optind];
	printf("Project directory is \"%s\".\n", argv[optind]);
	printf("Output directory is \"%s\".\n", argv[optind+1]);
	puts("----------------");

	/*
	 * Here, recursion helps.
	 * Start finding all relevant files from the project root directory.
	 */
	seekdirectory(projectdirectory);
	puts("I am done.");
	puts("Have a nice day.");

	return 0;
}

