#include "license_pbs.h"
#include <pbs_config.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h> /* hostent */

#include "mom_func.h" /* sig_tbl */
#include "pbs_job.h" /* job, pjobexec_t, task, pjobexec_t */
#include "u_tree.h" /* AvlTree */
#include "libpbs.h" /* job_file */
#include "mom_hierarchy.h" /* mom_hierarchy_t */
#include "mom_server.h" 
#include "server_limits.h" /* pbs_net_t. Also defined in net_connect.h */
#include "threadpool.h" /* threadpool_t */
#include "list_link.h" /* list_link, tlist_head */
#include "resizable_array.h" /* resizable_array */
#include "pbs_nodes.h" /* pbsnode */
#include "attribute.h" /* pbs_attribute */
#include "resource.h" /* resource_def */
#include "log.h" /* LOG_BUF_SIZE */
#include "tcp.h"
#include "prolog.h" /* PBS_PROLOG_TIME */

#ifdef PENABLE_LINUX26_CPUSETS
#include "pbs_cpuset.h"
#include "node_internals.hpp"
#endif


extern mom_hierarchy_t *mh;

mom_server     mom_servers[PBS_MAXSERVER];
resizable_array *received_statuses;
int mom_server_count = 0;
const char *msg_daemonname = "unset";
struct sig_tbl sig_tbl[2];
char pbs_current_user[PBS_MAXUSER];
extern char *server_alias;
const char *dis_emsg[10];
long *log_event_mask = NULL;
int rpp_dbprt = 0;
unsigned int pe_alarm_time = PBS_PROLOG_TIME;
struct connection svr_conn[PBS_NET_MAX_CONNECTIONS];
struct config standard_config[2];
struct config dependent_config[2];
long MaxConnectTimeout = 5000000;
tlist_head svr_requests;
const char *msg_info_mom = "Torque Mom Version = %s, loglevel = %d";
hash_table_t *received_table;
threadpool_t *request_pool;
AvlTree okclients;
time_t wait_time = 10;
pthread_mutex_t log_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
dynamic_string  *mom_status;
char log_buffer[LOG_BUF_SIZE];
char **ArgV;
char  *OriginalPath;
bool   parsing_hierarchy = false;
extern bool received_cluster_addrs;

void save_args(int argc, char **argv) {}

int log_remove_old(char *DirPath, unsigned long ExpireTime)
  {
  fprintf(stderr, "The call to long needs to be mocked!!\n");
  exit(1);
  }

int mom_close_poll(void)
  {
  fprintf(stderr, "The call to mom_close_poll needs to be mocked!!\n");
  exit(1);
  }

void init_abort_jobs(int recover)
  {
  fprintf(stderr, "The call to init_abort_jobs needs to be mocked!!\n");
  exit(1);
  }

int mom_server_add(const char *value)
  {
  fprintf(stderr, "The call to mom_server_add needs to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save needs to be mocked!!\n");
  exit(1);
  }

void mom_job_purge(job *pjob)
  {
  fprintf(stderr, "The call to job_purge needs to be mocked!!\n");
  exit(1);
  }

void scan_non_child_tasks(void)
  {
  fprintf(stderr, "The call to scan_non_child_tasks needs to be mocked!!\n");
  exit(1);
  }

extern "C"
{
unsigned int get_svrport(char *service_name, char *ptype, unsigned int pdefault)
  {
  fprintf(stderr, "The call to get_svrport needs to be mocked!!\n");
  exit(1);
  }
}

int get_la(double *rv)
  {
  fprintf(stderr, "The call to get_la needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

int get_parent_and_child(char *start, char **parent, char **child, char **end)
  {
  fprintf(stderr, "The call to get_parent_and_child needs to be mocked!!\n");
  exit(1);
  }

int MUStrNCat(char **BPtr, int *BSpace, const char *Src)
  {
  fprintf(stderr, "The call to MUStrNCat needs to be mocked!!\n");
  exit(1);
  }

void scan_for_exiting(void)
  {
  fprintf(stderr, "The call to scan_for_exiting needs to be mocked!!\n");
  exit(1);
  }

void mom_checkpoint_set_directory_path(const char *str)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_directory_path needs to be mocked!!\n");
  exit(1);
  }

void exec_bail(job *pjob, int code, std::set<int> *sisters_contacted)
  {
  fprintf(stderr, "The call to exec_bail needs to be mocked!!\n");
  exit(1);
  }

int AVL_list(AvlTree tree, char **Buf, long *current_len, long *max_len)
  {
  fprintf(stderr, "The call to AVL_list needs to be mocked!!\n");
  exit(1);
  }

unsigned long mom_checkpoint_set_checkpoint_interval(const char *value)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_checkpoint_interval needs to be mocked!!\n");
  exit(1);
  }

