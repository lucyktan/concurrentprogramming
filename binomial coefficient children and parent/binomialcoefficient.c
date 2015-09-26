#define _XOPEN_SOURCE			//to retrieve user id

#include <stdio.h>			//standard input/out
#include <stdlib.h>			//standard library
#include <errno.h>			//error numbers
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>			//wait()
#include <sys/resource.h>		
#include <time.h>			//system time

#define BINOM_MAX_ITERS 10  		//upper limit of n in calculating the binomial coefficient
#define SLEEP_LENGTH 2			//default sleep time for child processes

/*Error-detection functions*/

int fork_wrapper(int syscall_val) {	//identifies an error with fork() if process id is negative
	if (syscall_val < 0) {		
		perror("fork");
		exit(errno);
	} 
	else
		return syscall_val;
}

char* cuserid_wrapper() {		//identifies an error with retrieving the user id if pid is null
	char* val = cuserid(NULL);
	if (val == NULL) {
		perror("cuserid");
		exit(errno);
	} 
	else
		return val;
}

time_t time_wrapper() {			//identifies an error with retrieving the system time is time is null
	time_t val = time(NULL);
	if (val == ((time_t) -1)) {
		perror("time_wrapper");
		exit(errno);
	}
	else 
		return val;
}

void getrusage_wrapper(struct rusage* output) {		//identifies an error with retrieving CPU usage time
	int val = getrusage(RUSAGE_SELF, output);
	if (val == -1) {
		perror("getrusage_wrapper");
		exit(errno);
	}
}

char* getcwd_wrapper() {				//identifies an error with retrieving the current working directory
	char* cwd = getcwd(NULL, 0);
	if (cwd == NULL) {
		perror("getcwd_wrapper");
		exit(errno);
	}
	else
		return cwd;
}

int waitpid_wrapper(int syscall_val) {			//identifies an error with waitpid() if -1 is returned
	if (syscall_val == -1) {		
		perror("waitpid");
		exit(errno);
	} 
	else
		return syscall_val;
}

char* get_proc_name(const char* base, char* buff) {	//idenfities an error with getting the process name and id
	int val = sprintf(buff, "%s, PID=%d", base, (int)getpid());
	if (val < 0) {
		perror("get_proc_name");
		exit(errno);
	}
	else 
		sprintf(buff, "%s, PID=%d", base, (int)getpid());
	return (char*)buff;
}

/*prints username, userid, effective userid, group id, and process id, along with appropriate error messages*/
void print_uids_gids(const char* proc_name) {		
	printf("[%s] username: %s\n", proc_name, cuserid_wrapper());	
	printf("[%s] user id: %d\n", proc_name, getuid());
	printf("[%s] effective user id: %d\n", proc_name, geteuid());
	printf("[%s] group id: %d\n", proc_name, getgid());
	printf("[%s] pid: %d\n", proc_name, getpid());
}

void print_exec_times(const char* proc_name) {		//prints current time, user CPU time, and system CPU time
	int i;					
	int n = 0;
    	for (i = 0; i < 100000000; i++) 		//for processes to accumulate nonzero time values
        	n += 2;	

	time_t cur_time;
	struct rusage res_usage;

	cur_time = time_wrapper();
	getrusage_wrapper(&res_usage);

	printf("[%s] seconds since epoch: %d\n", proc_name, (int)cur_time);
	printf("[%s] current time: %s\n", proc_name, ctime(&cur_time));
	printf("[%s] user CPU time: %d us\n", proc_name, (int)res_usage.ru_utime.tv_usec);
	printf("[%s] system CPU time: %d us\n", proc_name, (int)res_usage.ru_stime.tv_usec);
}

int factorial(int n) {					//calculates the factorial of n
  	if (n == 0)
   		return 1;
  	else
    		return(n * factorial(n-1));
}

/*child functions*/
void child_proc_1(const char* proc_name) {				//child 1 terminates immediately after printing
	print_uids_gids(proc_name);
	printf("[%s] parent PID: %d\n", proc_name, getppid());

	printf("(n (n-2)) binomial coefficient computations of integers n=2, 3, ..., 10, start now!\n");
	print_exec_times(proc_name);
	exit(0);
}

