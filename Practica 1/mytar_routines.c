#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
//	 size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

//	size_t fwrite(const void *ptr, size_t size, size_t nmemb,
                     FILE *stream);

	if(origin == NULL || destination == NULL) return -1;
	
	char *str = malloc(nBytes);
	if(fread(str,sizeof(char),nBytes,origin) == nBytes){
		fwrite(str,sizeof(char),nBytes,destination);
		free(str);
		return nBytes;	
	// Complete the function
	free(str);
	return -1;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	int nBytes = 0;

	//mala practica, si pasamos null como primer argumento de fread el comportamiento es
	//impredecible
	//while(fread(NULL,sizeof(char),1,file) == 1) //leemos byte a byte
	//	nBytes++;
	
	while(getc(file) != '\0')
		nBytes++;
	
	//nBytes contiene el numero de nbytes del tamaño del string(necesario para reservar memoria para el
	//string que queremos devolver
	char *str = malloc(nBytes);
	// off_t lseek(int fd, off_t offset, int whence);
	// lseek(file,0,SEEK_SET);	//reseteamos el puntero de lectura del fichero
	rewind(file);			//equivalente
	if(fread(str,sizeof(char),nBytes,file); == nBytes){
		return &str;
	// Complete the function
	return NULL;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	stHeaderEntry* header = NULL;
	if(fread(nFiles, sizeof(int),1,tarFile) == 0) return NULL;
	header = malloc(sizeof(stHeaderENtry * (*nFiles)))
	for(int i = 0; i < nFiles; i++){
		header[i].name = loadstr(tarFile);
		if(header[i].name == NULL){
			free(header);
			return NULL;
		}
		fread(&header[i].size, siezof(int), 1, tarFile);
	}
	return header;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	File *tarfile = NULL;
	File *inputFile = NULL;
	stHeaderEntry *header = NULL;

	if((tarFile = fopen(tarName, "wb")) == NULL) return EXIT_FAILURE;

	header = malloc(sizeof(stHeaderEntry) * nFiles);	// First reserve room in the file to store the tarball header.

	offset = (nFiles * sizeof(unsigned int)) + sizeof(int);

	for(int i = 0; i < nFiles; i++){
		header[i].name = malloc(strlen(fileNames[i]+1));
		strcpy(header[i].name, fileNames[i]);	//build the representation of the tarball header in memory.
		offset += strlen(fileNames[i]+1);
	}

	fseek(tarFile, offset, SEEK_CUR);	//Move the file's position indicator to the data section (skip the header)
	
	for(int i = 0; i < nFiles; i++){
		if(inputFile = fopen(fileNames[i], "rb") == NULL){
			free(header);
			fclose(tarFile);
			return EXIT_FAILURE;
		}
		if(header[i].size = copynFile(inputFile, tarFile, INT_MAX) == -1){	//dump the contents of the source files (one by one) in the tarball archive
			free(header);
			fclose(tarFile);
			fclose(inputFile);
			return EXIT_FAILURE;
		}
		fclose(inputFile);
	}
	rewind(tarFile);	// rewind the file's position indicator

	fwrite(&nFiles, sizeof(int), tarFile);									//write the number of files as well as

	for(int i = 0; i < nFiles; i++){										// the (file name,file size) pairs in the tar archive.
		fwrite(header[i].name, strlen(header[i].name + 1, 1, tarFile));		
		fwrite(header[i].size, sizeof(int), 1, tarFile);					
	fclose(tarFile);
	free(header);

	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	File *tarfile = NULL;
	File *outputFile = NULL;
	stHeaderEntry *header = NULL;
	int nFiles;

	if(tarFile = fopen(tarName, "rb") == NULL) return EXIT_FAILURE;

	if(header = readHeader(tarName, &nFiles) == NULL) return EXIT_FAILLURE;

	for(int i = 0; i < nFiles; i++){
		if(outputFile = fopen(header[i].name, "wb") == NULL){
			free(header);
			return EXIT_FAILURE;
		}
		copynFile(tarFile, outputFile, header[i].size);
		fclose(outputFile);
	}

	fclose(tarFile);
	free(header);

	return EXIT_SUCCESS;
}