int TMomCheckJobChild(pjobexec_t *TJE, int Timeout, int *Count, int *RC)
  {
  fprintf(stderr, "The call to TMomCheckJobChild needs to be mocked!!\n");
  exit(1);
  }

int message_job(job *pjob, enum job_file jft, char *text)
  {
  fprintf(stderr, "The call to message_job needs to be mocked!!\n");
  exit(1);
  }

int IamRoot()
  {
  fprintf(stderr, "The call to IamRoot needs to be mocked!!\n");
  exit(1);
  }

int add_network_entry(mom_hierarchy_t *nt, char *name, struct addrinfo *ai, unsigned short rm_port, int path, int level)
  {
  fprintf(stderr, "The call to add_network_entry needs to be mocked!!\n");
  exit(1);
  }

int log_init(const char *suffix, const char *hostname)
  {
  fprintf(stderr, "The call to log_init needs to be mocked!!\n");
  exit(1);
  }

int post_epilogue(job *pjob, int ev)
  {
  fprintf(stderr, "The call to post_epilogue needs to be mocked!!\n");
  exit(1);
  }

char *get_job_envvar(job *pjob, const char *variable)
  {
  fprintf(stderr, "The call to get_job_envvar needs to be mocked!!\n");
  exit(1);
  }

char *netaddr_pbs_net_t(pbs_net_t ipadd)
  {
  fprintf(stderr, "The call to netaddr_pbs_net_t needs to be mocked!!\n");
  exit(1);
  }

unsigned long mom_checkpoint_set_checkpoint_script(const char *value)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_checkpoint_script needs to be mocked!!\n");
  exit(1);
  }

int mom_get_sample(void)
  {
  fprintf(stderr, "The call to mom_get_sample needs to be mocked!!\n");
  exit(1);
  }

void set_rpp_throttle_sleep_time(long sleep_time)
  {
  fprintf(stderr, "The call to set_rpp_throttle_sleep_time needs to be mocked!!\n");
  exit(1);
  }

int run_pelog(int which, char *specpelog, job *pjog, int pe_io_type, int deletejob)
  {
  fprintf(stderr, "The call to run_pelog needs to be mocked!!\n");
  exit(1);
  }

char *pbs_get_server_list(void)
  {
  fprintf(stderr, "The call to pbs_get_server_list needs to be mocked!!\n");
  exit(1);
  }

int tm_request(struct tcp_chan *chan, int version)
  {
  fprintf(stderr, "The call to tm_request needs to be mocked!!\n");
  exit(1);
  }

int csv_length(const char *csv_str)
  {
  fprintf(stderr, "The call to csv_length needs to be mocked!!\n");
  exit(1);
  }

char *csv_nth(const char *csv_str, int n)
  {
  fprintf(stderr, "The call to csv_nth needs to be mocked!!\n");
  exit(1);
  }

void dep_cleanup(void)
  {
  fprintf(stderr, "The call to dep_cleanup needs to be mocked!!\n");
  exit(1);
  }

size_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_setup(int fd)
  {
  fprintf(stderr, "The call to DIS_tcp_setup needs to be mocked!!\n");
  exit(1);
  }

int MUSNPrintF(char **BPtr, int *BSpace, const char *Format, ...)
  {
  fprintf(stderr, "The call to MUSNPrintF needs to be mocked!!\n");
  exit(1);
  }

int mom_over_limit(job *pjob)
  {
  fprintf(stderr, "The call to mom_over_limit needs to be mocked!!\n");
  exit(1);
  }

int AVL_is_in_tree_no_port_compare(u_long key, uint16_t port, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_is_in_tree_no_port_compare needs to be mocked!!\n");
  exit(1);
  }

void net_close(int but)
  {
  fprintf(stderr, "The call to net_close needs to be mocked!!\n");
  exit(1);
  }

int kill_task(task *ptask, int sig, int pg)
  {
  fprintf(stderr, "The call to kill_task needs to be mocked!!\n");
  exit(1);
  }

