/**
 * Decrypts an RSA encrypted file
 */

// standard libraries
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// header files
#include "rsa.h"

int main(int argc, char* argv[])
{
	// ensure proper usage
	if (argc != 3)
	{
		printf("Usage: ./decrypt hashfile outfile (w/o extension)\n");
		return 1;
	}
	
	// open hash file for reading
	FILE* infile = fopen(argv[1], "r");
	if (infile == NULL)
	{
		printf("Can't find hash file\n");
		return 2;
	}
	
	// extract file signature	
	int k;
	fread(&k, sizeof(int), 1, infile);
	
	char* ext = NULL;
	if (k != 0)
	{
		ext = malloc((k + 1) * sizeof(char));
		fread(ext, k * sizeof(char), 1, infile);
		ext[k] = '\0';
	}
	
	// extract RSA key
	RSAkey* key = malloc(sizeof(*key));
	fread(key, sizeof(*key), 1, infile);
	
	// make sure it's valid
	if (mod_exp(mod_exp(k, key->e, key->n), key->d, key->n) != k)
	{
		printf("\"hash\" isn't a supported hashfile\n");
		return 3;
	}
	
	// open outfile for writing
	int m = strlen(argv[2]);
	int l = (ext == NULL) ? m : m + 1 + k;
	char* filename = malloc(sizeof(char) * (l + 1));
	filename = (ext == NULL) ? argv[2] : strcat(strcat(argv[2], "."), ext);
	filename[l] = '\0';
	
	FILE* outfile = fopen(filename, "w");
	if (outfile == NULL)
	{
		printf("Error while processing outfile\n");
		return 4;
	}
	
	// start decryption
	printf("Decrypting...\n");
	
	// temporary storage
	int temp;
	while (fread(&temp, sizeof(int), 1, infile) == 1)
	{
		int c = mod_exp(temp, key->d, key->n);
		fwrite(&c, sizeof(uint8_t), 1, outfile);
	}
	
	printf("Decryption successful!\n");
	printf("%s was created.\n", filename);
	
	// free the memory
	free(ext);
	free(key);
	fclose(infile);
	fclose(outfile);
	
	return 0;
}
