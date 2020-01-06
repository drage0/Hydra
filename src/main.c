#include <getopt.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "configuration.h"

static int verbose = 0;

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
	printf("Usage: %s [OPTIONS] PROJECT_DIRECTORY\n", program);
	puts("Options:");
	puts("  -h\t Print usage help.");
	puts("  -v\t Verbose output.");
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

#define NO_NEW_LINE(str) str[strcspn(str, "\n")] = '\0';
static void
convert(const char *path)
{
	size_t len;
	char outpath[256], line[512], title[64], javascriptpath[64], keywords[64], description[256], spritesheet[64];
	FILE *out, *in;

	/* Output path should replace suffix ".md" to ".html" */
	strcpy(outpath, path);
	len = strlen(outpath);
	outpath[len-2] = '\0'; /* replace 'm' in ".md" with NULL. */
	strcat(outpath, "html");

	/* Friendly print. */
	printf("Working with \"%s\".\n", path);
	if (verbose)
	{
		printf("Output is at \"%s\".\n", outpath);
	}

	in  = fopen(path, "r");
	out = fopen(outpath, "w");

	/* Read- title */
	fgets(line, sizeof(line), in);
	strcpy(title, line+6);
	NO_NEW_LINE(title);

	/* Read- javascript path */
	fgets(line, sizeof(line), in);
	strcpy(javascriptpath, line+11);
	NO_NEW_LINE(javascriptpath);

	/* Read- keywords */
	fgets(line, sizeof(line), in);
	strcpy(keywords, line+9);
	NO_NEW_LINE(keywords);

	/* Read- description */
	fgets(line, sizeof(line), in);
	strcpy(description, line+12);
	NO_NEW_LINE(description);

	/* Read- sprite sheet */
	fgets(line, sizeof(line), in);
	strcpy(spritesheet, line+8);
	NO_NEW_LINE(spritesheet);

	/* Head */
	fputs("<!DOCTYPE html>\n", out);
	fputs("<html>\n", out);
	fputs("<head>\n", out);
	fputs("<meta charset=\"UTF-8\">\n", out);
	fputs("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n", out);
	fputs("<meta name=\"theme-color\" content=\""BACKGROUND_COLOUR"\">\n", out);
	fputs("<meta name=\"author\" content=\""AUTHOR"\">\n", out);
	fprintf(out, "<meta name=\"keywords\" content=\"%s\">\n", trim(keywords));
	fprintf(out, "<meta name=\"description\" content=\"%s\">\n", trim(description));
	fprintf(out, "<title>%s</title>\n", trim(title));
	fprintf(out, "<style>"STYLE"</style>\n", trim(spritesheet));
	fprintf(out, "<script src=\"%s\" defer></script>\n", trim(javascriptpath));
	fputs("</head>\n", out);

	/* Body */
	fputs("<body onload=\"bodyloaded()\">\n", out);
	while (fgets(line, sizeof(line), in) != NULL)
	{
		NO_NEW_LINE(line);

		/* Skip empty lines. */
		if (line[0] == '\0')
		{
			continue;
		}

		/* Headers */
		if (line[0] == '#')
		{
			char *line_trimmed;
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
		/*
		 * Paragraphs
		 *
		 * Paragraphs that begin with '~' are considered -fancy text- and they
		 * have their own style(class).
		 */
		else
		{
			char *parameters = "";
			char *text       = line;
			/*
			 * Sprite schema is "${icon}".
			 */
			if (line[0] == '$')
			{
				char icon[32];
				size_t iconlength;
				const char *iconstart = line+2;
				text = strchr(iconstart, '}')+1;
				iconlength = text-(iconstart);
				strncpy(icon, iconstart, iconlength);
				icon[iconlength-1] = '\0';
				fprintf(out, "<p><span class='i' id='%s'></span>%s</p>\n", icon, text);
			}
			else
			{
				if (line[0] == '~')
				{
					parameters = " class='f'";
					text       = line+1;
				}
				fprintf(out, "<p%s>%s</p>\n", parameters, text);
			}
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
		return;
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
			if (verbose)
			{
				printf("Entering directory \"%s\".\n", relativefile);
			}
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
	if (verbose)
	{
		printf("Leaving directory \"%s\".\n", path);
	}
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
		case 'v':
			verbose = 1;
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

	/*
	 * The leftover arguments are found at index of `optind`.
	 * Only the first and second argument is required, others are ignored.
	 */
	projectdirectory = argv[optind];
	if (verbose)
	{
		printf("Project directory is \"%s\".\n", argv[optind]);
		puts("----------------");
	}

	/*
	 * Here, recursion helps.
	 * Start finding all relevant files from the project root directory.
	 */
	seekdirectory(projectdirectory);
	puts("I am done.");
	puts("Have a nice day.");

	return 0;
}