void mom_server_all_send_state(void)
  {
  fprintf(stderr, "The call to mom_server_all_send_state needs to be mocked!!\n");
  exit(1);
  }

void mom_server_all_init(void)
  {
  fprintf(stderr, "The call to mom_server_all_init needs to be mocked!!\n");
  exit(1);
  }

int DIS_tcp_wflush(tcp_chan *chan)
  {
  fprintf(stderr, "The call to DIS_tcp_wflush needs to be mocked!!\n");
  exit(1);
  }

int diswcs(tcp_chan *chan, const char *value, size_t nchars)
  {
  fprintf(stderr, "The call to diswcs needs to be mocked!!\n");
  exit(1);
  }

void mom_checkpoint_check_periodic_timer(job *pjob)
  {
  fprintf(stderr, "The call to mom_checkpoint_check_periodic_timer needs to be mocked!!\n");
  exit(1);
  }

int chk_file_sec(const char *path, int isdir, int sticky, int disallow, int fullpath, char *SEMsg)
  {
  fprintf(stderr, "The call to chk_file_sec needs to be mocked!!\n");
  exit(1);
  }

int TMomFinalizeJob3(pjobexec_t *TJE, int ReadSize, int ReadErrno, int *SC)
  {
  fprintf(stderr, "The call to TMomFinalizeJob3 needs to be mocked!!\n");
  exit(1);
  }

unsigned long mom_checkpoint_set_restart_script(const char *value)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_restart_script needs to be mocked!!\n");
  exit(1);
  }

void mom_server_all_update_stat(void)
  {
  fprintf(stderr, "The call to mom_server_all_update_stat needs to be mocked!!\n");
  exit(1);
  }

void check_busy(double mla)
  {
  fprintf(stderr, "The call to check_busy needs to be mocked!!\n");
  exit(1);
  }

void mom_is_request(struct tcp_chan *chan, int version, int *cmdp)
  {
  fprintf(stderr, "The call to mom_is_request needs to be mocked!!\n");
  exit(1);
  }

int mom_checkpoint_init(void)
  {
  fprintf(stderr, "The call to mom_checkpoint_init needs to be mocked!!\n");
  exit(1);
  }

int wait_request(time_t waittime, long *SState)
  {
  fprintf(stderr, "The call to wait_request needs to be mocked!!\n");
  exit(1);
  }

