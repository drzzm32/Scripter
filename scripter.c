#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Comment below if your system is Linux
#define P_WIN

#define SCRIPT_MAX_LENGTH 128
#define TYPE_CONSOLE 0
#define TYPE_EXPLAIN 1

#ifdef P_WIN
#define CLS_SCREEN "cls"
#else
#define CLS_SCREEN "clear"
#endif

#define THIS_VERSION "ver0.06"

FILE* f;
double numBuf;
char* strBuf;
char* codeBuf;
char* scpBuf;
char* scpHead;
char* scpBody;
char* strPtr;
int lines = 1;
int jmpFlag = 0;

void encode(char* source, char* code) {
	int i = 0, j = 0, flag = 1;
	while (1) {
		strBuf[i] = source[i] + code[j];
		if (source[i + 1] == '\0') break;
		i += 1;
		if (code[j + 1] == '\0') flag = -flag;
		else if (j < 0) flag = -flag;
		j += flag;
	}
	strBuf[i + 1] = '\0';
}

void decode(char* source, char* code) {
	int i = 0, j = 0, flag = 1;
	while (1) {
		strBuf[i] = source[i] - code[j];
		if (source[i + 1] == '\0') break;
		i += 1;
		if (code[j + 1] == '\0') flag = -flag;
		else if (j < 0) flag = -flag;
		j += flag;	
	}
	strBuf[i + 1] = '\0';
}

int execute(char* script, int type) {
	sscanf(script, "%s %[^\n]", scpHead, scpBody);
	if (strcmp(scpHead, "print") == 0) {
		if (strcmp(scpBody, "_ver_") == 0) {
			printf("%s ", THIS_VERSION);
		} else if (strcmp(scpBody, "_mem_") == 0) {
			printf("%f ", numBuf);
		} else if (strcmp(scpBody, "_smem_") == 0) {
			printf("%s", strBuf);
		} else if (strcmp(scpBody, "_code_") == 0) {
			printf("%s", codeBuf);
		} else {
			printf("%s ", scpBody);
		}
		if (type == TYPE_CONSOLE) printf("\n");
	} else if (strcmp(scpHead, "println") == 0) {
		if (strcmp(scpBody, "_ver_") == 0) {
			printf("%s\n", THIS_VERSION);
		} else if (strcmp(scpBody, "_mem_") == 0) {
			printf("%f\n", numBuf);
		} else if (strcmp(scpBody, "_smem_") == 0) {
			printf("%s\n", strBuf);
		} else if (strcmp(scpBody, "_code_") == 0) {
			printf("%s\n", codeBuf);
		} else {
			printf("%s\n", scpBody);
		}
	} else if (strcmp(scpHead, "version") == 0 || strcmp(scpHead, "ver") == 0) {
		printf("Scripter version: %s\n", THIS_VERSION);
	} else if (strcmp(scpHead, CLS_SCREEN) == 0) {
		system(CLS_SCREEN);
	} 
	
	else if (strcmp(scpHead, "encode") == 0) {
		if (strcmp(scpBody, "_smem_") == 0) {
			encode(strBuf, codeBuf);
		} else {
			encode(scpBody, codeBuf);
		}
	} else if (strcmp(scpHead, "decode") == 0) {
		if (strcmp(scpBody, "_smem_") == 0) {
			decode(strBuf, codeBuf);
		} else {
			decode(scpBody, codeBuf);
		}
	} else if (strcmp(scpHead, "puts") == 0) {
		strcpy(strBuf, scpBody);
	} else if (strcmp(scpHead, "putc") == 0) {
		strcpy(codeBuf, scpBody);
	}
	
	else if (strcmp(scpHead, "put") == 0) {
		numBuf = atof(scpBody);
	} else if (strcmp(scpHead, "op+") == 0) {
		numBuf = numBuf + atof(scpBody);
	} else if (strcmp(scpHead, "op-") == 0) {
		numBuf = numBuf - atof(scpBody);
	} else if (strcmp(scpHead, "op*") == 0) {
		numBuf = numBuf * atof(scpBody);
	} else if (strcmp(scpHead, "op/") == 0) {
		numBuf = numBuf / atof(scpBody);
	}

	else if (strcmp(scpHead, "when") == 0) {
		if (strcmp(scpBody, "_mem_") == 0) {
			if (numBuf <= 0) jmpFlag = 1;
		} else {
			if (sscanf(scpBody, "%f", &numBuf)) {
				if (numBuf <= 0) jmpFlag = 1;
			} else if (execute(scpBody, type)) {
				if (numBuf <= 0) jmpFlag = 1;
			}
		}
	}

	else if (strcmp(scpHead, "run") == 0) {
		FILE* ft = fopen(scpBody, "r");
		if (ft == NULL) {
			printf("File open failed.\n");
			return 1;
		} else {
			int tmplines = 1;
			while (1) {
				strPtr = fgets(scpBuf, SCRIPT_MAX_LENGTH, ft);
				if (strPtr == NULL)
					break;
				if (jmpFlag) {
					jmpFlag = 0;
					tmplines++;
					continue;
				}
				if (!execute(strPtr, type))
					break;
				tmplines++;
			}
			fclose(ft);
		}
	}
	
	else if (strcmp(scpHead, "exit") == 0) {
		if (strcmp(scpBody, "this") == 0) {
			return 0;
		} else {
			return 1;
		}
	} else {
		printf("Syntax error, in line %d: %s %s\n", lines, scpHead, scpBody);
		if (type == TYPE_EXPLAIN) printf("Script aborted.\n");
		return type == TYPE_CONSOLE ? 1 : (type == TYPE_EXPLAIN ? 0 : 1);
	}
	return 1;
}

void cliBuild() {
	printf("================\n");
	printf("Scripter %s\n", THIS_VERSION);
	printf("================\n");
	printf("Usage: Scripter <script name>\n\n");
	while (1) {
		printf("%d >>> ", lines);
		strPtr = gets(scpBuf);
		if (jmpFlag) {
			jmpFlag = 0;
			lines++;
			continue;
		}
		if (!execute(scpBuf, TYPE_CONSOLE))
			break;
		lines++;
	}
}

int main(int argc, char* argv[]) {
	scpBuf = (char* )malloc(sizeof(char) * SCRIPT_MAX_LENGTH);
	strcpy(scpBuf, "_null_");
	scpHead = (char* )malloc(sizeof(char) * SCRIPT_MAX_LENGTH);
	strcpy(scpHead, "_null_");
	scpBody = (char* )malloc(sizeof(char) * SCRIPT_MAX_LENGTH);
	strcpy(scpBody, "_null_");
	
	strBuf = (char* )malloc(sizeof(char) * SCRIPT_MAX_LENGTH);
	strcpy(strBuf, "_null_");
	codeBuf = (char* )malloc(sizeof(char) * SCRIPT_MAX_LENGTH);
	strcpy(codeBuf, "_null_");

	if (argc < 2) {
		cliBuild();
	} else if (argc == 2) {
		f = fopen(argv[1], "r");
		if (f == NULL) {
			printf("File open failed.\n");
			return 0;
		} else {
			while (1) {
				strPtr = fgets(scpBuf, SCRIPT_MAX_LENGTH, f);
				if (strPtr == NULL)
					break;
				if (jmpFlag) {
					jmpFlag = 0;
					lines++;
					continue;
				}
				if (!execute(strPtr, TYPE_EXPLAIN))
					break;
				lines++;
			}
			fclose(f);
		}
	}

	free(strBuf);
	free(codeBuf);
	free(scpBuf);
	free(scpHead);
	free(scpBody);

	return 0;
}