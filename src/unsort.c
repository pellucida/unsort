/*
//	@(#) unsort.c	- output a permutation of the lines input
//
//	License: Creative Commons CC0 
//		http://creativecommons.org/publicdomain/zero/1.0/legalcode
//	Author:	James Sainsbury
//		toves@sdf.lonestar.org
*/
# include	<stdio.h>
# include	<stdlib.h>
# include	<stdarg.h>
# include	<string.h>
# include	<time.h>
# include	<errno.h>

# include	"constant.h"
# include	"line.h"


enum	{
	LOG2MAXLINES	= 32,	/* 4 billion lines, = 48 doesn't seem to affect speed */
	RANDOM_BITS	= 31,	/* random() produces longs 0..2^31-1 */
};

// Sizes and positions within FILE* vector
enum	{
	F_NTMP		= LOG2MAXLINES,	// number of tmpfiles
	F_NFILES	= 1+F_NTMP+1,	// total number of files input/tmp/output
					// position of 
	P_INPUT		= 0,		//             input file
	P_TMP1		= 1,		//             first tmpfile
	P_TMPN		= F_NTMP,	//             last tmpfile
	P_OUTPUT	= F_NTMP+1,	//             output file
};


static	void	fatal_msg (const  char* fmt,...) {
	va_list	ap;
	va_start (ap, fmt);
	vfprintf (stderr, fmt, ap);
	va_end (ap);
	exit (EXIT_FAILURE);
}

//
// Distribute line to one of the tmp files or output file
//
void	distribute (FILE* files[], size_t after, LINE* line) {
	size_t	result	= err;
	size_t	i	= after;
	size_t	last	= P_OUTPUT;
	size_t	j	= last;

static	unsigned long	u	= 0;
static	int	bits_left	= 0;


	while (i!=j) {
		if (bits_left == 0) {
			u	= random ();
			bits_left	= RANDOM_BITS;
		}
		if ((u&1) == 1) {	/* find first set bit */
			j	= i;
		}
		else {
			++i;
		}
		--bits_left;
		u >>= 1;
	}
	if (i >= last) {
		i	= last;
	}
	result	= line_put (files[i], line);
	if (result < 0) 
		fatal_msg ("ERROR: Writing line: ");
}

// Place the input and output stream at the beginning and end of
// the files[] vector respectively.
// Fill in the slots between with tmp files.

void	setup_files (FILE* files[], FILE* input, FILE* output) {
	size_t	i	= 0;
	files [P_INPUT]	= input;
	files [P_OUTPUT]= output;
	for (i=P_TMP1; i<=P_TMPN; ++i) {
		FILE*	tmp	= tmpfile();
		if (tmp) {
			files [i]	= tmp;
		}
		else	fatal_msg ("ERROR: unsort: tmpfile() creation failed\n");
	}
}

int	process (FILE* input, FILE* output) {
	FILE*	files [F_NFILES];
	LINE*	line	= 0;
	int	result	= line_Create (&line, 0);

	if (result == ok) {
		size_t	i	= 0;
		setup_files (files, input, output);

		for (i = P_INPUT; i < P_OUTPUT; ++i) {
			FILE*	src	= files [i];
			ssize_t	nread	= 0;

			while ((nread = line_get (src, line)) > 0) {
				distribute (files, i+1, line);
			}
			if (nread < 0)
				fatal_msg ("ERROR: Read line failed: ");
			
			fclose (src);
			rewind (files[i+1]);
		}
		line_Delete (&line);
	}
}

static	void	random_seed (void) {
	time_t	seed	= time(0); // 1 sec resolution is too low
# if	defined(HAVE_CLOCK_GETTIME)
	struct	timespec	t	= {0, 0};
	int	retval	= clock_gettime (CLOCK_REALTIME, &t);
	if (retval==ok) {
		seed	= t.tv_nsec;
	}
# elif	defined(HAVE_GETPID)
	seed	*= getpid ();
# endif
	srandom (seed);
}

main (int argc, char* argv[]) {
	FILE*	input	= stdin;
	FILE*	output	= stdout;
	
	if (argc > 2)
		fatal_msg ("Usage: unsort [filename]\n");
	else if (argc == 2) { /* allow one (only) filename on command line */
		input	= fopen (argv[1], "r");
		if (input == 0)
			fatal_msg ("ERROR: Couldn't open file: '%s'\n", argv[1]);
	}
	random_seed ();
	process (input, output);
}
