/*
 *   Copyright (c) 2018, tatt61880
 *
 *   This source code is released for free distribution under the terms of the
 *   GNU General Public License version 2 or (at your option) any later version.
 *
 *   This module contains functions for generating tags for Kuin language
 *   files.
 */

/*
 *   INCLUDE FILES
 */
#include "general.h"  /* must always come first */

#include <string.h>

#include "parse.h"
#include "read.h"
#include "routines.h"
#include "vstring.h"

/*
 *   DATA DEFINITIONS
 */
typedef enum {
	K_ALIAS,
	K_CLASS,
	K_CONST,
	K_ENUM,
	K_FUNC,
	K_VAR,
	K_NUM,
} kuinKind;

static char KuinKeywords[][16] = {
	"alias",
	"class",
	"const",
	"enum",
	"func",
	"var",
};

static kindDefinition KuinKinds [] = {
	{ true, 'a', "alias", "aliases"},
	{ true, 'C', "class", "classes"},
	{ true, 'c', "const", "constants" },
	{ true, 'E', "enum",  "enumeration types"},
	{ true, 'f', "func",  "functions" },
	{ true, 'v', "var",   "variables" },
};

/*
 *   FUNCTION DEFINITIONS
 */
static void findKuinTags (void)
{
	vString *name = vStringNew ();
	const unsigned char *line;

	bool line_top_flag = true;
	int comment_level = 0;
	int str_level = 0;
	while ((line = readLineFromInputFile ()) != NULL)
	{
		if (comment_level == 0)
		{
			line_top_flag = true;
		}
		while (*line != '\0')
		{
			if (comment_level != 0)
			{
				if (*line == '"')
				{
					do
					{
						if (*line == '\\')
						{
							line++;
							if (*line == '\0')
								break;
						}
						line++;
						if (*line == '"')
						{
							line++;
							break;
						}
					} while (*line != '\0');
				}
				else if (*line == '\'')
				{
					do
					{
						if (*line == '\\')
						{
							line++;
							if (*line == '\0')
								break;
						}
						line++;
						if (*line == '\'')
						{
							line++;
							break;
						}
					} while (*line != '\0');
				}
				else if (*line == ';')
				{
					do
					{
						line++;
					} while (*line != '\0');
				}
				else
				{
					if (*line == '{')
						comment_level++;
					else if (*line == '}')
						comment_level--;
					line++;
				}
			}
			else if (*line == ' ' || *line == '\t')
			{
				line++;
			}
			else if ('a' <= *line && *line <= 'z' || 'A' <= *line && *line <= 'Z' || *line == '_' || '0' <= *line && *line <= '9')
			{
				const char* str_begin = line;
				do
				{
					line++;
				} while ('a' <= *line && *line <= 'z' || 'A' <= *line && *line <= 'Z' || *line == '_' || '0' <= *line && *line <= '9');

				if (line_top_flag)
				{
					int word_len = (int)(line - str_begin);

					for (kuinKind kind = (kuinKind)0; kind < (kuinKind)K_NUM; kind++)
					{
						char *keyword = KuinKeywords[kind];
						int len = strlen(keyword);
						if (word_len == len && strncmp(str_begin, keyword, len) == 0)
						{
							do
							{
								line++;
							} while (*line != '\0' && isspace (*line));

							if ('a' <= *line && *line <= 'z' || 'A' <= *line && *line <= 'Z' || *line == '_' || '0' <= *line && *line <= '9')
							{
								vStringClear (name);
								do
								{
									vStringPut (name, (int) *line);
									line++;
								} while ('a' <= *line && *line <= 'z' || 'A' <= *line && *line <= 'Z' || *line == '_' || '0' <= *line && *line <= '9');
								makeSimpleTag (name, kind);
							}
						}
					}
				}
				line_top_flag = false;
			}
			else if (*line == '"' || str_level != 0 && *line == '}')
			{
				line_top_flag = false;
				if (*line == '}')
					str_level--;
				do
				{
					if (*line == '\\')
					{
						line++;
						if (*line == '\0')
							break;
						if (*line == '{')
						{
							str_level++;
							line++;
							break;
						}
					}
					line++;
					if (*line == '"')
					{
						line++;
						break;
					}
				} while (*line != '\0');
			}
			else if (*line == '\'')
			{
				line_top_flag = false;
				do
				{
					if (*line == '\\')
					{
						line++;
						if (*line == '\0')
							break;
					}
					line++;
					if (*line == '\'')
					{
						line++;
						break;
					}
				} while (*line != '\0');
			}
			else if (*line == '{')
			{
				comment_level = 1;
				line++;
			}
			else if (*line == ';')
			{
				do
				{
					line++;
				} while (*line != '\0');
			}
			else if (*line == '+' || *line == '*')
			{
				line++;
			}
			else
			{
				line_top_flag = false;
				line++;
			}
		}
	}
}

extern parserDefinition* KuinParser (void)
{
	static const char *const extensions [] = {
		"kn", NULL
	};
	parserDefinition* def = parserNew ("Kuin");
	def->extensions = extensions;
	def->kindTable  = KuinKinds;
	def->kindCount  = ARRAY_SIZE (KuinKinds);
	def->parser     = findKuinTags;
	return def;
}
