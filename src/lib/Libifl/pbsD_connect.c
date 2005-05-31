/*
*         OpenPBS (Portable Batch System) v2.3 Software License
* 
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
* 
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
* 
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
* 
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
* 
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
* 
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
* 
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
* 
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
* 
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
* 
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
* 
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information 
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
* 
* 7. DISCLAIMER OF WARRANTY
* 
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
* 
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/
/*	pbs_connect.c
 *
 *	Open a connection with the pbs server.  At this point several
 *	things are stubbed out, and other things are hard-wired.
 *
 */

#include <pbs_config.h>   /* the master config generated by configure */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "libpbs.h"
#include "dis.h"
#include "net_connect.h"

static uid_t pbs_current_uid;

extern char pbs_current_user[PBS_MAXUSER];
extern struct connect_handle connection[NCONNECTS];
extern time_t pbs_tcp_timeout;

static unsigned int dflt_port = 0;
static char dflt_server[PBS_MAXSERVERNAME+1];
static int got_dflt = FALSE;
static char server_name[PBS_MAXSERVERNAME+1];
static unsigned int server_port;
static char *pbs_destn_file = PBS_DEFAULT_FILE;

char *pbs_server = 0;


char *pbs_default()

  {
  FILE *fd;
  char *pn;
  char *server;

  if (got_dflt != TRUE) 
    {
    server = getenv("PBS_DEFAULT");

    if ((server == NULL) || (*server == '\0')) 
      {
      fd = fopen(pbs_destn_file,"r");

      if (fd == NULL) 
        {
        char tmpLine[1024];

        /* attempt again with parent */

        snprintf(tmpLine,sizeof(tmpLine),"../%s",
          pbs_destn_file);

        if ((fd = fopen(pbs_destn_file,"r")) == NULL)
          {
          return(NULL);
          }
        }

      fgets(dflt_server,PBS_MAXSERVERNAME,fd);

      if ((pn = strchr(dflt_server,(int)'\n')))
        *pn = '\0';

      fclose(fd);
      } 
    else 
      {
      strncpy(dflt_server,server,PBS_MAXSERVERNAME);
      }

    got_dflt = TRUE;
    }  /* END if (got_dflt != TRUE) */

  strcpy(server_name,dflt_server);

  return(dflt_server);
  }  /* END pbs_default() */




static char *PBS_get_server(

  char         *server,
  unsigned int *port)

  {
  int   i;
  char *pc;
	
  for (i = 0;i < PBS_MAXSERVERNAME + 1;i++) 
    server_name[i] = '\0';
	
  if (dflt_port == 0) 
    {
    dflt_port = get_svrport(
      PBS_BATCH_SERVICE_NAME, 
      "tcp",
      PBS_BATCH_SERVICE_PORT_DIS);
    }

  /* first, get the "net.address[:port]" into 'server_name' */
		
  if ((server == (char *)NULL) || (*server == '\0')) 
    {
    if (pbs_default() == NULL)
      {
      return(NULL);
      }
    } 
  else 
    {
    strncpy(server_name,server,PBS_MAXSERVERNAME);
    }
	
  /* now parse out the parts from 'server_name' */

  if ((pc = strchr(server_name,(int)':'))) 
    {
    /* got a port number */

    *pc++ = '\0';

    *port = atoi(pc);
    } 
  else 
    {
    *port = dflt_port;
    }

  return(server_name);
  }  /* END PBS_get_server() */





/*
 * PBS_authenticate - call pbs_iff(1) to authenticate use to the PBS server.
 */

static int PBSD_authenticate(

  int psock)  /* I */

  {
  char   cmd[PBS_MAXSERVERNAME + 80];
  int    cred_type;
  int    i;
  int    j;
  FILE	*piff;
  char  *ptr;

  /* use pbs_iff to authenticate me */

  ptr = getenv("PBSBINDIR");

  if (ptr == NULL)
    {
    sprintf(cmd,"%s %s %u %d", 
      IFF_PATH, 
      server_name, 
      server_port,
      psock);
    }
  else
    {
    sprintf(cmd,"%s/%s %s %u %d",
      ptr,
      "pbs_iff",
      server_name,
      server_port,
      psock);
    }

  piff = popen(cmd,"r");

  if (piff == NULL)
    {
    /* FAILURE */

    if (getenv("PBSDEBUG"))
      fprintf(stderr,"ALERT:  cannot open pipe, errno=%d (%s)\n",
        errno,
        strerror(errno));

    return(-1);
    }

  i = read(fileno(piff),&cred_type,sizeof(int));

  if ((i != sizeof(int)) || (cred_type != PBS_credentialtype_none))
    {
    /* FAILURE */

    if (getenv("PBSDEBUG"))
      {
      if (i != sizeof(int))
        {
        fprintf(stderr,"ALERT:  cannot read pipe, rc=%d, errno=%d (%s)\n",
          i,
          errno,
          strerror(errno));
        }
      else
        {
        fprintf(stderr,"ALERT:  invalid cred type %d reported\n",
          cred_type);
        }
      }

    pclose(piff);

    return(-1);
    }

  j = pclose(piff);

  if (j != 0)
    {
    /* FAILURE */

    if (getenv("PBSDEBUG"))
      fprintf(stderr,"ALERT:  cannot close pipe, errno=%d (%s)\n",
        errno,
        strerror(errno));

    /* report failure but do not fail (CRI) */

    /* return(-1); */
    }

  /* SUCCESS */

  return(0);
  }  /* END PBSD_authenticate() */




/* returns socket descriptor or -1 on failure */

