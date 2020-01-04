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

static void
convert(const char *path)
{
	size_t len;
	char outpath[256], line[512];
	FILE *out, *in;
	
	/* Output path should replace suffix ".md" to ".html"*/
	strcpy(outpath, path);
	len = strlen(outpath);
	outpath[len-2] = '\0'; /* replace 'm' in ".md" with NULL. */
	strcat(outpath, "html");
	printf("Working with \"%s\".\n", path);
	printf("Output is at \"%s\".\n", outpath);

	in  = fopen(path, "r");
	out = fopen(outpath, "w");
	while (fgets(line, sizeof(line), in) != NULL)
	{
		char *line_trimmed;
		line[strcspn(line, "\n")] = '\0';

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
	}
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