void log_close(int msg)
  {
  fprintf(stderr, "The call to log_close needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

resizable_array *initialize_resizable_array(int size)
  {
  resizable_array *ra = (resizable_array*)calloc(1, sizeof(resizable_array));
  size_t           amount = sizeof(slot) * size;

  ra->max       = size;
  ra->num       = 0;
  ra->next_slot = 1;
  ra->last      = 0;

  ra->slots = (slot *)calloc(1, amount);

  return(ra);
  }

int log_open(char *filename, char *directory)
  {
  fprintf(stderr, "The call to log_open needs to be mocked!!\n");
  exit(1);
  }

int send_sisters(job *pjob, int com, int using_radix, std::set<int> *sisters_to_contact)
  {
  fprintf(stderr, "The call to send_sisters needs to be mocked!!\n");
  exit(1);
  }

int mom_set_use(job *pjob)
  {
  fprintf(stderr, "The call to mom_set_use needs to be mocked!!\n");
  exit(1);
  }

int initialize_threadpool(threadpool_t **pool, int min_threads, int max_threads, int max_idle_time)
  {
  fprintf(stderr, "The call to initialize_threadpool needs to be mocked!!\n");
  exit(1);
  }

void term_job(job *pjob)
  {
  fprintf(stderr, "The call to term_job needs to be mocked!!\n");
  exit(1);
  }

struct passwd * getpwnam_ext(char * user_name)
  {
  fprintf(stderr, "The call to getpwnam_ext needs to be mocked!!\n");
  exit(1);
  }

char *disrst(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrst needs to be mocked!!\n");
  exit(1);
  }

int mom_open_poll(void)
  {
  fprintf(stderr, "The call to mom_open_poll needs to be mocked!!\n");
  exit(1);
  }

void dep_main_loop_cycle(void)
  {
  fprintf(stderr, "The call to dep_main_loop_cycle needs to be mocked!!\n");
  exit(1);
  }

int init_network(unsigned int socket, void *(*readfunc)(void *))
  {
  fprintf(stderr, "The call to init_network needs to be mocked!!\n");
  exit(1);
  }

void dep_initialize(void)
  {
  fprintf(stderr, "The call to dep_initialize needs to be mocked!!\n");
  exit(1);
  }

void append_link(tlist_head *head, list_link *new_link, void *pobj)
  {
  fprintf(stderr, "The call to append_link needs to be mocked!!\n");
  exit(1);
  }

void log_roll(int max_depth)
  {
  fprintf(stderr, "The call to log_roll needs to be mocked!!\n");
  exit(1);
  }

hash_table_t *create_hash(int size)
  {
  fprintf(stderr, "The call to create_hash needs to be mocked!!\n");
  exit(1);
  }

int init_resc_defs(void)
  {
  fprintf(stderr, "The call to init_resc_defs needs to be mocked!!\n");
  exit(1);
  }

int swap_things(resizable_array *ra, void *thing1, void *thing2)
  {
  fprintf(stderr, "The call to swap_things needs to be mocked!!\n");
  exit(1);
  }

int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg)
  {
  fprintf(stderr, "The call to get_fullhostname needs to be mocked!!\n");
  exit(1);
  }

void scan_for_terminated(void)
  {
  fprintf(stderr, "The call to scan_for_terminated needs to be mocked!!\n");
  exit(1);
  }

long log_size(void)
  {
  fprintf(stderr, "The call to log_size needs to be mocked!!\n");
  exit(1);
  }

int task_save(task *ptask)
  {
  fprintf(stderr, "The call to task_save needs to be mocked!!\n");
  exit(1);
  }

void *mom_process_request(void *sock_num)
  {
  fprintf(stderr, "The call to mom_process_request needs to be mocked!!\n");
  exit(1);
  }

int setup_env(const char *filen)
  {
  fprintf(stderr, "The call to setup_env needs to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn needs to be mocked!!\n");
  exit(1);
  }

void catch_child(int sig)
  {
  fprintf(stderr, "The call to catch_child needs to be mocked!!\n");
  exit(1);
  }

AvlTree AVL_insert(u_long key, uint16_t port, struct pbsnode *node, AvlTree tree)
  {
  fprintf(stderr, "The call to AVL_insert needs to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry needs to be mocked!!\n");
  exit(1);
  }

job *mom_find_job(char *jobid)
  {
  fprintf(stderr, "The call to find_job needs to be mocked!!\n");
  exit(1);
  }

void DIS_tcp_settimeout(long timeout)
  {
  fprintf(stderr, "The call to DIS_tcp_settimeout needs to be mocked!!\n");
  exit(1);
  }

mom_hierarchy_t *initialize_mom_hierarchy(void)
  {
  mom_hierarchy_t *nt = (mom_hierarchy_t *)calloc(1, sizeof(mom_hierarchy_t));
  nt->paths = initialize_resizable_array(INITIAL_SIZE_NETWORK);
  return(nt);
  }

int diswsi(tcp_chan *chan, int value)
  {
  fprintf(stderr, "The call to diswsi needs to be mocked!!\n");
  exit(1);
  }

void mom_server_all_diag(std::stringstream &output)
  {
  fprintf(stderr, "The call to mom_server_all_diag needs to be mocked!!\n");
  exit(1);
  }

int disrsi(tcp_chan *chan, int *retval)
  {
  fprintf(stderr, "The call to disrsi needs to be mocked!!\n");
  exit(1);
  }

unsigned long mom_checkpoint_set_checkpoint_run_exe_name(const char *value)
  {
  fprintf(stderr, "The call to mom_checkpoint_set_checkpoint_run_exe_name needs to be mocked!!\n");
  exit(1);
  }

void im_request(struct tcp_chan *chan, int version)
  {
  fprintf(stderr, "The call to im_request needs to be mocked!!\n");
  exit(1);
  }

int lock_init()
  {
  fprintf(stderr, "The call to lock_init needs to be mocked!!\n");
  exit(1);
  }

char *netaddr(struct sockaddr_in *sai)
  {
  fprintf(stderr, "The call to netaddr needs to be mocked!!\n");
  exit(1);
  }

int remove_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to remove_thing needs to be mocked!!\n");
  exit(1);
  }

int tcp_connect_sockaddr(struct sockaddr *sa, size_t sa_size)
  {
  fprintf(stderr, "The call to tcp_connect_sockaddr needs to be mocked!!\n");
  exit(1);
  }

int insert_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to insert_thing needs to be mocked!!\n");
  exit(1);
  }

