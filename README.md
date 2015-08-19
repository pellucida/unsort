## unsort
permute the lines of a text file

#### DESCRIPTION

	unsort [file]

Reads lines from stdin or a file if specified and writes
each line to stdout in some random order.
ie
  the output is a permutation of the input.

The intermediate results are kept in temporary files
so that the memory foot print is largely independent
of input size.

Typical use would be where a sequence of values is easy
to generate but a permutation is need for the input of
another process.
eg
	seq 1 1024 | unsort | sort -n

or more realistically generating all the internet addresses
between 10.1.0.0 and 10.1.255.255, permuting them and feeding
them to a subnet merging program for testing.

	( for i in `seq 0 255`
	do
		for j in `seq 0 255`
		do
			printf "10.1.%d.%d\n" $i $j
		done
	done
	) | unsort | subnet_merge
	# Output should be 10.1.0.0/16

#### BUGS
The output is not uniformly distributed.

#### ALGORITHM
Probably too simplistic for anything other than simple testing
viz
Connect N of the following processes in a pipeline

	while more input :
		read line from input
		flip coin
		if heads :
			write line to output
		otherwise
			append line to temporary file
	copy contents of temporary file to output
	exit

where N is log2(maximum-lines-expected)

#### SEE ALSO
`shuf(1)` and `sort(1)` [gnu core-utils](http://www.gnu.org/software/coreutils/coreutils.html)

noting the usage
	sort --random-sort 
#### LICENSE
Creative Commons CC0 [http://creativecommons.org/publicdomain/zero/1.0/legalcode]
(http://creativecommons.org/publicdomain/zero/1.0/legalcode)

#### AUTHOR
[James Sainsbury](toves@sdf.lonestar.org)
