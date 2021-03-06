#include <getopt.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "configuration.h"

static int verbose = 0;
static char *projectdirectory;

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
	char outpath[WORKINGFILE_PATH_LENGHT_MAX], line[LINE_LENGTH_MAX], title[TITLE_LENGTH_MAX], javascriptpath[JS_LENGTH_MAX], keywords[KEYWORDS_LENGTH_MAX], description[DESCRIPTION_LENGTH_MAX], spritesheet[SPRITESHEET_LENGTH_MAX], stylepath[STYLEPATH_LENGTH_MAX], stylefile[STYLE_LENGTH_MAX], style[STYLE_LENGTH_MAX], background[BACKGROUND_LENGTH_MAX];
	int recordlist, rawhtml;
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

	/* Read- background */
	fgets(line, sizeof(line), in);
	strcpy(background, line+11);
	NO_NEW_LINE(background);

	/* Read- style (CSS) */
	strcpy(stylepath, projectdirectory);
	strcat(stylepath, "/");
	fgets(line, sizeof(line), in);
	strcat(stylepath, trim(line+6));
	NO_NEW_LINE(stylepath);
	{
		long len;
		FILE *sf = fopen(stylepath, "r");
		if (!sf)
		{
			fprintf(stderr, "Failed to open style file \"%s\"!\n", stylepath);
			return;
		}
		fseek(sf, 0, SEEK_END);
		len = ftell(sf);
		fseek(sf, 0, SEEK_SET);
		fread(stylefile, sizeof(char), len, sf);
		fclose(sf);
		sprintf(style, stylefile, trim(background), trim(spritesheet));
	}

	/* Head */
	fputs("<!DOCTYPE html>\n", out);
	fputs("<html lang=\""LANGUAGE"\">\n", out);
	fputs("<head>\n", out);
	fputs("<meta charset=\"UTF-8\">\n", out);
	fputs("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n", out);
	fputs("<meta name=\"theme-color\" content=\""BACKGROUND_COLOUR"\">\n", out);
	fputs("<meta name=\"author\" content=\""AUTHOR"\">\n", out);
	fputs("<link rel=\"icon\" href=\""FAVICON"\" type=\"image/gif\">\n", out);
	fprintf(out, "<meta name=\"keywords\" content=\"%s\">\n", trim(keywords));
	fprintf(out, "<meta name=\"description\" content=\"%s\">\n", trim(description));
	fprintf(out, "<title>%s</title>\n", trim(title));
	fprintf(out, "<style>%s</style>\n", style);
	fprintf(out, "<script src=\"%s\" defer></script>\n", trim(javascriptpath));
	fputs("</head>\n", out);

	/* Body */
	fputs("<body onload=\"bodyloaded()\">\n", out);
	recordlist = rawhtml = 0;
	while (fgets(line, sizeof(line), in) != NULL)
	{
		NO_NEW_LINE(line);

		/* Skip empty lines. */
		if (line[0] == '\0')
		{
			continue;
		}

		/* Images */
		if (line[0] == '!')
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
		 * List
		 *
		 * recordlist of 1 will make the upcoming paragraphs output a list item, "<li>", instead of "<p>".
		 */
		else if (line[0] == '%')
		{
			recordlist = 1-recordlist;
			fputs((recordlist ? "<ul>" : "</ul>"), out);
		}
		/*
		 * Horizontal line
		 */
		else if (strlen(line) >= 3 && line[0] == '-' && line[1] == '-' && line[2] == '-')
		{
			fputs("<hr>\n", out);
		}
		/*
		 * Raw html
		 */
		else if (line[0] == '&')
		{
			rawhtml = 1-rawhtml;
		}
		/*
		 * Paragraphs
		 *
		 * Paragraphs that begin with '~' are considered -fancy text- and they
		 * have their own style(class).
		 *
		 * Paragraphs beginning with '#' are headers.
		 */
		else if (!rawhtml)
		{
			char *parameters, *linestart, *tag;
			size_t i, len;

			/* Paragraphs beginning with '~' have their own class. */
			parameters = "";
			linestart  = line;
			if (line[0] == '~')
			{
				parameters = " class='f'";
				linestart += 1;
			}

			/* If paragraph begins with '#', consider it as a header. */
			tag = "p";
			if (line[0] == '#')
			{
				if (line[1] == '#')
				{
					if (line[2] == '#')
					{
						if (line[3] == '#')
						{
							tag = "h4";
							linestart = line+4;
						}
						else
						{
							tag = "h3";
							linestart = line+3;
						}
					}
					else
					{
						tag = "h2";
						linestart = line+2;
					}
				}
				else
				{
					tag = "h1";
					linestart = line+1;
				}
			}

			/* Tag is 'li' if we are in list mode. */
			if (recordlist)
			{
				tag = "li";
			}

			/* Begin tag */
			fprintf(out, "<%s%s>", tag, parameters);
			len = strlen(linestart);
			for (i = 0; i < len; i++)
			{
				/*
				 * Link special. (<a> tag)
				 *
				 * Schema:
				 * |the.url|shown text|
				 *  +----->0--------->0
				 *
				 * By setting the '|' chracters to NULL,
				 * string (linestart+i+1) will be the URL,
				 * string (urlend+1) will be the shown text.
				 *
				 * The first letter of the URL is located at linestart+i+1.
				 */
				if (linestart[i] == '|')
				{
					char * const urlbegin = linestart+i+1;
					char *urlend, *txtend;

					/* Malformed URLs will be printed as they are, without the link tag. */
					if (((urlend = strchr(urlbegin, '|')) && (txtend = strchr(urlend+1, '|'))) == 0)
					{
						fprintf(stderr, "Malformed link (missing '|'): %s\n", urlbegin);
						fputs(urlbegin, out);
						break;
					}
					*urlend = '\0';
					*txtend = '\0';

					/* Write out... */
					fputs("<a href=\"", out);
					fputs(urlbegin, out);
					fputs("\">", out);
					fputs(urlend+1, out);
					fputs("</a>", out);

					/*
					 * Advance the read index in this parsing line.
					 * txtend-(linestart+i+1)+1 is the length of the parsed string.
					 */
					i += txtend-(linestart+i+1)+1;
				}
				/* Sprite special. */
				else if (linestart[i] == '$')
				{
					char icon[SPRITE_LENGHT_MAX];
					const char *iconstart = linestart+i+2;
					const char *iconend   = strchr(iconstart, '}')+1;
					strncpy(icon, iconstart, iconend-iconstart);
					icon[iconend-iconstart-1] = '\0';
					fprintf(out, "<span class='i' id='%s'></span>", icon);
					i += strlen(icon)+2;
				}
				/* Ordinary letter. */
				else
				{
					fputc(linestart[i], out);
				}
			}
			/* End tag */
			fprintf(out, "</%s>\n", tag);
		}
		else
		{
			fputs(line, out);
			fputs("\n", out);
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
		char relativefile[WORKINGFILE_PATH_LENGHT_MAX];
		struct stat filestat;

		/* Skip dot files. */
		if (entry->d_name[0] == '.')
		{
			continue;
		}

		/* Relative path to the found file is stored in relativefile. */
		snprintf(relativefile, WORKINGFILE_PATH_LENGHT_MAX, "%s/%s", path, entry->d_name);

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

