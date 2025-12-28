#include <stdbool.h>
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "malloc.h"

#define MAX_LINE_SIZE	4096

char line[MAX_LINE_SIZE];
char lastPattern[MAX_LINE_SIZE];
char error[MAX_LINE_SIZE];
char **lines;
int nlines, curline;
bool modified;

static void print(char **lines, int start, int end){
	int line;

	if (nlines == 0) {
		printf("file is empty\n");
		return;
	}
	if (start >= nlines) {
		printf("at end of file\n");
		return;
	}
	if (start > end) {
		printf("no lines to print\n");
		return;
	}
	if (end >= nlines) {
		end = nlines - 1;
	}
	for (line = start; line <= end; line++) {
		// printf("%5d   %s", line + 1, lines[line]);
		printf("%d   %s", line + 1, lines[line]);
	}
	curline = end;
}

void insert(char *line){ 
	int len = strlen(line);
	char *copy = malloc(len + 1);
	strcpy(copy, line);
	lines = realloc(lines, (nlines + 1) * sizeof(char *));
	nlines++;
	memmove(&lines[curline + 1], &lines[curline], (nlines - curline - 1) * sizeof(char *));
	lines[curline] = copy;
}

void save(char *file){
	FILE *output;
	int i;

	if (!modified) {
		printf("no changes\n");
		return;
	}
	if ((output = fopen(file, "w")) == 0) {
		fprintf(stderr, "save: can't open file %s for writing\n", file);
		return;
	}
	for (i = 0; i < nlines; i++) {
		fputs(lines[i], output);
	}
	fclose(output);
	modified = false;
}

void skipBlanks(char **pline){
	while (**pline == ' ' || **pline == '\t') (*pline)++;
}

bool findPattern(char **pline, int *pos){
	char *line = *pline, *pattern = line;
	while (*line != '/' && *line != '\n' && *line != 0) {
		line++;
	}
	if (*line != 0) {
		*line++ = 0;
	}
	*pline = line;

	if (*pattern == 0) {
		pattern = lastPattern;
	}
	else {
		strcpy(lastPattern, pattern);
	}

	bool wrap = false;
	for (int i = 0; i < nlines; i++) {
		int j = (curline + i) % nlines;
		if (strstr(lines[j], pattern) != 0) {
			*pos = j;
			if (wrap && error[0] == 0) {
				sprintf(error, "wrapped around finding '%s'", pattern);
			}
			return true;
		}
		if (j == nlines - 1) {
			wrap = true;
		}
	}
	sprintf(error, "pattern '%s' not found", pattern);
	return false;
}

int strtoi10(const char *nptr, char **endptr) {
    const char *s = nptr;
    while (*s == ' ' || *s == '\t') s++;
    int neg = 0;
    if (*s == '-') { neg = 1; s++; }
    else if (*s == '+') s++;
    int val = 0, any = 0;
    while (*s >= '0' && *s <= '9') {
        val = val * 10 + (*s - '0');
        s++;
        any = 1;
    }
    if (endptr != 0) *endptr = any ? (char *) s : (char *)nptr;
    return neg ? -val : val;
}

bool parsePosition(char **pline, int *pos){
	skipBlanks(pline);
	char *line = *pline;
	if (*line == '/' || *line == '$' || *line == '.') {
		if (*line == '/') {
			line++;
			if (!findPattern(&line, pos)) {
				*pline = line;
				return false;
			}
		}
		else {
			*pos = *line == '$' ? nlines - 1: curline;
			if (*pos < 0) *pos = 0;
			line++;
		}
		skipBlanks(&line);
		if (*line == '-' || *line == '+') {
			long r = strtoi10(line, pline);
			*pos += r;
			if (*pos < 0) *pos = 0;
			else if (*pos > nlines) *pos = nlines;
		}
		else *pline = line;
		return true;
	}
	if ('0' <= *line && *line <= '9') {
		long r = strtoi10(line, pline);
		*pos = r == 0 ? 0 : (int) r - 1;
		return true;
	}
	return false;
}

/* Format of a line: "[start[,end]]C", where 'start' and 'end' are
 * positions and C some command (can be '\n').
 */
