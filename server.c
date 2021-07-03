#include <sys/types.h>        
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include<string.h>
#include<stdlib.h>

#define port 7780


#define ADD_ACC 1
#define DELETE 2
#define fetch_account 3
#define fetch_account_Joint 8
#define UPDATE_Joint 9
#define WRITE_TRANS 5
#define UPDATE 4





struct accountdata{
	char username1[20];
	char password1[20];
	char username2[20];
	char password2[20];
	long long int account_no;
	double balance;
	char type;
};

struct trans_info{
	long long int account_no;
	double balance;
	double transamount;
	char flag;
};


struct message{
	int action;
	struct accountdata account;
	struct trans_info trans;
	int result;
//	char *result_msg;
};

int getRecordNo(char username[20]){	
	int fd1=open("accountdb.txt",O_RDWR,0666);
	struct accountdata account;
	int record_no=-1,i=0,n;
	//lseek(fd,0,SEEK_SET);
	
	while(read(fd1,&account,sizeof(account))){
		
		if(strcmp(username,account.username1) == 0 || strcmp(username,account.username2) == 0){
			
			//printf("Account no. :%d\n",account.account_no);
			//printf("Account no. :%lf\n",account.balance);
			record_no=i;
			break;
		}
		i++;
		//lseek(fd,sizeof(account),SEEK_CUR);
	}
	close(fd1);
	printf("\nrecord no. : %d",record_no);
	return record_no;
}

int getRecordNoJoint(long long int accountno){	
	int fd1=open("accountdb.txt",O_RDONLY,0666);
	struct accountdata account;
	int record_no=-1,i=0,n;
	//lseek(fd,0,SEEK_SET);
	
	while(read(fd1,&account,sizeof(account))){
		//printf("use 1 : %s\n",account.username2);
		//printf("use 2 : %s\n",username);
		//printf("balance : %c",account.type);
		//printf("byte read : %d",n);
		if(account.account_no==accountno){
			
			//printf("Account no. :%d\n",account.account_no);
			//printf("Account no. :%lf\n",account.balance);
			record_no=i;
			break;
		}
		i++;
		//lseek(fd,sizeof(account),SEEK_CUR);
	}
	close(fd1);
	printf("\nrecord no. : %d",record_no);
	return record_no;
}




struct accountdata getRecord(int record_no){
	int fd;
	struct accountdata account;
	fd=open("accountdb.txt",O_RDONLY);
	struct flock lock;
	size_t s=0;
	off_t off=lseek(fd,(record_no*sizeof(account)),SEEK_SET);
	lock.l_type=F_RDLCK;
	lock.l_whence=SEEK_CUR;
	lock.l_start=0;
	lock.l_len=sizeof(account);
	lock.l_pid=getpid();
	//printf("record no.: %d",record_no);
	printf("File Descripter value :: %d, Process id %ld\n",fd,(long)getpid());
	printf("Before Entering To Critical Section\n");
	int i=fcntl(fd,F_SETLKW,&lock);
	if(i!=-1){
		printf("Locking Status :: %d\n",i);
		printf("Entering Critical Section\n");
		s=read(fd,&account,sizeof(struct accountdata));
		//printf("Account no. :%d\n",account.account_no);
		//printf("Account no. :%lf\n",account.balance);
		lock.l_type=F_UNLCK;
		int u=fcntl(fd,F_SETLK,&lock);
		printf("Exiting CS... Unlocking Status :: %d\n",u);
		/*CS End*/
		}else{
		printf("Lock Not Accquired");
		}
		close(fd);
		return account;
}


int deleteRecord(long long int accountno){
	int fd1=open("accountdb.txt",O_RDWR|O_APPEND,0666);
	int fd2=open("accountdbt.txt",O_CREAT|O_RDWR|O_APPEND,0666);
	struct flock lock,lock1;
	int s=0;

	struct accountdata account;
	
	lock.l_type=F_WRLCK;
	lock.l_whence=SEEK_CUR;
	lock.l_start=0;
	lock.l_len=sizeof(account);
	lock.l_pid=getpid();

	lock1.l_type=F_WRLCK;
	lock1.l_whence=SEEK_CUR;
	lock1.l_start=0;
	lock1.l_len=sizeof(account);
	lock1.l_pid=getpid();


	
	printf("Before Entering To Critical Section\n");
	int i=fcntl(fd1,F_SETLKW,&lock);
	int j=fcntl(fd2,F_SETLKW,&lock1);
	if(i!=-1 && j!=-1){
		printf("Locking Status :: %d\n",i);
		printf("Entering Critical Section\n");
		
		while(read(fd1,&account,sizeof(account))){
			//printf("\nhello\n");
			if(account.account_no==accountno){
				printf("yess\n");
				continue;
			}
			else{
				write(fd2,&account,sizeof(account));
			}
		}
}
	
			
		//}
		remove("accountdb.txt");
		rename("accountdbt.txt","accountdb.txt");
		lock.l_type=F_UNLCK;
		lock1.l_type=F_UNLCK;
		int u=fcntl(fd1,F_SETLK,&lock);
		int u1=fcntl(fd2,F_SETLK,&lock1);
		//printf("Exiting CS... Unlocking Status :: %d,%d\n",u,u1);
		s=1;
		/*CS End
		}else{
		printf("Lock Not Accquired");
		}*/
		close(fd1);
		close(fd2);
		return s;

}

