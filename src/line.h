/*
//	#(#) line.h
//
//	License: Creative Commons CC0 
//		http://creativecommons.org/publicdomain/zero/1.0/legalcode
//	Author:	James Sainsbury
//		toves@sdf.lonestar.org
*/
# if	!defined( LINE_H_)
# define	LINE_H_

# include	<stdio.h>
# include	<stdlib.h>

typedef	struct	line	LINE;

int	line_Create (LINE** lpp, size_t size);
int	line_Delete (LINE** lpp);
ssize_t	line_get (FILE* input, LINE* line);
ssize_t	line_put (FILE* output, LINE* line);

# endif
