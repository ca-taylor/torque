#define NEED_BLOCKING_CONNECTIONS
#include <pbs_config.h>   /* the master config generated by configure */

#include "license_pbs.h" /* See here for the software license */
#include "lib_net.h"

#include <stdio.h> /* perror */
#include <unistd.h> /* close, usleep, read, write */

#include <string.h> /* memset */
#include <sys/ioctl.h> /* ioctl, FIONREAD */
#include <signal.h> /* Signals SIGPIPE, etc */
#include <sys/types.h>
#include <sys/socket.h> /* Socket communication */
#include <sys/un.h>
#include <netdb.h> /* struct addrinfo */
#include <netinet/in.h> /* Internet domain sockets */
#include <arpa/inet.h> /* in_addr_t */
#include <netinet/tcp.h>
#include <errno.h> /* errno */
#include <fcntl.h> /* fcntl, F_GETFL */
#include <sys/time.h> /* gettimeofday */
#include <poll.h> /* poll functionality */
#include <iostream>
#include "../lib/Liblog/pbs_log.h" /* log_err */
#include "log.h" /* LOCAL_LOG_BUF_SIZE */
#include "net_cache.h"

#include "pbs_error.h" /* torque error codes */

#include "../lib/Libifl/lib_ifl.h"

using std::cerr;

extern time_t pbs_tcp_timeout; /* located in tcp_dis.c. Move here later */

#define RES_PORT_START 144
#define RES_PORT_END (IPPORT_RESERVED - 1)
#define RES_PORT_RANGE (RES_PORT_END - RES_PORT_START + 1)
#define RES_PORT_RETRY 50 /* This need to be large enough to guarantee we get a privilege port that is not in use */
#define PBS_NET_RC_RETRY -2
#define TCP_PROTO_NUM 0
#define MAX_NUM_LEN 21

#define MAX_USED_PRIV_PORTS 15
int used_priv_ports[MAX_USED_PRIV_PORTS];
int used_index;

unsigned availBytesOnDescriptor(
    
  int pLocalSocket)

  {
  unsigned availBytes;
  if (ioctl(pLocalSocket, FIONREAD, &availBytes) != -1)
    return availBytes;
  perror("availBytes");
  return(0);
  } /* END availBytesOnDescriptor() */




int socket_avail_bytes_on_descriptor(
    
  int socket)

  {
  unsigned avail_bytes;
  if (ioctl(socket, FIONREAD, &avail_bytes) != -1)
    return avail_bytes;
  return(0);
  } /* END socket_avail_bytes_on_descriptor() */


int socket_get_unix()
  {
  int fd;
  int rc;

  fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd < 0)
    {
    rc = PBSE_SOCKET_FAULT * -1;
    return(rc);
    }

  return(fd);
  }



