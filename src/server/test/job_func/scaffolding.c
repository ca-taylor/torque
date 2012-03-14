#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include <pthread.h>

#include "pbs_ifl.h" /* MAXPATHLEN, PBS_MAXSERVERNAME */
#include "server.h" /* server, NO_BUFFER_SPACE */
#include "pbs_job.h" /* all_jobs, job_array, job */
#include "resizable_array.h" /* resizable_array */
#include "attribute.h" /* pbs_attribute, attribute_def */
#include "net_connect.h" /* pbs_net_t */
#include "list_link.h" /* list_link */
#include "hash_table.h" /* hash_table_t */
#include "batch_request.h" /* batch_request */
#include "work_task.h" /* all_tasks */
#include "array.h" /* ArrayEventsEnum */
#include "dynamic_string.h" /* dynamic_string */

/* This section is for manipulting function return values */
#include "test_job_func.h" /* *_SUITE */
int func_num = 0; /* Suite number being run */
int tc = 0; /* Used for test routining */
int iter_num = 0;

int valbuf_size = 0;
/* end manip */

char *path_jobs;
char path_checkpoint[MAXPATHLEN + 1];
char *job_log_file = NULL;
char server_name[PBS_MAXSERVERNAME + 1];
char *msg_abt_err = "Unable to abort Job %s which was in substate %d";
int queue_rank = 0;
char *path_spool;
char *msg_err_purgejob = "Unlink of job file failed";
struct server server;
struct all_jobs array_summary;
char *path_jobinfo_log;
int LOGLEVEL = 0;
pthread_mutex_t *job_log_mutex;

int array_save(job_array *pa)
  {
  fprintf(stderr, "The call to array_save needs to be mocked!!\n");
  exit(1);
  }

int insert_thing(resizable_array *ra, void *thing)
  {
  fprintf(stderr, "The call to insert_thing needs to be mocked!!\n");
  exit(1);
  }

void account_record(int acctype, job *pjob, char *text)
  {
  fprintf(stderr, "The call to account_record needs to be mocked!!\n");
  exit(1);
  }

char *arst_string(char *str, pbs_attribute *pattr)
  {
  fprintf(stderr, "The call to arst_string needs to be mocked!!\n");
  exit(1);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save needs to be mocked!!\n");
  exit(1);
  }

void svr_mailowner(job *pjob, int mailpoint, int force, char *text)
  {
  fprintf(stderr, "The call to svr_mailowner needs to be mocked!!\n");
  exit(1);
  }

int remove_thing_from_index(resizable_array *ra, int index)
  {
  fprintf(stderr, "The call to remove_thing_from_index needs to be mocked!!\n");
  exit(1);
  }

ssize_t read_nonblocking_socket(int fd, void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to read_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr needs to be mocked!!\n");
  exit(1);
  }

pbs_net_t get_hostaddr(int *local_errno, char *hostname)
  {
  fprintf(stderr, "The call to get_hostaddr needs to be mocked!!\n");
  exit(1);
  }

int log_job_record(char *buf)
  {
  int rc = 0;
  if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 2))
    rc = -1;
  if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 8))
    {
    if (iter_num == 1)
      rc = -1;
    iter_num++;
    }
  return rc;
  }

int job_log_open(char *filename, char *directory)
  {
  int rc = 0;
  if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 1))
    rc = -1;
  return rc;
  }

void delete_link(struct list_link *old)
  {
  fprintf(stderr, "The call to delete_link needs to be mocked!!\n");
  exit(1);
  }

int add_hash(hash_table_t *ht, int value, char *key)
  {
  fprintf(stderr, "The call to add_hash needs to be mocked!!\n");
  exit(1);
  }

void free_br(struct batch_request *preq)
  {
  fprintf(stderr, "The call to free_br needs to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to work_task needs to be mocked!!\n");
  exit(1);
  }

void svr_dequejob(job *pjob, int val)
  {
  fprintf(stderr, "The call to svr_dequejob needs to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket needs to be mocked!!\n");
  exit(1);
  }

void initialize_all_tasks_array(all_tasks *at)
  {
  fprintf(stderr, "The call to initialize_all_tasks_array needs to be mocked!!\n");
  exit(1);
  }

job_array *get_array(char *id)
  {
  fprintf(stderr, "The call to get_array needs to be mocked!!\n");
  exit(1);
  }

job *get_recycled_job()
  {
  fprintf(stderr, "The call to get_recycled_job needs to be mocked!!\n");
  exit(1);
  }

int get_value_hash(hash_table_t *ht, char *key)
  {
  fprintf(stderr, "The call to get_value_hash needs to be mocked!!\n");
  exit(1);
  }

void delete_task(struct work_task *ptask)
  {
  fprintf(stderr, "The call to delete_task needs to be mocked!!\n");
  exit(1);
  }

int depend_on_term(job *pjob)
  {
  fprintf(stderr, "The call to depend_on_term needs to be mocked!!\n");
  exit(1);
  }

int client_to_svr(pbs_net_t hostaddr, unsigned int port, int local_port, char *EMsg)
  {
  fprintf(stderr, "The call to client_to_svr needs to be mocked!!\n");
  exit(1);
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next needs to be mocked!!\n");
  exit(1);
  }

