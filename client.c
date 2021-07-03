#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include<string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <termios.h>

#define port 7780

#define ADD_ACC 1
#define DELETE 2
#define UPDATE 4
#define fetch_account 3
#define fetch_account_Joint 8
#define UPDATE_Joint 9
#define Check_Balance 3
#define Change_Password 4
#define View_details 5
#define MODIFY 3
#define WRITE_TRANS 5






struct trans_info{
	long long int account_no;
	double balance;
	double transamount;
	char flag;
};

union semun{
	int val;
	struct semid_ds *buf;
	unsigned short int *array;
}arg;

struct accountdata{
	char username1[20];
	char password1[20];
	char username2[20];
	char password2[20];
	long long int account_no;
	double balance;
	char type;
};


/*struct data{
  int action;
  int result;
  };*/
struct message{
	int action;
	struct accountdata account;
	struct trans_info trans;
	int result;

};

long long int generateAccountNo(){
	srand(time(0));
 	long long int random=rand()%10000000;
	return random;
}
struct accountdata openAccount(){



	struct accountdata account;
		printf("****************** Enter User Details ****************");
	
		printf("Enter User Type\na - Admin\nn - Normal User\nj - JointUser\n");
		scanf(" %[^\n]",&account.type);

		if(account.type=='n'){

			printf("\nEnter User Name : ");
			scanf(" %20[^\n]",account.username1);
			printf("Enter Password : ");
			scanf(" %10[^\n]",account.password1);
			account.account_no=generateAccountNo();
			account.balance=0;
		}
		else if(account.type=='j'){
			printf("\nEnter User Name for joint user1 : ");
			scanf(" %20[^\n]",account.username1);
			printf("Enter Password for joint user1: ");
			scanf(" %20[^\n]",account.password1);
			printf("\nEnter User Name for joint user2 : ");
			scanf(" %20[^\n]",account.username2);
			printf("Enter Password for joint user2: ");
			scanf(" %20[^\n]",account.password2);
			account.account_no=generateAccountNo();
			account.balance=0;
		}
		else{
			printf("\nEnter User Name : ");
			scanf(" %20[^\n]",account.username1);
			printf("Enter Password : ");
			scanf(" %20[^\n]",account.password1);
			account.account_no=generateAccountNo();
			account.balance=0;
		}
		
		//account.type='n';
		//account.user=user;	

		return account;

	}

void AccountDetails(struct accountdata account){
	printf("**************** Bank Account Details ******************\n");

	if(account.type=='n'){
		printf("Account_No\t\t :: %lld\nUserName\t\t :: %s\nBalance\t\t\t :: %f\nUser_type\t\t :: Normal User\n",account.account_no,account.username1,account.balance);
	}
	else if(account.type=='j'){
		printf("Account_No\t\t :: %lld\nUserName 1\t\t :: %s\nUserName 2\t\t :: %s\nBalance\t\t\t :: %f\nUser_type\t\t :: Jointusers\n",account.account_no,account.username1,account.username2,account.balance);
	}
	else{
		printf("Account_No\t\t :: %lld\nUserName\t\t :: %s\nBalance\t\t\t :: %f\nUser_type\t\t :: Admin\n",account.account_no,account.username1,account.balance);	
	}
	/*if(account.account_type=='j')
		{printf("Name\t\t :: %s\nName\t\t :: %s\n",account.user.name_1,account.user.name_2);}
	else
		{printf("Name\t\t :: %s\n",account.username);}*/
}



struct message execute(struct message request_packet){
	struct message response_packet={0};
	struct sockaddr_in serv,cli;
	
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	serv.sin_family=AF_INET;
	serv.sin_addr.s_addr=INADDR_ANY;
	serv.sin_port=htons(port);
	int result=connect(sfd,(void *)(&serv),sizeof(serv));
	if(result<0){printf("connection status :: %d\t\n",result);}
	ssize_t s=write(sfd,&request_packet,sizeof(struct message));
	ssize_t r=read(sfd,&response_packet,sizeof(struct message));
	close(sfd);
	
	return response_packet;
}