int socket_get_tcp()

  {
  int local_socket = 0;
  struct linger l_delay;
  int on = 1;

  (void) signal(SIGPIPE, SIG_IGN);
  memset(&l_delay, 0, sizeof(struct linger));
  l_delay.l_onoff = 0;
  if ((local_socket = socket(PF_INET, SOCK_STREAM, TCP_PROTO_NUM)) == -1)
    {
    local_socket = -2;
    }
  else if (setsockopt(local_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
    close(local_socket);
    local_socket = -3;
    }
  else if (setsockopt(local_socket, SOL_SOCKET, SO_LINGER, &l_delay, sizeof(struct linger)) == -1)
    {
    close(local_socket);
    local_socket = -4;
    }
  return local_socket;
  } /* END socket_get_tcp() */




int get_listen_socket(
    
  struct addrinfo *addr_info)

  {
  int local_socket = 0;
  struct linger l_delay;
  int on = 1;
  
  (void) signal(SIGPIPE, SIG_IGN);
  memset(&l_delay, 0, sizeof(struct linger));
  l_delay.l_onoff = 0;

  if ((local_socket = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol)) == -1)
    {
    local_socket = -2;
    }
  else if (setsockopt(local_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
    close(local_socket);
    local_socket = -3;
    }
  else if (setsockopt(local_socket, SOL_SOCKET, SO_LINGER, &l_delay, sizeof(struct linger)) == -1)
    {
    close(local_socket);
    local_socket = -4;
    }

  return(local_socket);
  } /* END get_listen_socket() */
   


int get_random_reserved_port()

  {
  int res_port = 0;
  int i;
  int done = 0;
  int found;

  do
    {
    found = 0;
    res_port = (rand() % RES_PORT_RANGE) + RES_PORT_START;
    for (i = 0; i < MAX_USED_PRIV_PORTS; i++)
      {
      if (res_port == used_priv_ports[i])
        found = 1;
      }
    if (!found)
      {
      used_priv_ports[used_index++] = res_port;
      if (used_index >= MAX_USED_PRIV_PORTS)
        used_index = 0;
      done = 1;
      }
    else
      usleep(50000);
    } while(!done);

  return res_port;
  } /* END get_random_reserved_port() */




int socket_get_tcp_priv()

  {
  int                local_socket = 0;
  int                rc = PBSE_NONE;
  struct sockaddr_in local;
#ifndef NOPRIVPORTS
  int                cntr = 0;
  int                priv_port = 0;
  int                flags;
#endif
  
  memset(&local, 0, sizeof(struct sockaddr_in));
  local.sin_family = AF_INET;
  
  if ((local_socket = socket_get_tcp()) < 0)
    return(-1);

#ifndef NOPRIVPORTS
  /* According to the notes in the previous code:
   * bindresvport seems to cause connect() failures in some odd corner case
   * when talking to a local daemon.  So we'll only try this once and
   * fallback to the slow loop around bind() if connect() fails
   * with EADDRINUSE or EADDRNOTAVAIL.
   * http://www.supercluster.org/pipermail/torqueusers/2006-June/003740.html
   */

  flags = fcntl(local_socket, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(local_socket, F_SETFL, flags);

  priv_port = get_random_reserved_port();
  while (cntr < RES_PORT_RETRY)
    {
    if (++priv_port >= RES_PORT_END)
      priv_port = RES_PORT_START;
    local.sin_port = htons(priv_port);
    if ((rc = bind(local_socket, (struct sockaddr *)&local, sizeof(struct sockaddr))) < 0)
      {
      if ((errno == EADDRINUSE) ||
           (errno == EADDRNOTAVAIL) ||
           (errno == EINVAL) ||
           (errno == EINPROGRESS))
        {
        cntr++;
        }
      else
        {
        cntr = RES_PORT_RETRY;
        break;
        }

      }
    else
      {
      rc = PBSE_NONE;
      break;
      }
    }

  if (cntr >= RES_PORT_RETRY)
    {
    close(local_socket);
    rc = PBSE_SOCKET_FAULT;
    errno = PBSE_SOCKET_FAULT;
    local_socket = -1;
    }
#else
  rc = bind(local_socket, (struct sockaddr *)&local, sizeof(struct sockaddr));

  if (rc != 0)
    {
    close(local_socket);
    local_socket = -1;
    }
#endif

  if (rc != PBSE_NONE)
    {
    local_socket = -1;
    }

  return(local_socket);
  } /* END socket_get_tcp_priv() */

int socket_connect_unix(

  int   local_socket,
  const char *sock_name,
  char **error_msg)

  {
  int rc;
  struct sockaddr_un addr;
  char tmp_buf[LOCAL_LOG_BUF_SIZE+1];

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", sock_name);

  rc = connect(local_socket, (struct sockaddr *)&addr, sizeof(addr));
  if (rc < 0)
    {
    snprintf(tmp_buf, sizeof(tmp_buf), "could not connect to unix socket %s: %d", sock_name, errno);
    *error_msg = strdup(tmp_buf);
    rc = PBSE_DOMAIN_SOCKET_FAULT;
    }

  return(rc);
  }


int socket_connect(

  int   *local_socket,
  char  *dest_addr,
  int    dest_addr_len,
  int    dest_port,
  int    family,
  int    is_privileged,
  char **error_msg)

  {
  struct sockaddr_in remote;
  size_t r_size = sizeof(struct sockaddr_in);

  memset(&remote, 0, r_size);
  remote.sin_family = family;
  memcpy(&remote.sin_addr, dest_addr, dest_addr_len);
  remote.sin_port = htons((unsigned short)dest_port);
  return socket_connect_addr(local_socket, (struct sockaddr *)&remote, r_size, is_privileged, error_msg);
  } /* END socket_connect() */



/*
 * socket_connect_addr()
 *
 * connects socket to the remote address specified
 * @param socket - the socket that will be connected. On failure, set to a permanent or transient
 * failure code.
 * @param remote - the address that socket should be connected to
 * @param remote_size - the size of the memory remote points to
 * @param is_privileged - indicates whether socket is bound to a privileged port or not
 * @param error_msg - pointer to an error msg buffer
 */

int socket_connect_addr(
    
  int              *socket,
  struct sockaddr  *remote,
  size_t            remote_size,
  int               is_privileged,
  char            **error_msg)

  {
  int   cntr = 0;
  int   rc = PBSE_NONE;
  char  tmp_buf[LOCAL_LOG_BUF_SIZE+1];
  int   local_socket = *socket;

  while ((cntr < RES_PORT_RETRY) &&
         ((rc = connect(local_socket, remote, remote_size)) != 0))
    {
    cntr++;
    
    switch (errno)
      {
      /* permanent failures go here */
      case ECONNREFUSED:    /* Connection refused */
      case ETIMEDOUT:       /* Connection timed out */
        snprintf(tmp_buf, sizeof(tmp_buf), "cannot connect to port %d in %s - connection refused",
          local_socket, __func__);
        *error_msg = strdup(tmp_buf);
        rc = PBS_NET_RC_RETRY;
        close(local_socket);
        local_socket = PERMANENT_SOCKET_FAIL;
        break;

      case EINPROGRESS:   /* Operation now in progress */
      case EALREADY:    /* Operation already in progress */
      case EISCONN:   /* Transport endpoint is already connected */
      case EAGAIN:    /* Operation would block */
      case EINTR:     /* Interrupted system call */

        if ((rc = socket_wait_for_write(local_socket)) == PBSE_NONE)
          {
          /* no network failures detected, socket available */
          break;
          }
        else if (rc == PERMANENT_SOCKET_FAIL)
          {
          close(local_socket);
          return(rc);
          }

        /* essentially, only fall through for a transient failure */

      /* socket not ready for writing after 5 timeout */
      case EINVAL:    /* Invalid argument */
      case EADDRINUSE:    /* Address already in use */
      case EADDRNOTAVAIL:   /* Cannot assign requested address */
      case EBADF:          /* something is wrong with the socket descriptor */

        if (is_privileged)
          {
          rc = PBSE_SOCKET_FAULT;
          /* 3 connect attempts are made to each socket */
          /* Fail on RES_PORT_RETRY */
          close(local_socket);
          local_socket = TRANSIENT_SOCKET_FAIL;

          while (cntr < RES_PORT_RETRY)
            {
            if ((local_socket = socket_get_tcp_priv()) < 0)
              cntr++;
            else
              {
              rc = PBSE_NONE;
              break;
              }
            }

          }
        break;

      default:

        snprintf(tmp_buf, sizeof(tmp_buf), "cannot connect with socket %d in %s - errno:%d %s",
          local_socket, __func__, errno, strerror(errno));
        *error_msg = strdup(tmp_buf);
        close(local_socket);
        rc = PBSE_SOCKET_FAULT;
        local_socket = PERMANENT_SOCKET_FAIL;

        break;
      }

    if (local_socket == PERMANENT_SOCKET_FAIL)
      break;
    }

  *socket = local_socket;
  
  if ((local_socket >= 0) &&
      (rc != PBSE_NONE))
    close(local_socket);

  return(rc);
  } /* END socket_connect_addr() */



/*
 * process_and_save_socket_error()
 * Evaluates an error read from a socket to determine if it is a retryable error or not. 
 * For our purposes, timeouts are not retryable.
 *
 * @post-cond: errno is updated to sock_errno, an error read from the socket.
 *
 * @return TRANSIENT_SOCKET_FAIL if the error is retryable, PERMANENT_SOCKET_FAIL otherwise
 */

int process_and_save_socket_error(

  int sock_errno)

  {
  int rc = PBSE_NONE;

  errno = sock_errno;

  switch (sock_errno)
    {
    /* ETIMEDOUT is not listed because it should be considered a permanent failure */
    case EINPROGRESS:
    case EALREADY:    /* Operation already in progress */
    case EISCONN:   /* Transport endpoint is already connected */
    case EAGAIN:    /* Operation would block */
    case EINTR:     /* Interrupted system call */
    case EINVAL:    /* Invalid argument */
    case EADDRINUSE:    /* Address already in use */
    case EADDRNOTAVAIL:   /* Cannot assign requested address */

      rc = TRANSIENT_SOCKET_FAIL;

      break;

    default:

      rc = PERMANENT_SOCKET_FAIL;

      break;
    }

  return(rc);
  }



/*
 * socket_wait_for_write()
 *
 * connect failed, this function determines why. For non-blocking sockets,
 * EINPROGRESS is almost always set, and you need to select the socket and then
 * read error using getsockopt(), as is done below.
 *
 * if the failure is a permanent failure, pass that back to the caller
 *
 * @post-cond: errno will be populated with the correct error condition
 *
 * @return TRANSIENT_SOCKET_FAIL if its not permanent, PERMANENT_SOCKET_FAIL
 * if it is permanent.
 */

int socket_wait_for_write(
    
  int socket)

  {
  int            rc = PBSE_NONE;
  int            write_soc = 0;
  int            sock_errno;
  socklen_t      len = sizeof(int);
  fd_set         wfd;
  struct timeval timeout;

  timeout.tv_sec = pbs_tcp_timeout;
  timeout.tv_usec = 0;

  FD_ZERO(&wfd);
  FD_SET(socket, &wfd);

  if ((write_soc = select(socket+1, 0, &wfd, 0, &timeout)) != 1)
    {
    /* timeout is now seen as a permanent failure */
    rc = PERMANENT_SOCKET_FAIL;
    }
  else if (((rc = getsockopt(socket, SOL_SOCKET, SO_ERROR, &sock_errno, &len)) == 0) && 
           (sock_errno == 0))
    {
    rc = PBSE_NONE;
    }
  else
    {
    rc = process_and_save_socket_error(sock_errno);
    }

  return(rc);
  } /* END socket_wait_for_write() */



int socket_wait_for_xbytes(
    
  int socket,
  int len)

  {
  int rc = PBSE_NONE;
  int avail_bytes = socket_avail_bytes_on_descriptor(socket);
  while (avail_bytes < len)
    {
    if ((rc = socket_wait_for_read(socket)) == PBSE_NONE)
      {
      avail_bytes = socket_avail_bytes_on_descriptor(socket);
      if (avail_bytes < len)
        usleep(1);
      }
    else
      {
      break;
      }
    }
  return rc;
  } /* END socket_wait_for_xbytes() */




int socket_wait_for_read(
    
  int socket)

  {
  int           rc = PBSE_NONE;
  int           ret;
  struct pollfd pfd;

  pfd.fd = socket;
  pfd.events = POLLIN | POLLHUP; /* | POLLRDNORM; */
  pfd.revents = 0;

  ret = poll(&pfd, 1, pbs_tcp_timeout * 1000); /* poll's timeout is in milliseconds */
  if (ret > 0)
    {
    char buf[8];
    if (recv(socket, buf, 7, MSG_PEEK | MSG_DONTWAIT) == 0)
      {
      /* This will only occur when the socket has closed */
      rc = PBSE_SOCKET_CLOSE;
      }
    }
  else if (ret == 0)
    {
    /* Server timeout reached */
    rc = PBSE_TIMEOUT;
    }
  else /* something bad happened to poll */
    {
    if (pfd.revents & POLLNVAL)
      {
      rc = PBSE_SOCKET_CLOSE;
      }
    else
      rc = PBSE_SOCKET_DATA;
    }


  return(rc);
  } /* END socket_wait_for_read() */




void socket_read_flush(
    
  int socket)

  {
  char incoming[256];
  int  avail_bytes = 0;
  int  i;
  
  while ((avail_bytes = socket_avail_bytes_on_descriptor(socket)) > 0)
    {
    if (avail_bytes > 256)
      avail_bytes = 256; 
    i = read_ac_socket(socket, incoming, avail_bytes);
    if (i < 0)
      break;
    }
  } /* END socket_read_flush() */




int socket_write(
    
  int         socket,
  const char *data,
  int         data_len)

  {
  int data_len_actual = -1;
  if ((data != NULL) &&
      (data_len > 0))
    {
    data_len_actual = write(socket, data, data_len);
    if (data_len_actual == -1)
      printf("Error (%d-%s) writing %d bytes to socket (write_socket) data [%s]\n",
        errno, strerror(errno), data_len, data);
    else if (data_len_actual != data_len)
      {
      printf("Error (%d-%s)writing data to socket (tried to send %d chars, actual %d)\n",
        errno, strerror(errno), data_len, data_len_actual);
      data_len_actual = data_len_actual - data_len;
      }
    }

  return(data_len_actual);
  } /* END socket_write() */




int socket_read_force(

  int        socket,
  char      *the_str,
  long long  avail_bytes,
  long long *byte_count)

  {
  int rc = PBSE_NONE;
  char *read_loc = the_str;
  long long tmp_len = avail_bytes;
  long long bytes_read = 1;
  long long sock_check = 0;
  char log_buf[LOCAL_LOG_BUF_SIZE+1];
  while (bytes_read != 0)
    {
    bytes_read = read_ac_socket(socket, read_loc, tmp_len);
    if ((bytes_read == -1) && (errno != EINTR))
      {
      if (getenv("PBSDEBUG") != NULL)
        fprintf(stderr, "Error reading data from socket %d - (%s)\n",
            errno, strerror(errno));
      rc = PBSE_SOCKET_READ;
      break;
      }
    else if (bytes_read == 0)
      {
      if (*byte_count == 0)
        rc = PBSE_SOCKET_READ;
      break;
      }
    else if (bytes_read == avail_bytes)
      {
      *byte_count += bytes_read;
      break;
      }
    else
      {
      tmp_len -= bytes_read;
      read_loc += bytes_read;
      *byte_count += bytes_read;
      sock_check = socket_avail_bytes_on_descriptor(socket);
      if (sock_check == 0)
        {
        snprintf(log_buf, LOCAL_LOG_BUF_SIZE, "ioctl hsa been lying, expected avail %lld, actual avail %lld", tmp_len, sock_check);
        log_record(PBSEVENT_SYSTEM, PBS_EVENTCLASS_REQUEST, __func__, log_buf);
        break;
        }
      if (sock_check < tmp_len)
        tmp_len = sock_check;
      }
    }
  return rc;
  } /* END socket_read_force() */




int socket_read(
    
  int         socket,
  char      **the_str,
  long long  *str_len)

  {
  int       rc = PBSE_NONE;
  long long avail_bytes = socket_avail_bytes_on_descriptor(socket);
  long long byte_count = 0;

  if ((the_str == NULL) || (str_len == NULL))
    return PBSE_INTERNAL;

  while (avail_bytes == 0)
    {
    if ((rc = socket_wait_for_read(socket)) != PBSE_NONE)
      break;
    avail_bytes = socket_avail_bytes_on_descriptor(socket);
    if (avail_bytes == 0)
      {
      rc = PBSE_SOCKET_READ;
      break;
      }
    }

  if (rc != PBSE_NONE)
    {
    }
  else if ((*the_str = (char *)calloc(1, avail_bytes+1)) == NULL)
    {
    rc = PBSE_MEM_MALLOC;
    }
  else if ((rc = socket_read_force(socket, *the_str, avail_bytes, &byte_count))
      != PBSE_NONE)
    {
    }
  else
    {
    if (getenv("PBSDEBUG") != NULL)
      if (byte_count != avail_bytes)
        fprintf(stderr, "Bytes on socket = %lld, bytes read = %lld\n",
            avail_bytes, byte_count);
    *str_len = byte_count;
    }

  return(rc);
  } /* END socket_read() */




int socket_read_one_byte(
    
  int   socket,
  char *one_char)

  {
  int rc = PBSE_NONE;
  int avail_bytes = socket_avail_bytes_on_descriptor(socket);
  if (avail_bytes <= 0)
    rc = socket_wait_for_read(socket);
  if (rc == PBSE_NONE)
    {
    if (read_ac_socket(socket, one_char, 1) != 1)
      rc = PBSE_SOCKET_READ;
    else
      rc = PBSE_NONE;
    }
  return rc;
  } /* END socket_read_one_byte() */




int socket_read_num(
    
  int        socket,
  long long *the_num)

  {
  int  rc =  PBSE_INTERNAL;
  int  pos = 0;
  char str_ll[MAX_NUM_LEN];
  char tmp_char = '\0';
  int  avail_bytes = socket_avail_bytes_on_descriptor(socket);

  memset(str_ll, 0, MAX_NUM_LEN);

  while (1)
    {
    if (avail_bytes == 0)
      {
      /* Wait until there is activity on the socket .... */
      if ((rc = socket_wait_for_read(socket)) != PBSE_NONE)
        break;
      avail_bytes = socket_avail_bytes_on_descriptor(socket);
      if (avail_bytes == 0)
        {
        rc = PBSE_SOCKET_READ;
        break;
        }
      }
    else if (read_ac_socket(socket, &tmp_char, 1) == -1)
      break;
    else if (pos >= (int)sizeof(str_ll) - 1)
      break;
    else if ((tmp_char >= '0' && tmp_char <= '9') || (tmp_char == '-'))
      {
      str_ll[pos++] = tmp_char;
      avail_bytes--;
      }
    else if (tmp_char == '|')
      {
      if (pos != 0)
        {
        *the_num = atol(str_ll);
        rc = PBSE_NONE;
        break;
        }
      }
    else
      break;
    }

  if ((str_ll[0] == '\0') &&
      (rc == PBSE_INTERNAL))
    rc = PBSE_SOCKET_READ;

  return(rc);
  } /* END socket_read_num() */




/* memory for "the_str" is allocated in inside this function.
 * "str_len" is not.
 */
int socket_read_str(
    
  int         socket,
  char      **the_str,
  long long  *str_len)

  {
  int rc =  PBSE_NONE;
  long long tmp_len = 0;
  long long read_len = 0;
  char delin;

  if ((the_str == NULL) || (str_len == NULL))
    return(PBSE_INTERNAL);

  if ((rc = socket_read_num(socket, (long long *)&tmp_len)) != PBSE_NONE)
    {
    }
  else if (tmp_len == 0)
    {
    /* Valid case, NULL string */
    *the_str = NULL;
    *str_len = 0;
    rc = PBSE_NONE;
    }
  else if ((*the_str = (char *)calloc(1, (tmp_len)+1)) == NULL)
    {
    rc = PBSE_INTERNAL;
    }
  /* This is where the select would go until the availbytes is > the tmp_len */
  else if ((rc = socket_wait_for_xbytes(socket, tmp_len)) != PBSE_NONE)
    {
    }
  else if (socket_read_force(socket, *the_str, tmp_len, &read_len) == -1)
    {
    rc = PBSE_INTERNAL;
    }
  else if (read_ac_socket(socket, &delin, 1) != 1)
    {
    rc = PBSE_INTERNAL;
    }
  else if (delin != '|')
    {
    rc = PBSE_INTERNAL;
    }
  else
    {
    if (getenv("PBSDEBUG") != NULL)
      if (read_len != tmp_len)
        fprintf(stderr, "Bytes on socket = %lld, bytes read = %lld\n",
            tmp_len, read_len);
    *str_len = tmp_len;
    /* SUCCESS */
    }
  return rc;
  } /* socket_read_str() */




int socket_close(
    
  int socket)

  {
  int rc;
  socket_read_flush(socket);
  if ((rc = close(socket)) == 0)
    rc = PBSE_NONE;
  else
    rc = PBSE_SYSTEM;
  return rc;
  } /* END socket_close() */


int pbs_getaddrinfo(
    
  const char       *pNode,
  struct addrinfo  *pHints,
  struct addrinfo **ppAddrInfoOut)

  {
  int rc;
  struct addrinfo hints;
  int retryCount = 3;
  int addrFound = FALSE;

  if (ppAddrInfoOut == NULL)
    {
    return -1;
    }

  if ((*ppAddrInfoOut = get_cached_addrinfo_full(pNode)) != NULL)
    {
    return 0;
    }

  if (pHints == NULL)
    {
    memset(&hints,0,sizeof(hints));
    hints.ai_flags = AI_CANONNAME;
    pHints = &hints;
    }

  do
    {
    if (addrFound)
      {
      rc = 0;
      }
    else
      {
      rc = getaddrinfo(pNode,NULL,pHints,ppAddrInfoOut);
      }
    if (rc == 0)
      {
      addrFound = TRUE;
      *ppAddrInfoOut = insert_addr_name_info(*ppAddrInfoOut,pNode);
      if (*ppAddrInfoOut != NULL)
        {
        return 0;
        }
      rc = EAI_AGAIN;
      }
    if (rc != EAI_AGAIN)
      {
      return rc;
      }
    }while(retryCount-- >= 0);
  return EAI_FAIL;
  }    


int connect_to_trqauthd(
    
  int *sock)

  {
  int   rc = PBSE_NONE;
  int   local_socket;
  char  unix_sockname[MAXPATHLEN + 1];
  char *err_msg = NULL;

  snprintf(unix_sockname, sizeof(unix_sockname), "%s/%s", TRQAUTHD_SOCK_DIR, TRQAUTHD_SOCK_NAME);
  
  if ((local_socket = socket_get_unix()) < 0)
    {
    cerr << "could not open unix domain socket\n";
    rc = PBSE_SOCKET_FAULT;
    }
  else if ((rc = socket_connect_unix(local_socket, unix_sockname, &err_msg)) != PBSE_NONE)
    {
    cerr << "socket_connect_unix failed: " << rc;
    rc = PBSE_SOCKET_FAULT;
    close(local_socket);
    }
  else
    {
    *sock = local_socket;
    }

  if (err_msg != NULL)
    free(err_msg);

  return(rc);
  }

  