int issue_signal(job *pjob, char *signame, void (*func)(struct work_task *), void *extra)
  {
  fprintf(stderr, "The call to issue_signal needs to be mocked!!\n");
  exit(1);
  }

resizable_array *initialize_resizable_array(int size)
  {
  fprintf(stderr, "The call to initialize_resizable_array needs to be mocked!!\n");
  exit(1);
  }

int svr_enquejob(job *pjob, int has_sv_qs_mutex, int prev_index)
  {
  fprintf(stderr, "The call to svr_enquejob needs to be mocked!!\n");
  exit(1);
  }

void update_array_values(job_array *pa, void *j, int old_state, enum ArrayEventsEnum event)
  {
  fprintf(stderr, "The call to update_array_values needs to be mocked!!\n");
  exit(1);
  }

int array_delete(job_array *pa)
  {
  fprintf(stderr, "The call to array_delete needs to be mocked!!\n");
  exit(1);
  }

void *next_thing(resizable_array *ra, int *iter)
  {
  fprintf(stderr, "The call to next_thing needs to be mocked!!\n");
  exit(1);
  }

void release_req(struct work_task *pwt)
  {
  fprintf(stderr, "The call to release_req needs to be mocked!!\n");
  exit(1);
  }

hash_table_t *create_hash(int size)
  {
  fprintf(stderr, "The call to create_hash needs to be mocked!!\n");
  exit(1);
  }

work_task *next_task(all_tasks *at, int *iter)
  {
  fprintf(stderr, "The call to next_task needs to be mocked!!\n");
  exit(1);
  }

int swap_things(resizable_array *ra, void *thing1, void *thing2)
  {
  fprintf(stderr, "The call to swap_things needs to be mocked!!\n");
  exit(1);
  }

int insert_thing_after(resizable_array *ra, void *thing, int index)
  {
  fprintf(stderr, "The call to insert_thing_after needs to be mocked!!\n");
  exit(1);
  }

void issue_track(job *pjob)
  {
  fprintf(stderr, "The call to issue_track needs to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate needs to be mocked!!\n");
  exit(1);
  }

struct batch_request *setup_cpyfiles(struct batch_request *preq, job *pjob, char *from, char *to, int direction, int tflag)
  {
  fprintf(stderr, "The call to setup_cpyfiles needs to be mocked!!\n");
  exit(1);
  }

int insert_into_recycler(job *pjob)
  {
  fprintf(stderr, "The call to insert_into_recycler needs to be mocked!!\n");
  exit(1);
  }

int attr_to_str(struct dynamic_string *ds, attribute_def *attr_def,struct pbs_attribute attr,int XML)
  {
  int rc = 0;
  if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 4))
    {
    if (iter_num == 0)
      {
      iter_num++;
      rc = NO_BUFFER_SPACE;
      }
    else if (iter_num == 1)
      {
      iter_num++;
      }
    }
  else if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 5))
    {
    rc = NO_BUFFER_SPACE;
    }
  else if ((func_num == RECORD_JOBINFO_SUITE) && (tc == 6))
    {
    iter_num++;
    }
  return rc;
  }

void check_job_log(struct work_task *ptask)
  {
  fprintf(stderr, "The call to check_job_log needs to be mocked!!\n");
  exit(1);
  }

void svr_evaljobstate(job *pjob, int *newstate, int *newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate needs to be mocked!!\n");
  exit(1);
  }

int remove_hash(hash_table_t *ht, char *key)
  {
  fprintf(stderr, "The call to remove_hash needs to be mocked!!\n");
  exit(1);
  }

void *next_thing_from_back(resizable_array *ra, int *iter)
  {
  fprintf(stderr, "The call to next_thing_from_back needs to be mocked!!\n");
  exit(1);
  }

void change_value_hash(hash_table_t *ht, char *key, int new_value)
  {
  fprintf(stderr, "The call to change_value_hash needs to be mocked!!\n");
  exit(1);
  }

int lock_queue(struct pbs_queue *the_queue, const char *method_name, char *msg, int logging)
  {
  fprintf(stderr, "The call to lock_queue needs to be mocked!!\n");
  exit(1);
  }

int append_dynamic_string(dynamic_string *ds, const char *to_append)
  {
  fprintf(stderr, "The call to append_dynamic_string needs to be mocked!!\n");
  exit(1);
  }

void clear_dynamic_string(dynamic_string *ds)
  {
  fprintf(stderr, "The call to attr_to_str needs to be mocked!!\n");
  exit(1);
  }

dynamic_string *get_dynamic_string(int initial_size, const char *str)
  {
  fprintf(stderr, "The call to attr_to_str needs to be mocked!!\n");
  exit(1);
  }

void free_dynamic_string(dynamic_string *ds)
  {
  fprintf(stderr, "The call to attr_to_str needs to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  return(0);
  }

int is_svr_attr_set(int index)
  {
  return(0);
  }

int get_svr_attr_str(int index, char **str)
  {
  return(0);
  }

int unlock_queue(struct pbs_queue *the_queue, const char *id, char *msg, int logging)
  {
  fprintf(stderr, "The call to unlock_queue to be mocked!!\n");
  exit(1);
  }