void parse(char *line, int *start, int *end, char *cmd){
	error[0] = 0;
	skipBlanks(&line);
	if (*line == 'g') {
		*start = 0;
		*end = nlines - 1;
		line++;
	}
	else if (*line == 'z') {
		*start = curline + 1;
		*end = curline + 10;
		line++;
	}
	else if (*line == '\n') {
		if (curline != nlines) {
			*start = *end = curline + 1;
		}
	}
	else if (parsePosition(&line, start)) {
		curline = *start;
		skipBlanks(&line);
		if (*line == 'z') {
			*end = *start + 9;
			if (*end > nlines) {
				*end = nlines;
			}
			line++;
		}
		else if (*line == ',') {
			line++;
			if (!parsePosition(&line, end)) {
				*end = *start;
			}
		}
		else {
			*end = *start;
		}
	}
	else {
		*start = *end = curline;
	}
	skipBlanks(&line);
	if (error[0] == 0) {
		*cmd = *line;
	}
	else {
		printf("%s\n", error);
		*cmd = 'p';
	}

	/* Set to within bounds.
	 */
	if (*start >= nlines) *start = nlines;
	if (*end >= nlines) *end = nlines;
	if (*start < 0) *start = 0;
	if (*end < 0) *end = 0;
}

void readInput(){
	printf("-- going into append mode.  After typing input, hit <control>D (EOF).\n");
	while (fgets(line, MAX_LINE_SIZE, stdin) != 0) {
		insert(line);
		curline++;
		modified = true;
	}
	clearerr(stdin);		// clear EOF
}

int main(int argc, char **argv){
	FILE *fp;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s file\n", argv[0]);
		return 1;
	}

	/* Read the file.
	 */
	if ((fp = fopen(argv[1], "r")) == 0) {
		printf("can't open %s\n", argv[1]);
		modified = true;
	}
	else {
		while (fgets(line, MAX_LINE_SIZE, fp) != 0) {
			insert(line);
			curline++;
		}
		fclose(fp);
	}

	/* Read commands.
	 */
    for (;;) {
		if (curline == nlines) {
			printf("EOF/%d -> ", nlines);
		}
		else {
			printf("%d/%d -> ", curline + 1, nlines);
		}
		fflush(stdout);
		int start, end;
		char cmd;
		if (fgets(line, MAX_LINE_SIZE, stdin) == 0) {
			cmd = 'q';
			start = end = curline;
			clearerr(stdin);
		}
		else {
			parse(line, &start, &end, &cmd);
		}
		switch (cmd) {
		case '\n': case 0:
			// if (start == curline + 1) {
			// 	printf("\033[F\033[K");		// go to start of previous line and erase it
			// }
			print(lines, start, end);
			break;
		case 'p':
			print(lines, start, end);
			break;
		case 'd':
			for (int i = start; i <= end; i++) {
				free(lines[i]);
			}
			memmove(&lines[start], &lines[end + 1],
								(nlines - end + 1) * sizeof(char *));
			nlines -= end - start + 1;
			modified = true;
			curline = start;
			break;
		case 'c':
			for (int i = start; i <= end; i++) {
				free(lines[i]);
			}
			memmove(&lines[start], &lines[end + 1],
								(nlines - end + 1) * sizeof(char *));
			nlines -= end - start + 1;
			modified = true;
			curline = start;
			readInput();
			break;
		case '-':
			if (curline > 0) {
				curline = curline - 1;
			}
			else {
				printf("Already at start of file\n");
			}
			print(lines, curline, curline);
			break;
		case 'i':
			printf("cur %d %d\n", curline, start);
			curline = start;
			readInput();
			break;
		case 'a':
			curline = end + 1;
			if (curline > nlines) {
				curline = nlines;
			}
			readInput();
			break;
		case 'w':
			save(argv[1]);
			break;
		case 'q':
			if (modified) {
				fprintf(stderr, "save first, or use Q\n");
				break;
			}
			else {
				return 0;
			}
		case 'Q':
			return 0;
		case ':':
			break;
		default:
			fprintf(stderr, "unknown command '%c'\n", cmd);
		}
	}

    return 0;
}
