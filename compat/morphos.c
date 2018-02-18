#include <unistd.h>
#include <string.h>

pid_t getpgid(pid_t pid)
{
	return 667;
}

/*
 * ix_to_ados (c) 2005 Harry Sintonen <sintonen@iki.fi>
 * taken from the ixemul.library source code (GPLv2)
 */

char *ix_to_ados(char *buf, const char *name)
{
	char *p;
	const char *q = name;
	int allowdot = 1;
	int dotcnt = 0;
	int root = 0;
	int absados = 0;

	if(*q == '/')
	{
		root = 1;
		q++;
	}

#if 1
	/* allow amigados paths concatenated to nix ones */
	p = strrchr(q, ':');
	if(p)
	{
		dotcnt = -1;
		q = p + 1;
		while((p + dotcnt >= name) && p[dotcnt] != ':' && p[dotcnt] != '/')
			dotcnt--;
		memcpy(buf, p + dotcnt + 1, -dotcnt);
		p = buf - dotcnt;

		root = 0;
		dotcnt = 0;
		absados = 1;
	}
	else
#endif
		p = buf;

	for(;;)
	{
		char c = *q++;

		switch(c)
		{
		case '.':
			if(allowdot)
				dotcnt++;
			else
				*p++ = '.';
			break;

		case '/':
		case '\0':
			if(dotcnt > 2)
			{
				int cnt = dotcnt;
				while(cnt--)
					*p++ = '.';
				if(root == 1)
					root++;
			}
			if(root == 2 || (root == 1 && c == '\0'))
			{
				*p++ = ':';
				root = 0;
			}
			else if((root == 0 && absados == 0) &&
			        (dotcnt == 2 ||                              /* "/../" */
			         ((dotcnt == 0 || dotcnt > 2) && c != '\0'))) /* normal "/" */
			{
				*p++ = '/';
			}

			if(c == '\0')
				goto out;

			while(*q == '/')
				q++;

			allowdot = 1;
			dotcnt = 0;
			break;

		default:
			if(root == 1)
				root++;
			while(dotcnt--)
				*p++ = '.';
			*p++ = c;
			allowdot = 0;
			dotcnt = 0;
			absados = 0;
			break;
		}
	}
out:

	/* remove lonely '/' from the end */
	if(p > buf + 2 && p[-1] == '/' && p[-2] != '/' && p[-2] != ':')
		p--;

	*p = '\0';

	if(!strcasecmp(buf, "console:"))
	{
		buf[0] = '*';
		buf[1] = '\0';
	}
	else if(!strncmp(buf, "dev:", 4))
	{
		if(!strcmp(buf + 4, "tty"))
		{
			buf[0] = '*';
			buf[1] = '\0';
		}
		else if(!strcmp(buf + 4, "null"))
		{
			strcpy(buf, "NIL:");
		}
		else if(!strcmp(buf + 4, "random") || !strcmp(buf + 4, "urandom") || !strcmp(buf + 4, "srandom") || !strcmp(buf + 4, "prandom"))
		{
			strcpy(buf, "RANDOM:");
		}
		else if(!strcmp(buf + 4, "zero"))
		{
			strcpy(buf, "ZERO:");
		}
	}

	return buf;
}