void child_proc_2_3(int binom_start, const char* proc_name) {		//child 2 and 3 alternate printing the binomial coefficient
	print_uids_gids(proc_name);
	printf("[%s] parent PID: %d\n", proc_name, getppid());

	sleep(SLEEP_LENGTH);
	int binom_coeff;
	int i;
	for (i = binom_start; i <= BINOM_MAX_ITERS; i += 2) {		//child 2 starts by printing the binomial coefficient when n is even
		if (i % 2 == 0) {
			binom_coeff = factorial(i)/(factorial(i-2) * factorial(2));
			printf("[%s] binom_n=%d: %d\n", proc_name, i, binom_coeff);
		}
		else {							//child 3 starts by printing the binomial coefficient when n is odd
			binom_coeff = factorial(i)/(factorial(i-2) * factorial(2));
			printf("[%s] binom_n=%d: %d\n", proc_name, i, binom_coeff);
		}
		sleep(SLEEP_LENGTH);
	}
	print_exec_times(proc_name);
    	exit(0);
}

int child_proc_4(const char* proc_name) {				//child 4 prints out a list of files in the working directory 
	print_uids_gids(proc_name);
	printf("[%s] parent PID: %d\n", proc_name, getppid());

	sleep(SLEEP_LENGTH);
	fflush(stdout);
	execlp("ls", "ls", "-l", (char *) NULL);			//execlp() shouldn't return anything if it is working properly
	print_exec_times(proc_name);
	perror("Error calling execlp()");				//print error if execlp() returns something
	return EXIT_FAILURE;
	exit(0);
}	

int main(void) {	

	//prints the username, userid, effective userid, group id, and process id, along with appropriate error messages
	char parent_proc_name[255];	
	get_proc_name("parent", parent_proc_name);				
	printf("[%s] current working directory: %s\n", parent_proc_name, getcwd_wrapper());	//prints the curent working directory
	print_uids_gids(parent_proc_name);					

	pid_t child1_pid = fork_wrapper(fork());				//forks child 1
	if (child1_pid == 0) {
		char proc_name[255];
		get_proc_name("child1", proc_name);
		child_proc_1(proc_name);					//runs child 1 and exits
	}
	printf("[%s] child1_pid: %d\n", parent_proc_name, child1_pid);

	pid_t child2_pid = fork_wrapper(fork());				//forks child 2
	if (child2_pid == 0) {
		sleep(SLEEP_LENGTH / 2);					//sleep half the length because it is alternating with child 3
		char proc_name[255];
		get_proc_name("child2", proc_name);
		child_proc_2_3(2, proc_name);					//calculate the binomial coefficient starting with n=2 and n+=2
	}
	printf("[%s] child2_pid: %d\n", parent_proc_name, child2_pid);

	pid_t child3_pid = fork_wrapper(fork());				//forks child 3
	if (child3_pid == 0) {
		sleep(SLEEP_LENGTH);
		char proc_name[255];
		get_proc_name("child3", proc_name);
		child_proc_2_3(3, proc_name);					//calculate the binomial coefficient starting with n=3 and n+=2
	}
	printf("[%s] child3_pid: %d\n", parent_proc_name, child3_pid);

	pid_t child4_pid = fork_wrapper(fork()); 				//forks child 4
	if (child4_pid == 0) {
		sleep(SLEEP_LENGTH * 7);					//sleeps for a long time to wait for child 2 and 3 to terminate
		char proc_name[255];
		get_proc_name("child4", proc_name);
		child_proc_4(proc_name);					//runs child 4 and exits
	}
	printf("[%s] child4_pid: %d\n", parent_proc_name, child4_pid);


	//prints when each child has terminated
	int status;

	waitpid_wrapper(waitpid(child1_pid, &status, 0));
	printf("[%s] child1 terminated\n", parent_proc_name);

	waitpid_wrapper(waitpid(child2_pid, &status, 0));
	printf("[%s] child2 terminated\n", parent_proc_name);

	waitpid_wrapper(waitpid(child3_pid, &status, 0));
	printf("[%s] child3 terminated\n", parent_proc_name);

	waitpid_wrapper(waitpid(child4_pid, &status, 0));
	printf("[%s] child4 terminated\n", parent_proc_name);

	print_exec_times(parent_proc_name);					//parent waits for all children to terminate
	return 0;

}


