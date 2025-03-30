#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
	char *keyword;
	char *value;
} token;


// look through the entire document, if print is found create a token.
void lexer (char *file, token tokens[], int *num_of_tokens) {
	char keyprint[] = "print";
	int tokennum = 0;
	int str_index = 0;
	int reading_str = 0;
	size_t value_size = 100;
	// print token
	for (int i = 0; file[i] != '\0'; i++) {
		// find the keyword
		if (file[i] == 'p' && reading_str == 0) {
			int keyexists = 0;
			for (int j = 0; j < 5; j++) {
				if (file[i+j] == keyprint[j]) keyexists++;
			}
			if (keyexists == 5) {
				tokens[tokennum].keyword = "print";
				tokens[tokennum].value = malloc (value_size * sizeof(char));
			}
		}
		// find the value
		if (file[i] == '"' && reading_str == 0) {\
			reading_str = 1;
			i++;
		}
		if (file[i] == '"' && reading_str == 1) {
			reading_str = 0;
			tokennum++;
			str_index = 0;
		}
		if (reading_str == 1) {
			if (str_index >= value_size -1) {
				value_size += 100;
				tokens[tokennum].value = realloc (tokens[tokennum].value, value_size * sizeof(char));
			}
			tokens[tokennum].value[str_index] = file[i];
			str_index++;
		}
	}
	*num_of_tokens = tokennum;
}

void code_gen (token tokens[], int num_of_tokens, FILE *output) {
	for (int i = 0; i < num_of_tokens; i++) {
		if (tokens[i].keyword == "print") fprintf(output, "\tprintf(\"%s\");\n", tokens[i].value);
	}
}


int main (int argc, char *argv[]) {
	
	FILE *fptr = fopen(argv[1], "r");
	if (fptr == NULL) {
		printf("could'nt access file.\n");
		return 1;
	}

	fseek(fptr, 0, SEEK_END);
	long file_size = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	
	char *file = malloc(file_size + 1);
	if (file == NULL) {
		printf("memory allocation failed.\n");
		return 1;
	}
	
	fread(file, 1, file_size, fptr);
	file[file_size] = '\0';

	char file_name[50];
	for (int i = 0; argv[1][i] != '.'; i++) {
		file_name[i] = argv[1][i];
	}
	strcat(file_name, ".c");
	
	token tokens[50]; // for small programs we assume a maximum of 50 tokens
	int num_of_tokens;
	lexer(file, tokens, &num_of_tokens);
	for (int i = 0; i < num_of_tokens; i++) {
		printf("%s: %s\n", tokens[i].keyword, tokens[i].value);
	}
	
	FILE *output = fopen(file_name, "w");
	
	fputs("#include <stdio.h>\n", output);
	fputs("int main () {\n", output);

	code_gen(tokens, num_of_tokens, output);

	fputs("\treturn 0;\n", output);
	fputs("}\n", output);
	free(file);
	fclose(fptr);
	return 0;
}