int updateRecord(struct accountdata account,int record_no){
	int fd;
	fd=open("accountdb.txt",O_WRONLY,0666);
	struct flock lock;
	size_t s=0;
	off_t off=lseek(fd,(record_no*sizeof(account)),SEEK_SET);

	lock.l_type=F_WRLCK;
	lock.l_whence=SEEK_CUR;
	lock.l_start=0;
	lock.l_len=sizeof(account);
	lock.l_pid=getpid();
	printf("File Descripter value :: %d, Process id %ld\n",fd,(long)getpid());
	printf("Before Entering To Critical Section\n");
	int i=fcntl(fd,F_SETLKW,&lock);
	if(i!=-1){
		printf("Locking Status :: %d\n",i);
		printf("Entering Critical Section\n");
		
		s=write(fd,&account,sizeof(struct accountdata));
		lock.l_type=F_UNLCK;
		int u=fcntl(fd,F_SETLK,&lock);
		printf("Exiting CS... Unlocking Status :: %d\n",u);
		/*CS End*/
		}else{
		printf("Lock Not Accquired");
		}
		close(fd);
		return s;
}




int main(){
	struct sockaddr_in serv,cli;
	int sd,sz,nsd;
	char buff[80];
	pid_t pid;

	sd=socket(AF_INET,SOCK_STREAM,0);
	
	serv.sin_family=AF_INET;
	serv.sin_addr.s_addr=INADDR_ANY;
	serv.sin_port=htons(port);




	int bind_result=bind(sd,(void *)(&serv),sizeof(serv));
	int listen_result=listen(sd,5);
	sz=sizeof(cli);
	printf("Server Initializing...\nSocket returns :: %d\nBind result:: %d\nListening status :: %d\n",sd,bind_result,listen_result);
	for(;;){
	nsd=accept(sd,(void *)(&cli),&sz);
	if ( (pid = fork()) == 0 ) {
		close(sd);
		ssize_t s=0;
		struct message request={0},response={0};
		struct trans_info trans; 
		read(nsd,&request,sizeof(request));
		printf("Message Recieve From client :: Action %d\n",request.result);
		switch(request.action){
		case ADD_ACC:{
			response.result=1;
			response.action=request.action;
			struct accountdata acc;
			acc=request.account;
			int fd;
			fd=open("accountdb.txt",O_WRONLY|O_APPEND);
			//printf("opening db status %d \n",fd);
			write(fd,&acc,sizeof(acc));
			close(fd);
			response.account=acc;
			printf("\n***********Write Successful***********\n");

			//response.account=openAccount(request.account);

		//	response.result_msg="ADD_ACC_OK";
			break;}
		case UPDATE:{
			response.action=request.action;
			int record_no=getRecordNo(request.account.username1);
			response.result=updateRecord(request.account,record_no);
			response.account=request.account;
			printf("\n***********Update Successful***********\n");
		//	response.result_msg="DELETE_OK";
			break;}

		case UPDATE_Joint:{
			response.action=request.action;
			int record_no=getRecordNoJoint(request.account.account_no);
			//printf("record number :  %d\n",record_no);
			response.result=updateRecord(request.account,record_no);
			response.account=request.account;
			printf("\n***********Update Successful***********\n");
		//	response.result_msg="DELETE_OK";
			break;}

		case DELETE:{
			response.action=request.action;
			//printf("hello%lld\n",request.account.account_no);
			response.result=deleteRecord(request.account.account_no);
			strcpy(response.account.username1,"");
			printf("\n***********Delete Successful***********\n");
			break;
			}

		case fetch_account:{
			response.result=1;
			response.action=request.action;
			//printf("%s\n",request.account.username1);
			int record_no=getRecordNo(request.account.username1);
			response.account=getRecord(record_no);
		//	response.result_msg="INVALID_ACTION";
			break;}

		case fetch_account_Joint:{
			response.result=1;
			response.action=request.action;
			//printf("%s\n",request.account.username1);
			int record_no=getRecordNoJoint(request.account.account_no);
			response.account=getRecord(record_no);
		//	response.result_msg="INVALID_ACTION";
			break;}

		case WRITE_TRANS:{
			//response.result=1;
			//response.action=request.action;
			int f=open("transactionDB.txt",O_CREAT|O_WRONLY|O_APPEND,0666);
			write(f,&request.trans,sizeof(trans));
			response.result=1;
			response.trans=request.trans;

			break;
		}
		default:{
			response.result=0;
			response.action=request.action;
			response.account=request.account;
		
		}
		}
		
			s=write(nsd,&response,sizeof(response));
		
		
		close(nsd);
		printf("Message  sent to client :: %d\n Write Response %d",response.result,(int)s);
		exit(0);
		}
		close(nsd);
	}
	}


