/*
//	#(#) line.c	- read/write lines of arbitrary length
//
//	License: Creative Commons CC0 
//		http://creativecommons.org/publicdomain/zero/1.0/legalcode
//	Author:	James Sainsbury
//		toves@sdf.lonestar.org
*/
# include	<stdio.h>
# include	<stdlib.h>
# include	<errno.h>

# include	"constant.h"
# include	"line.h"

struct	line	{
	size_t	size;
	size_t	length;
	char*	l;
};

enum	{
	MINLINESIZE	= BUFSIZ,
};


static int	line_grow (LINE* lp, size_t newsize) {
	int	result	= ok;

	if (newsize < MINLINESIZE) 
		newsize	= MINLINESIZE;

	if (lp->size < newsize) { /* never shrink */
		char*	newline	= 0;
		if (lp->l != 0) {
			newline	= realloc (lp->l, newsize);
		}
		else	{
			newline	= malloc (newsize);
		}
		if (newline) {
			lp->l		= newline;
			lp->size	= newsize;
			result		= ok;
		}
		else	result	= -ENOMEM;
	}
	return	result;
}

int	line_Create (LINE** lpp, size_t size) {
	int	result	= -ENOMEM;
	LINE*	lp	= calloc (sizeof(*lp), 1);
	if (lp) {
		lp->size	= 0;
		lp->length	= 0;
		lp->l		= 0;
		result	= line_grow (lp, size);
		if (result == ok) {
			*lpp	= lp;
		}
		else	free (lp);
	}
	return	result;
}
int	line_Delete (LINE** lpp) {
	if (lpp) {
		LINE*	lp	= *lpp;
		if (lp) {
			if (lp->l)
				free (lp->l);
			free (lp);
			*lpp	= 0;
		}
	}
	return	ok;
}
/*
//	Read a line into struct LINE storing the newline don't terminate with \0
//
//	return	L_EOF	- EOF
//		>0	- bytes read including newline
//		L_ERR	- error 
*/
enum	{
	L_MORE	= -2, // buffer too small enlarge and continue
	L_ERR	= -1,
	L_EOF	= 0,
};
static ssize_t	line_getx (FILE* input, LINE* lp, size_t off) {
	ssize_t	result	= L_MORE;
	size_t	i	= off;
	size_t	j	= lp->size;
	char*	l	= lp->l;
	
	while (i!=j) {
		int	ch	= fgetc (input);
		
		if (ch == EOF && ferror (input)) {
			result	= L_ERR;
			j	= i;
		}
		else if (ch == '\n' || ch == EOF) {
			if (ch == EOF && i==0) { /* just an EOF */
				result	= L_EOF;
			}
			else	{ 
				l [i++]	= '\n';
				lp->length	= i;
				result	= i;
			}
			j	= i;
		}
		else	{
			l [i++]	= ch;
		}
	}
	if (result == L_MORE) {/* buffer size too small. Note: L_MORE never returned */
		result	= line_grow (lp, 2*lp->size);
		if (result == ok) {
			result	= line_getx (input, lp, i);
		}
	}
	else if ((result > L_EOF) && i < (lp->size-1)) { /* If there is space append a \0 */
		l[i++]	= '\0';
	}
	return	result;
}

//
//	Read a line into struct LINE storing the newline don't terminate with \0
//	Return	-1 (L_ERR) on error, 0 (L_EOF) on EOF and >0 on normal return
//
ssize_t	line_get (FILE* input, LINE* line) {
	return	line_getx (input, line, 0);
}

//
//	Write line to output
//	Return: number of bytes writen
//	on error (-1)
//	
ssize_t	line_put (FILE* output, LINE* line) {
	ssize_t	result	= err;
	size_t	request	= line->length;

	size_t	nwriten	= fwrite (line->l, sizeof(char), request, output);

	if (nwriten == request) {
		result	= nwriten;
	}
	return	result;
}
