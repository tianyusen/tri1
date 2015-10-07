// UCLA CS 111 Lab 1 main program

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <errno.h>
//#include <error.h>
#include <string.h>
#include <stdio.h>

int     opterr = 1,             /* if error message should be printed */
optind = 1,             /* index into parent argv vector */
optopt,                 /* character checked for validity */
optreset;               /* reset getopt */
char    *optarg;                /* argument associated with option */

#define BADCH   (int)'?'
#define BADARG  (int)':'
#define EMSG    ""

								/*
								* getopt --
								*      Parse argc/argv argument vector.
								*/
int
getopt(int nargc, char * const nargv[], const char *ostr)
{
	static char *place = EMSG;              /* option letter processing */
	const char *oli;                        /* option letter list index */

	if (optreset || !*place) {              /* update scanning pointer */
		optreset = 0;
		if (optind >= nargc || *(place = nargv[optind]) != '-') {
			place = EMSG;
			return (-1);
		}
		if (place[1] && *++place == '-') {      /* found "--" */
			++optind;
			place = EMSG;
			return (-1);
		}
	}                                       /* option letter okay? */
	if ((optopt = (int)*place++) == (int)':' ||
		!(oli = strchr(ostr, optopt))) {
		/*
		* if the user didn't specify '-' as an option,
		* assume it means -1.
		*/
		if (optopt == (int)'-')
			return (-1);
		if (!*place)
			++optind;
		if (opterr && *ostr != ':')
			(void)printf("illegal option -- %c\n", optopt);
		return (BADCH);
	}
	if (*++oli != ':') {                    /* don't need argument */
		optarg = NULL;
		if (!*place)
			++optind;
	}
	else {                                  /* need an argument */
		if (*place)                     /* no white space */
			optarg = place;
		else if (nargc <= ++optind) {   /* no arg */
			place = EMSG;
			if (*ostr == ':')
				return (BADARG);
			if (opterr)
				(void)printf("option requires an argument -- %c\n", optopt);
			return (BADCH);
		}
		else                            /* white space */
			optarg = nargv[optind];
		place = EMSG;
		++optind;
	}
	return (optopt);                        /* dump back option letter */
}
#include <stdio.h>


#include "command.h"


static char const *program_name;
static char const *script_name;


static void
usage(void)
{
	//error (1, 0, "usage: %s [-pt] SCRIPT-FILE", program_name);
}


static int
get_next_byte(void *stream)
{
	return getc(stream);
}


int
main(int argc, char **argv)
{
	int opt;
	int command_number = 1;
	int print_tree = 0;
	int time_travel = 0;
	program_name = argv[0];


	for (;;)
		switch (getopt(argc, argv, "pt"))
		{
		case 'p': print_tree = 1; break;
		case 't': time_travel = 1; break;
		default: usage(); break;
		case -1: goto options_exhausted;
		}
options_exhausted:;


	// There must be exactly one file argument.
	if (optind != argc - 1)
		usage();


	script_name = argv[optind];
	FILE *script_stream = fopen("Z:/Fall2015/111/lab1/ported/111/tri1/lab1-skeleton/lab1-Skeleton/test-p-ok - Copy.sh", "r");


	//if (! script_stream)
	//error (1, errno, "%s: cannot open", script_name);
	command_stream_t command_stream =
		make_command_stream(get_next_byte, script_stream);


	command_t last_command = NULL;
	command_t command;
	while ((command = read_command_stream(&command_stream)))
	{
		if (print_tree)
		{
			printf("# %d\n", command_number++);
			print_command(command);
		}
		else
		{
			last_command = command;
			execute_command(command, time_travel);
		}
	}


	return print_tree || !last_command ? 0 : command_status(last_command);
}