int main(){
	struct message request,response={0};
	int z=0;
	
	 struct termios oflags, nflags;

	char c;
	printf("\n\n");
	printf("\n\t\t***********WELCOME TO ONLINE-BANKING***********\n\n\n");
   
	printf("ENTER USERNAME: \t");
	scanf(" %20[^\n]",&request.account.username1);

	tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
        return EXIT_FAILURE;
    }

    printf("ENTER PASSWORD: \t");
    scanf(" %20[^\n]",&request.account.password1);

    /* restore terminal */
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
        perror("tcsetattr");
        return EXIT_FAILURE;
    }


	//char user[]=request.account.username1;
	//char pass[]=request.account.password1;
	//memcpy(request.account.username1,user,strlen(user));
	//memcpy(request.account.password1,pass,strlen(pass));
	strcpy(request.account.username2,request.account.username1);
	strcpy(request.account.password2,request.account.password1);
	
	//printf("%lld,%s",request.account.account_no,request.account.password);
	request.action=fetch_account;
	request.result=-1;
	response=execute(request);
	printf("Response Status :: %d\n",response.result);
	//printf("Response Status :: %s\n",response.account.password1);
	
	if((strcmp(request.account.password1,response.account.password1) == 0)||(strcmp(request.account.password2,response.account.password2) == 0)){
		//login success
		
		//AccountDetails(response.account);
		if(response.account.type=='n'){
			if(strcmp(response.account.username1,request.account.username1)==0){
			printf("LOGIN SUCCESS .......\n");
		//normal user
			printf("Weclome User :: %s\n",response.account.username1);
			int choice,ch;
			do{
			printf("Enter Choice\n");
			printf("1.Deposit\n");
			printf("2.Withdraw\n");
			printf("3.Check Balance\n");
			printf("4.Change Username or Password\n");
			printf("5.View details\n");
			printf("6.Logout\n");
			scanf("%d",&choice);
			switch(choice){
			case 1:{
				//int fd1=open("transactionDB.txt",O_APPEND|O_WRONLY|O_CREAT,0666);
				struct trans_info trans;
				double amount;
				printf("Enter Deposit Amount\n");
				scanf("%lf",&amount);
				if(amount>0){
					struct message a_request,a_response={0};
					//a_request.account.username=response.account.username;
					a_request.account.account_no=response.account.account_no;
					a_request.action=fetch_account_Joint;
					a_request.result=-1;
					a_response=execute(a_request);
					if(a_response.result>0){
						a_response.account.balance=a_response.account.balance+amount;
						a_response.action=UPDATE_Joint;
						a_response.result=-1;
						response=execute(a_response);
						//AccountDetails(response.account);
						if(response.result>0){
						
								a_request.action=WRITE_TRANS;
								a_request.trans.account_no=response.account.account_no;
								a_request.trans.balance=response.account.balance;
								a_request.trans.transamount=amount;
								a_request.trans.flag='d';
								//char a[10];

								a_response=execute(a_request);
									
							
								//lseek(fd,sizeof(account),SEEK_CUR);
							
							//close(fd1);
							printf("Transaction Success\n");
						}
						else{printf("Sorry !! Transaction Fail\n");}
					}else{printf("Sorry !! Unable To Fetch Account Details To Deposit.\n");}
					}else{printf("Sorry !! Invalid Amount\n");}
				break;}
			case 2:{
				double amount;
				printf("Enter Withdraw Amount\n");
				scanf("%lf",&amount);
				struct message a_request,a_response={0};
				
				a_request.account.account_no=response.account.account_no;
				a_request.action=fetch_account_Joint;
				a_request.result=-1;
				a_response=execute(a_request);
				if(a_response.result>0){
					response=a_response;
					if(response.account.balance>=amount){
					response.account.balance=response.account.balance-amount;
					response.action=UPDATE_Joint;
					response.result=-1;
					response=execute(response);
					if(response.result>0){
						
								a_request.action=WRITE_TRANS;
								a_request.trans.account_no=response.account.account_no;
								a_request.trans.balance=response.account.balance;
								a_request.trans.transamount=amount;
								a_request.trans.flag='w';
								//char a[10];
								
								a_response=execute(a_request);
								
								//lseek(fd,sizeof(account),SEEK_CUR);
							
							//close(fd1);
							printf("Transaction Success\n");
						}
					else{printf("Sorry !! Transaction Fail\n");}
					}
					else{printf("Sorry !! Not Enough Balance\n");}
					}else{printf("Sorry !! Unable To Fatch Account Details To Withdraw.\n");}
				break;}
			case 3:{
				response.action=fetch_account;
				response.result=-1;
				response=execute(response);
				if(response.result>0){printf("Transaction Success\n");
				printf("Available Balance Is INR %lf\n",response.account.balance);
				}
				else{printf("Sorry !! Transaction Fail\n");}
				break;}
			case 4:{
				struct message a_request,a_response={0};
				printf("Change Username(press 1) or Password(other than 1) ?\n");
				int dec;
				scanf("%d",&dec);
				if(dec==1){
					response.action=fetch_account_Joint;
					response.result=-1;
					response=execute(response);
					printf("Enter a new username1 : ");
					scanf(" %20[^\n]",response.account.username1);
					response.result=-1;
					response.action=UPDATE_Joint;

					response=execute(response);
					if(response.result>0){printf("Transaction Success !! Username Successfully Changed\n");}
					else{printf("Sorry !! Transaction Fail. Something Going Wrong\n");}
				}
				else{
					char password[10];
					printf("Enter New Password\n");
					scanf(" %10[^\n]",response.account.password1);
					printf("Confirm Password\n");
					scanf(" %10[^\n]",password);
					if(strcmp(password,response.account.password1) == 0){
					response.action=UPDATE;
					response.result=-1;
					response=execute(response);
					
				
				if(response.result>0){printf("Transaction Success !! Password Successfully Changed\n");}
				else{printf("Sorry !! Transaction Fail. Something Going Wrong\n");}
				}else{printf("Sorry !! Password Not Match\n");}

				}
				break;}
			case 5:{
				response.action=fetch_account;
				response.result=-1;
				response=execute(response);
				AccountDetails(response.account);

				struct trans_info tr;
				printf("\n******Last Transactions*******\n");
				int f=open("transactionDB.txt",O_CREAT|O_APPEND|O_RDONLY,0666);
				while(read(f,&tr,sizeof(tr))){
					if(tr.account_no==response.account.account_no && tr.flag=='d'){
						printf("\nDebited INR : %lf",tr.transamount);

					}
					else if(tr.account_no==response.account.account_no && tr.flag=='w'){
						printf("\nWithdrawn INR : %lf",tr.transamount);
					}
				}
				break;}
			case 6:{
				exit(0);
				break;}
			default:{printf("Invalid Choice :: %d\n",choice);}
			}
			printf("\nDo You Want To Continue (Press 1) else (Press 0) to Logout...\n");
			scanf("%d",&ch);
			}while(ch==1);
		}
		else{
			printf("invalid username...");
		}
		
		}

		else if(response.account.type=='j'){


			int key=ftok(".",'b');
			int semid=semget(key,1,0);


		    if(semid==-1){
		        semid = semget(key, 1, IPC_CREAT|0644);    
		        arg.val = 1;
		        semctl(semid, 0, SETVAL, arg);
		    }
					    


			int flag=0;
			if(strcmp(request.account.username1,response.account.username1)==0){
				if(strcmp(request.account.password1,response.account.password1)==0){
					flag=1;
				}
				else{
					
					printf("Invalid Password....\n");
					exit(0);
				}
			}
			else if(strcmp(request.account.username1,response.account.username2)==0){
				if(strcmp(request.account.password1,response.account.password2)==0){
					flag=1;
				}
				else{
					
					printf("Invalid Password....\n");
					exit(0);
				}
			}
			else{}
		
			//printf("Weclome User :: %s\n",user);
			if(flag==1){
					printf("LOGIN SUCCESS .......\n");
					printf("Welcome to %s and %s Joint Account\n",response.account.username1,response.account.username2);
					char choice;
					int ch;
					do{
					printf("Enter Choice\n");
					printf("A.Deposit / Withdraw\n");
					printf("B.Check Balance\n");
					printf("C.Change Username or Password\n");
					printf("D.View details\n");
					printf("E.Logout\n");
					scanf(" %5[^\n]",&choice);
    				


					switch(choice){
					case 'A':{

						struct sembuf buf={0,-1,0|SEM_UNDO};
					    
					    printf("Before entering critical section.......\n");
					    printf("Waiting for unlock........\n");
					    semop(semid,&buf,1);
						int ch;
						label:
						printf("For deposit press 1\nFor Withdrawal press 2\n");
						scanf("%d",&ch);


						if(ch==1){
							double amount;
							printf("Enter Deposit Amount\n");
							scanf("%lf",&amount);
							if(amount>0){
								struct message a_request,a_response={0};
								//a_request.account.username=response.account.username;
								a_request.account.account_no=response.account.account_no;
								
								a_request.action=fetch_account_Joint;
								a_request.result=-1;
								a_response=execute(a_request);
								if(a_response.result>0){
									a_response.account.balance=a_response.account.balance+amount;
									a_response.action=UPDATE;
									a_response.result=-1;
									response=execute(a_response);
									AccountDetails(response.account);
									if(response.result>0){
										a_request.action=WRITE_TRANS;
										a_request.trans.account_no=response.account.account_no;
										a_request.trans.balance=response.account.balance;
										a_request.trans.transamount=amount;
										a_request.trans.flag='d';
										//char a[10];
										
										a_response=execute(a_request);
										
										//lseek(fd,sizeof(account),SEEK_CUR);
									
									//close(fd1);
										printf("Transaction Success\n");
									}
									else{printf("Sorry !! Transaction Fail\n");}
								}else{printf("Sorry !! Unable To Fetch Account Details To Deposit.\n");}
								}else{printf("Sorry !! Invalid Amount\n");}
							}
						else if(ch==2){
								double amount;
								printf("Enter Withdraw Amount\n");
								scanf("%lf",&amount);
								struct message a_request,a_response={0};
								//a_request.account.account_no=response.account.account_no;
								a_request.account.account_no=response.account.account_no;
								a_request.action=fetch_account_Joint;
								a_request.result=-1;
								a_response=execute(a_request);
								if(a_response.result>0){
									response=a_response;
									if(response.account.balance>=amount){
									response.account.balance=response.account.balance-amount;
									response.action=UPDATE;
									response.result=-1;
									response=execute(response);
									if(response.result>0){
										a_request.action=WRITE_TRANS;
										a_request.trans.account_no=response.account.account_no;
										a_request.trans.balance=response.account.balance;
										a_request.trans.transamount=amount;
										a_request.trans.flag='w';
										//char a[10];
										
										a_response=execute(a_request);
										
										//lseek(fd,sizeof(account),SEEK_CUR);
									
									//close(fd1);
										printf("Transaction Success\n");
									}
									else{printf("Sorry !! Transaction Fail\n");}
									}
									else{printf("Sorry !! Not Enough Balance\n");}
									}else{printf("Sorry !! Unable To Fatch Account Details To Withdraw.\n");}
								}
								else{
									printf("enter correct choice 1 or 2...");
									goto label;
								}

								buf.sem_op=1;
    							semop(semid,&buf,1);
						break;
						}

					case 'B':{
						response.action=fetch_account_Joint;
						response.result=-1;
						response=execute(response);
						if(response.result>0){printf("Transaction Success\n");
						printf("Available Balance Is INR %lf\n",response.account.balance);
						}
						else{printf("Sorry !! Transaction Fail\n");}
						break;}
					case 'C':{
							struct message a_request,a_response={0};
								printf("Change Username(press 1) or Password(other than 1) ?\n");
								int dec;
								scanf("%d",&dec);
								if(dec==1){
									if(strcmp(response.account.password1,request.account.password1) == 0){
									printf("Enter a new username1 : ");
									scanf(" %20[^\n]",response.account.username1);
									response.result=-1;
									response.action=UPDATE_Joint;

									response=execute(response);
									if(response.result>0){printf("Transaction Success !! Username Successfully Changed\n");}
									else{printf("Sorry !! Transaction Fail. Something Going Wrong\n");}
									}
									else if(strcmp(response.account.password2,request.account.password1) == 0){
										printf("Enter a new username2 : ");
										scanf(" %20[^\n]",response.account.username2);
										response.result=-1;
										response.action=UPDATE_Joint;

										response=execute(response);
										if(response.result>0){printf("Transaction Success !! Username Successfully Changed\n");}
										else{printf("Sorry !! Transaction Fail. Something Going Wrong\n");}
										}
									else{}
								}
									
							

							else{
							char password[20];
							if(strcmp(response.account.password1,request.account.password1) == 0){
								printf("Enter New Password\n");
								scanf(" %20[^\n]",response.account.password1);
								printf("Confirm Password\n");
								scanf(" %20[^\n]",&password);
								if(strcmp(password,response.account.password1) == 0){
								response.action=UPDATE_Joint;
								response.result=-1;
								response=execute(response);
								if(response.result>0){printf("Transaction Success !! Password Successfully Changed for user1\n");}
								else{printf("Sorry !! Transaction Fail. Something Going Wrong\n");}
								}
							}
							else if(strcmp(response.account.password2,request.account.password1) == 0){
								
								printf("Enter New Password\n");
								scanf(" %20[^\n]",response.account.password2);
								printf("Confirm Password\n");
								scanf(" %20[^\n]",&password);
								if(strcmp(password,response.account.password2) == 0){
								response.action=UPDATE_Joint;
								response.result=-1;
								response=execute(response);
								if(response.result>0){printf("Transaction Success !! Password Successfully Changed for user2\n");}
								else{printf("Sorry !! Transaction Fail. Something Going Wrong\n");}
								}
							}
						else{printf("Sorry !! Password Not Match\n");}
					}
						

						break;
					}
					case 'D':{
						response.action=fetch_account_Joint;
						response.result=-1;
						response=execute(response);
						AccountDetails(response.account);

						struct trans_info tr;
						printf("\n******Last Transactions*******\n");
						int f=open("transactionDB.txt",O_CREAT|O_APPEND|O_RDONLY,0666);
						while(read(f,&tr,sizeof(tr))){
							if(tr.account_no==response.account.account_no && tr.flag=='d'){
								printf("\nDebited INR : %lf",tr.transamount);

							}
							else if(tr.account_no==response.account.account_no && tr.flag=='w'){
								printf("\nWithdrawn INR : %lf",tr.transamount);
							}
						}

						break;}

					case 'E':{
						exit(0);
						break;}
					default:{printf("Invalid Choice :: %c\n",choice);}
					}
						printf("\nDo You Want To Continue (Press 1) or (Press 0) to Logout...\n");
						scanf("%d",&ch);
					}while(ch==1);
				
				}
			}



		else if(response.account.type=='a'){

			int key=ftok(".",'a');
			int semid=semget(key,1,0);


		    if(semid==-1){
		        semid = semget(key, 1, IPC_CREAT|0644);    
		        arg.val = 1;
		        semctl(semid, 0, SETVAL, arg);
		    }


			struct sembuf buf={0,-1,0|SEM_UNDO};
					    
		    printf("Before entering critical section.......\n");
		    printf("Waiting for unlock........\n");
		    semop(semid,&buf,1);
//AdminLogout
			printf("LOGIN SUCCESS .......\n");
			int choice,ch;
			printf("Weclome Admin ");

			label5:
			do{
			printf("Enter Choice\n");
			printf("1.Open New Account\n");
			printf("2.Delete Account\n");
			printf("3.Modify an account\n");
			printf("5.Search An account\n");
			printf("6.Logout\n");
			scanf("%d",&choice);
			switch(choice){
			case 1:{
				struct accountdata account=openAccount();
				struct message a_request,a_response={0};
				a_request.action=ADD_ACC;
				a_request.result=-1;
				a_request.account=account;
				a_response=execute(a_request);
				if(response.result>0){printf("Transaction Success !! New Account Successfully Created\nDetails Are..\n");
				AccountDetails(a_response.account);
				}
				else{printf("Sorry !! Transaction Fail. Account Creating Fail\n");}
				
				break;
			}

			case 2:{
				struct message a_request,a_response,b_request,b_response={0};
				printf("Enter account number To Delete : ");
				scanf("%lld",&a_request.account.account_no);
				a_request.action=DELETE;
				a_request.result=-1;
				a_response=execute(a_request);
				//printf("%d",a_response.result);
				
				b_request.account.account_no=a_request.account.account_no;
				b_request.action=fetch_account_Joint;
				b_request.result=-1;
				b_response=execute(a_request);
				//AccountDetails(b_response.account);
				if(b_response.result>0 && (strcmp(b_response.account.username1,"")!=0)){
					printf("Sorry !! Unable To Delete Account \n");
				}

				else{printf("\nRecord Deleted Successfully \n");
				}
				
				break;
				}

			case MODIFY:{
				int choice,m_ch;
				struct message a_request,a_response={0};
				do{
					printf("What needs to be modified ??\n");
					printf("1.Update Name\n");
					printf("2.Update password\n");
					printf("3.Update user type\n");
					scanf("%d",&choice);
					switch(choice){
						case 1:{
							int ch;
							printf("Enter a accountnumber whose username to be modified : \n");
							scanf("%lld",&a_request.account.account_no);
							a_request.action=fetch_account_Joint;
							a_request.result=-1;
							a_response=execute(a_request);
							if(a_response.result>0 && a_response.account.type=='j'){

								label1:printf("press 1 to modify username1 or press 2 to modify username2  ? \n");
								scanf("%d",&ch);
								if(ch==1){
									printf("Enter a new username1 : ");
									scanf(" %20[^\n]",a_response.account.username1);
									a_response.result=-1;
									a_response.action=UPDATE_Joint;
									a_response=execute(a_response);
								}
								else if(ch==2){
									printf("Enter a new username2 : ");
									scanf(" %20[^\n]",a_response.account.username2);
									a_response.result=-1;
									a_response.action=UPDATE_Joint;
									a_response=execute(a_response);
								}
								else{
									printf("Enter correct choice 1 or 2....or 0 to Logout\n");
									int c;
									scanf("%d",&c);
									if(c==0) exit(0);
									else goto label1;
								}
							}
							else if(a_response.result>0 && a_response.account.type=='n'){

								printf("Enter a new username : ");
								scanf(" %20[^\n]",a_response.account.username1);
								a_response.result=-1;
								a_response.action=UPDATE;
								a_response=execute(a_response);
							}

							
							if(a_response.result>0){
								printf("Record successfully modified.....\n");
							}
							else{
								printf("Record not Modified.......\n");
							}
							break;
						}
						
						case 2:{
							int ch;
							printf("Enter a accountnumber whose password to be modified : \n");
							scanf("%lld",&a_request.account.account_no);
							a_request.action=fetch_account_Joint;
							a_request.result=-1;
							a_response=execute(a_request);
							if(a_response.result>0 && a_response.account.type=='j'){

								label2:printf("press 1 to modify password1 or press 2 to modify password2  ? \n");
								scanf("%d",&ch);
								if(ch==1){
									printf("Enter a new password1 : ");
									scanf(" %20[^\n]",a_response.account.password1);
									a_response.result=-1;
									a_response.action=UPDATE_Joint;
									a_response=execute(a_response);
								}
								else if(ch==2){
									printf("Enter a new password2 : ");
									scanf(" %20[^\n]",a_response.account.password2);
									a_response.result=-1;
									a_response.action=UPDATE_Joint;
									a_response=execute(a_response);
								}
								else{
									printf("Enter correct choice 1 or 2....or 0 to Logout\n");
									int c;
									scanf("%d",&c);
									if(c==0) exit(0);
									else goto label2;
								}
							}
							else if(a_response.result>0 && a_response.account.type=='n'){

								printf("Enter a new password : ");
								scanf(" %20[^\n]",a_response.account.password1);
								a_response.result=-1;
								a_response.action=UPDATE;
								a_response=execute(a_response);
							}

							
							if(a_response.result>0){
								printf("Record successfully modified.....\n");
							}
							else{
								printf("Record not Modified.......\n");
							}
							break;
						}
						
						case 3:{
							int ch;
							printf("Enter a accountnumber whose usertype to be modified : \n");
							scanf("%lld",&a_request.account.account_no);
							a_request.action=fetch_account_Joint;
							a_request.result=-1;
							a_response=execute(a_request);
							if(a_response.result>0 && a_response.account.type=='j'){

								
								printf("Enter User Type\na - Admin\nn - Normal User.....to be changed to which user type \n");
								scanf(" %[^\n]",&a_response.account.type);

								label3:
								printf("Which user to be removed ? press 1 for user1 press 2 for user2..\n");
								scanf("%d",&ch);

								if(ch==1){
									strcpy(a_response.account.username1,a_response.account.username2);
									strcpy(a_response.account.password1,a_response.account.password2);
									strcpy(a_response.account.username2,"");
									strcpy(a_response.account.password2,"");
									a_response.action=UPDATE_Joint;
									a_response=execute(a_response);
								}
								else if(ch==2){
									
									strcpy(a_response.account.username2,"");
									strcpy(a_response.account.password2,"");
									a_response.result=-1;
									a_response.action=UPDATE_Joint;
									a_response=execute(a_response);
								}
								else{
									printf("Enter correct choice 1 or 2....or 0 to Logout\n");
									int c;
									scanf("%d",&c);
									if(c==0) exit(0);
									else goto label3;
								}
							}
							else if(a_response.result>0 && a_response.account.type=='n'){

								label4:
								printf("Enter User Type\na - Admin\nj - Joint User.....to be changed to which user type \n");
								scanf(" %[^\n]",&a_response.account.type);
								if(a_response.account.type=='a'){
									a_response.result=-1;
									a_response.action=UPDATE;
									a_response=execute(a_response);
								}
								else if(a_response.account.type=='j'){
									printf("Enter user 2 details :\n");
									printf("\nEnter User Name for joint user2 : ");
									scanf(" %20[^\n]",a_response.account.username2);
									printf("Enter Password for joint user2: ");
									scanf(" %20[^\n]",a_response.account.password2);

									a_response.result=-1;
									a_response.action=UPDATE_Joint;
									a_response=execute(a_response);
								}
								else{
									printf("Enter correct choice 1 or 2....or 0 to Logout\n");
									int c;
									scanf("%d",&c);
									if(c==0) exit(0);
									else goto label4;
								}


								}

							
							if(a_response.result>0){
								printf("Record successfully modified.....\n");
							}
							else{
								printf("Record not Modified.......\n");
							}
							break;
						}

						default:{
							printf("Enter correct choice.....\n");
							break;
						}
						}
						printf("If you want to continue to modify press 1 or Press 0 to exit from this module...\n");
						scanf("%d",&m_ch);
					}while(m_ch==1);

					break;
				}


			case 5:{
				long long int accountno;	
				struct message a_request,a_response={0};
				printf("Enter Account Number To Search\n");
				scanf("%lld",&accountno);
				a_request.account.account_no=accountno;
				a_request.action=fetch_account_Joint;
				a_request.result=-1;
				a_response=execute(a_request);
				if(a_response.result>0&&a_response.account.account_no==accountno){
				printf("Transaction Success !! Account Found\n");
				AccountDetails(a_response.account);
				struct trans_info tr;
				printf("\n******Last Transactions*******\n");
				int f=open("transactionDB.txt",O_CREAT|O_APPEND|O_RDONLY,0666);
				while(read(f,&tr,sizeof(tr))){
					if(tr.account_no==a_response.account.account_no && tr.flag=='d'){
						printf("\nDebited INR : %lf",tr.transamount);

					}
					else if(tr.account_no==a_response.account.account_no && tr.flag=='w'){
						printf("\nWithdrawn INR : %lf",tr.transamount);
					}
				}
				}
				else{printf("Sorry !! Unable To Fetch Account ,doesnot exists\n");}
				break;}

			case 6:{
				buf.sem_op=1;
    			semop(semid,&buf,1);
				exit(0);
				break;}
			default:
			{
				int ch;

				printf("Enter appropriate choice...or press 0 to exit\n");
				scanf("%[^\n]",&ch);
				if(ch==0) exit(0);
				else goto label5;
				break;
			}

		}

			printf("\nDo You Want To Continue press 1 else press 0 to Logout..\n");
			scanf("%d",&ch);
			}while(ch==1);

			buf.sem_op=1;
    		semop(semid,&buf,1);
		}
		else{
			printf("Invalid usertype :: %c\n",request.account.type);
		}
	}
	else{printf("Invalid Username / Password....");}
return 0;
}