void clear_servers()
  {
  fprintf(stderr, "The call to clear_servers needs to be mocked!!\n");
  exit(1);
  }

int diswul(tcp_chan *chan, unsigned long value)
  {
  fprintf(stderr, "The call to diswul needs to be mocked!!\n");
  exit(1);
  }

void *next_thing(resizable_array *ra, int *iter)
  {
  if (parsing_hierarchy)
    received_cluster_addrs = true;
  return(NULL);
  }

int im_compose(tcp_chan *chan, char *jobid, char *cookie, int command, tm_event_t event, tm_task_id taskid)
  {
  fprintf(stderr, "The call to im_compose needs to be mocked!!\n");
  exit(1);
  }

dynamic_string *get_dynamic_string(int size, const char *str)
  {
  fprintf(stderr, "The call to get_dynamic_string needs to be mocked!!\n");
  exit(1);
  }

int socket_avail_bytes_on_descriptor(int socket)
  {
  return(0);
  }

int tcp_chan_has_data(struct tcp_chan *chan)
  {
  return(1);
  }

unsigned int get_random_number()
  {
  return(0);
  }

void shutdown_to_server(int ServerIndex) {}

void DIS_tcp_cleanup(struct tcp_chan *chan) {}

void initialize_network_info() {}

void *pop_thing(resizable_array *ra)
  {
  return(NULL);
  }

int insert_thing_after(resizable_array *ra, void *thing, int i)
  {
  return(0);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

ssize_t read_ac_socket(int fd, void *buf, ssize_t count)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}
void log_ext(int type, const char *func_name, const char *msg, int o) {}

void free_attrlist(list_link *l) {}

void attrl_fixlink(list_link *l) {}

int send_join_job_to_a_sister(job *pjob, int stream, eventent *ep, tlist_head phead, int node_id)
  {
  return(0);
  }

void im_request(struct tcp_chan *chan, int version,struct sockaddr_in *pSockAddr) {}

attribute_def    job_attr_def[1];

int pbs_getaddrinfo(

  const char       *pNode,
  struct addrinfo  *pHints,
  struct addrinfo **ppAddrInfoOut)

  {
  return(0);
  }

void parse_mom_hierarchy(int fds)
  {
  mh->paths->num++;
  }

int put_env_var(const char *name, const char *value)
  {
  return(0);
  }

void sort_paths() {}

void free_mom_hierarchy(mom_hierarchy_t *nh) {}

bool am_i_mother_superior(const job &pjob)
  {
  return(false);
  }

#ifdef PENABLE_LINUX26_CPUSETS

int hwloc_topology_init(hwloc_topology_t *)
  {
  return 0;
  }

int hwloc_topology_set_flags(hwloc_topology_t,unsigned long)
  {
  return 0;
  }

int hwloc_topology_load(hwloc_topology_t )
  {
  return 0;
  }

int hwloc_get_type_depth (hwloc_topology_t topology, hwloc_obj_type_t type)
  {
  return 0;
  }

unsigned hwloc_get_nbobjs_by_depth (hwloc_topology_t topology, unsigned depth)
  {
  return 0;
  }

void cleanup_torque_cpuset(void){}


int init_torque_cpuset(void)
  {
  return 0;
  }

void create_cpuset_reservation_if_needed(
      job &pjob)
  {
  return;
  }

void recover_cpuset_reservation(

      job &pjob)

  {
  return;
  }

node_internals::node_internals(void){}

numa_node::numa_node(numa_node const&){}

allocation::allocation(allocation const&){}

#endif

char *threadsafe_tokenizer(

  char **str,    /* M */
  const char  *delims) /* I */

  {
  char *current_char;
  char *start;

  if ((str == NULL) ||
      (*str == NULL))
    return(NULL);

  /* save start position */
  start = *str;

  /* return NULL at the end of the string */
  if (*start == '\0')
    return(NULL);

  /* begin at the start */
  current_char = start;

  /* advance to the end of the string or until you find a delimiter */
  while ((*current_char != '\0') &&
         (!strchr(delims, *current_char)))
    current_char++;

  /* advance str */
  if (*current_char != '\0')
    {
    /* not at the end of the string */
    *str = current_char + 1;
    *current_char = '\0';
    }
  else
    {
    /* at the end of the string */
    *str = current_char;
    }

  return(start);
  } /* END threadsafe_tokenizer() */
