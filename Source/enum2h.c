/*****************************************************************************
 *;++
 * Name           : $RCSfile: enum2h.c,v $
 * Title          : ENUM2H.C
 * C Author       : Jim Page
 * Created        : 21/04/95
 *
 * Copyright	: 1995-2022 Imagination Technologies (c)
 * License		: MIT
 *
 * Description	  : Converts an enum .h file to an array of structures
 *                   
 * Program Type   : C module (.dll)
 *
 * RCS info:
 *
 * $Date: 1995/07/25 12:03:54 $
 * $Revision: 1.3 $
 * $Locker:  $
 * $Log: enum2h.c,v $
 * Revision 1.3  1995/07/25  12:03:54  jop
 * Removed toupper bug
 *
 * Revision 1.2  1995/07/25  11:30:00  jop
 * remived strupr() from code and de-ansified it for cc
 *
 * Revision 1.1  1995/07/23  17:12:51  jop
 * Initial revision
 *
 *
 *;--
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Globals */

char	szLineBuff[512];
char	szIpFileName[80];
char	szOpFileName[80];
char	szVariableName[80];
char	szItemName[64];

/*
	Main entry point
*/
int main (argc, argv)
	int argc;
	char *argv[];
{
	FILE			*inputFile;	
	FILE			*outputFile;
	char 			*szPos;
	int 			ret = 0, nFirstTime = 1;
	unsigned int 	itemID = 0;
	int 			nItems = 0, k;
			
	/* Check input param's */
	if (argc != 4)
	{
		printf ("Usage: enum2h <input file> <output file> <array name>\n");
		return (0);
	}

	/* Check */
	if (argv[1] == NULL)
	{
		printf ("Input file missing\n");
		return (0);
	}

	if (argv[2] == NULL)
	{
		printf ("Output file missing\n");
		return (0);
	}

	if (argv[3] == NULL)
	{
		printf ("Variable name missing\n");
		return (0);
	}

	/* Get the input file name - argument 1 */
	strcpy (szIpFileName, argv[1]);

	/* Get the output file name - argument 2 */
	strcpy (szOpFileName, argv[2]);

	/* Get the variable name - argument 3 */
	strcpy (szVariableName, argv[3]);

	printf ("Translating %s to %s\n", szIpFileName, szOpFileName);

	/* Open the output file */
	if ((outputFile = fopen (szOpFileName, "wa")) == NULL)
	{
		/* Failed to open/create it */
		printf ("Could not create the output file: %s\n", szOpFileName);
		return (0);
	}

	/* Open the input file */
	if ((inputFile = fopen (szIpFileName, "rt")) == NULL)
	{
		/* Failed to open/create it */
		printf ("Could not open the input file: %s\n", szIpFileName);
		return (0);
	}
		
	/* Write header to output file */
	fprintf (outputFile, "/* Enum2H auto-generated custom resource - DO NOT EDIT BY HAND!! */\n");
	fprintf (outputFile, "/* Created from file: %s */\n", szIpFileName);
	fprintf (outputFile, "\n");
	fprintf (outputFile, "\n");

	/* Write struct definition */	
	fprintf (outputFile, "typedef struct tagENUMSTRUCT\n");
	fprintf (outputFile, "{\n");
	fprintf (outputFile, "\tint\t\tnID;\t\t\t\t/* integer value of enum item */\n");
	fprintf (outputFile, "\tchar\t*pszName;\t\t\t/* pointer to null-terminated item name string */\n");
	fprintf (outputFile, "\tchar\t*pszDescription;\t/* pointer to null-terminated description string*/\n");
	fprintf (outputFile, "} ENUMSTRUCT;\n");
	fprintf (outputFile, "\n");

	/* Define array */
	fprintf (outputFile, "static ENUMSTRUCT %s[] = \n{\n", szVariableName);

	/* Process the file */
	while ( fgets (szLineBuff, 512, inputFile) != NULL)
	{
		char *tok = strtok (szLineBuff, " \t\n\r");
		
		while (tok)
		{
			if (!strcmp (tok, "{"))
			{
				while ( fgets (szLineBuff, 512, inputFile) != NULL)
				{
					long  toklen;
					
					tok = strtok (szLineBuff, " \t\n\r");
					if (!tok)
					{
						continue;
					}
					
					if ((tok[0] == '/') && (tok[1] == '*'))
					{
						continue;
					}
					
					if (tok[0] == '}')
					{
						break;
					}
					
					toklen = strlen (tok);
					
					if (tok[toklen - 1] == ',')
					{
						/* autoincrement ... */
						tok[toklen - 1] = '\0';
						--toklen;
						strcpy (szItemName, tok);

						tok = strtok (NULL, "\t\n\r");
					}                                                                      
					else
					{
						/* assigned a new ID */
						strcpy (szItemName, tok);
						tok = strtok (NULL, "\t\n\r");
						if (tok)
						{
							if (tok[0] == '=')
							{
								if (tok[1] == '\0')
								{
									tok = strtok (NULL, " ,\t\r\n");
								}
								else
								{
									++tok;
								}
						
								itemID = (unsigned int) strtol (tok, &szPos, 0);

								tok = strtok (NULL, "\t\n\r");
							}
							else
							{
								if ((tok[0] == '/') && (tok[1] == '*'))
								{
									/* comment */
								}
								else
								{
									printf ("Expected \'=\', got %s\n", tok);
									continue;
								}
							}
						}
					}
								
					if (tok)
					{
						char *EndCmt;
						
						while (tok && tok[0] && tok[1])
						{
							if ((tok[0] == '/') && (tok[1] == '*'))
							{
								tok += 2;

								/* remove leading spaces */
								while (tok[0] == ' ')
								{
									++tok;
								}
								break;
							}
							++tok;
						}
							
						EndCmt = strstr (tok, "*/");
						if (EndCmt)
						{
							while (EndCmt[-1] == ' ')
							{
								--EndCmt;
							}

							EndCmt[0] = '\0';
						}
					}
					
					if (nFirstTime)
					{
						nFirstTime = 0;
					}
					else
					{
						fprintf (outputFile, ",\n");
					}
					
					if (tok)
					{
						fprintf (outputFile, "\t{%u, \"%s\", \"%s\"}", itemID, szItemName, tok);
					}
					else
					{
						fprintf (outputFile, "\t{%u, \"%s\", \"\"}", itemID, szItemName);
					}
								
					++itemID;
					++nItems;
				}
			}
			tok = strtok (NULL, " \t\n\r");
		}
	}

	/* end of array def */
	fprintf (outputFile, "\n};\n\n");

	/* number of items */
	for (k = 0; k < strlen (szVariableName); ++k)
	{
		if (islower (szVariableName[k]))
		{
			szVariableName[k] -= 'a' - 'A';
		}
	}
/*	strupr (szVariableName);*/
	fprintf (outputFile, "#define NUM_ITEMS_IN_%s_ARRAY %d\n", szVariableName, nItems);

	/* Write string table resource tail */
	fprintf (outputFile,	"\n/* end of file */\n");

	/* Close the files */
	fclose (outputFile);
	fclose (inputFile);

	/* Success message */
	printf ("Resource file %s created\n", szOpFileName);
	
	return (ret);
}