int pbs_connect(

  char *server)  /* I */

  {
  struct sockaddr_in server_addr;
  struct hostent *hp;
  struct hostent *gethostbyname();
  int out;
  int i;
  struct passwd *pw;

  char  *ptr;

  /* reserve a connection state record */
	
  out = -1;

  for (i = 1;i < NCONNECTS;i++) 
    {
    if (connection[i].ch_inuse) 
      continue;

    out = i;

    connection[out].ch_inuse  = 1;
    connection[out].ch_errno  = 0;
    connection[out].ch_socket = -1;
    connection[out].ch_errtxt = NULL;

    break;
    }

  if (out < 0) 
    {
    pbs_errno = PBSE_NOCONNECTS;

    if (getenv("PBSDEBUG"))
      fprintf(stderr,"ALERT:  cannot locate free channel\n");

    return(-1);
    }
	
  /* get server host and port */
	
  server = PBS_get_server(server,&server_port);

  if (server == NULL) 
    {
    connection[out].ch_inuse = 0;
    pbs_errno = PBSE_NOSERVER;

    if (getenv("PBSDEBUG"))
      fprintf(stderr,"ALERT:  PBS_get_server() failed\n");

    return(-1);
    }
		
  /* determine who we are */

  pbs_current_uid = getuid();

  if ((pw = getpwuid(pbs_current_uid)) == NULL) 
    {
    pbs_errno = PBSE_SYSTEM;

    if (getenv("PBSDEBUG"))
      {
      fprintf(stderr,"ALERT:  cannot get password info for uid %ld\n",
        (long)pbs_current_uid);
      }

    return(-1);
    }

  strcpy(pbs_current_user,pw->pw_name);

  /* get socket	*/

  connection[out].ch_socket = socket(AF_INET,SOCK_STREAM,0);

  if (connection[out].ch_socket < 0) 
    {
    if (getenv("PBSDEBUG"))
      {
      fprintf(stderr,"ERROR:  cannot create socket:  errno=%d (%s)\n",
        errno,
        strerror(errno));
      }

    connection[out].ch_inuse = 0;
    pbs_errno = PBSE_PROTOCOL;

    return(-1);
    }

  /* and connect... */

  pbs_server = server;    /* set for error messages from commands */
	
  server_addr.sin_family = AF_INET;
  hp = NULL;
  hp = gethostbyname(server);

  if (hp == NULL) 
    {
    close(connection[out].ch_socket);
    connection[out].ch_inuse = 0;
    pbs_errno = PBSE_BADHOST;

    if (getenv("PBSDEBUG"))
      {
      fprintf(stderr,"ERROR:  cannot get servername (%s) errno=%d (%s)\n",
        (server != NULL) ? server : "NULL",
        errno,
        strerror(errno));
      }

    return(-1);
    }

  memcpy((char *)&server_addr.sin_addr,hp->h_addr_list[0],hp->h_length);
  server_addr.sin_port = htons(server_port);
	
  if (connect(
        connection[out].ch_socket,
        (struct sockaddr *)&server_addr,
        sizeof(server_addr)) < 0) 
    {
    close(connection[out].ch_socket);

    connection[out].ch_inuse = 0;
    pbs_errno = errno;

    if (getenv("PBSDEBUG"))
      {
      fprintf(stderr,"ERROR:  cannot connect to server, errno=%d (%s)\n",
        errno,
        strerror(errno));
      }

    return(-1);
    }

  /* Have pbs_iff authencate connection */

  if (PBSD_authenticate(connection[out].ch_socket) != 0) 
    {
    close(connection[out].ch_socket);

    connection[out].ch_inuse = 0;

    pbs_errno = PBSE_PERM;

    if (getenv("PBSDEBUG"))
      {
      fprintf(stderr,"ERROR:  cannot authenticate connection, errno=%d (%s)\n",
        errno,
        strerror(errno));
      }

    return(-1);
    }

  /* setup DIS support routines for following pbs_* calls */

  DIS_tcp_setup(connection[out].ch_socket);

  if ((ptr = getenv("PBSAPITIMEOUT")) != NULL)
    {
    pbs_tcp_timeout = (int)strtol(ptr,NULL,0);	/* set for 3 hour time out */
    }
  else
    {
    pbs_tcp_timeout = 10800;      /* set for 3 hour time out */
    }

  return(out);
  }  /* END pbs_connect() */





int pbs_disconnect(

  int connect)  /* I (socket descriptor) */

  {
  int  sock;
  static char x[THE_BUF_SIZE / 4];

  /* send close-connection message */

  sock = connection[connect].ch_socket;

  DIS_tcp_setup(sock);

  if ((encode_DIS_ReqHdr(sock,PBS_BATCH_Disconnect,pbs_current_user) == 0) && 
      (DIS_tcp_wflush(sock) == 0)) 
    {
    int atime;

    /* set alarm to break out of potentially infinite read */

    atime = alarm(10);

    while (1) 
      {	
      /* wait for server to close connection */

      /* NOTE:  if read of 'sock' is blocking, request below may hang forever */

      if (read(sock,&x,sizeof(x)) < 1) 
        break;
      }

    alarm(atime);
    }
	
  close(sock);

  if (connection[connect].ch_errtxt != (char *)NULL) 
    free(connection[connect].ch_errtxt);

  connection[connect].ch_errno = 0;
  connection[connect].ch_inuse = 0;

  return(0);
  }  /* END pbs_disconnect() */




int pbs_query_max_connections()

  {
  return(NCONNECTS - 1);
  }


/* END pbsD_connect.c */

