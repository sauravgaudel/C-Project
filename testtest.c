// -- importing all the required modules -- //
#include<stdio.h>                           // for standard functions
#include<conio.h>                           // for using getch() function
#include<string.h>                          // for handling string
#include<math.h>                            // for taking absoulte value
#include<windows.h>                         // to use system("cls") for clearing screen
#include<time.h>                            // to create delay
#include<stdlib.h>                          // to use exit() function
#include "\Users\User\Desktop\1 sem\C project\E-pay-main\ascii_display.c"

// defining structure globally
struct userdata{
    char name[100];
    char password[100];
    double phone;
    char email[100];
    double balance;
};

// -- some essential global declarations --//
FILE *login_data_file, *user_data_file, *log_file;          //globally declaring a pointer to handle file of login data
struct userdata current_user, signup_verify;                   //to keep in track of logged in user details
#define initial_balance 8000


// ---- function prototyping --- //
void login();
void signup();
void menu_handling();
void showdetails();
void exit_program();
void send_money();
void pay_school_fee();
void viewTransactionLogs();
void encrypt(char []);
void decrypt(char []);
void delay(int);
void password_taker(char []);
void logTransaction(char *, char *, double);
// ----starting of main function ---//
void main(){
    // ----- variables initalization ------ //
    //struct userdata userlogin;
    char initial_choice;

    // ----- display section ----- //
    do{
    epay();
    printf("\t[1] Login\n\t[2] SignUp\n\t[3] Exit\n");
    printf("\tChoose any option [1/2/3]:   ");
    initial_choice = getche();
    }while(initial_choice!='1' && initial_choice!='2' && initial_choice!='3');

    switch(initial_choice){
        case '1':
        {
            // check if this is very first time of running program
            login_data_file = fopen("logindata.dat", "rb");
            if(login_data_file == NULL){     // if this is first time and no file is opened, proceed to signup
                printf("\n\n\t***Seems like you are first user***\n\t\t***Signup first***");
                delay(1.5);

                // now head to case 2;
                goto signup_first;
                }

            login();
            break;
        }

        signup_first:
            case '2':             // if user chooses to signup
            {
                signup();
                break;
            }
            case '3':
                {
                    exit_program();
                }
    }
    // This switch case gets terminated after user logins successfully
    // starting of main menu handling
    menu_handling();
    getch();
}
void logTransaction(char *username, char *action, double amount) {
    FILE *logFile = fopen("logs.txt", "ab");
    if (logFile == NULL) {
        printf("\n\tError opening log file!\n");
        return;
    }
    fprintf(logFile, "%s | %s : %.2f\n", username, action, amount);
    fclose(logFile);
}
void viewTransactionLogs() {
    system("cls");
    FILE *logFile = fopen("logs.txt", "r");
    if (logFile == NULL) {
        printf("\n\tNo transaction logs available.\n");
        return;
    }
    char fileUser[100], action[50];
    double amount;
    int found = 0;
    printf("\n\tTransaction History for %s:\n", current_user.name);
    printf("\t--------------------------------\n");
    while (fscanf(logFile, "%s | %99[^\n] | %lf", fileUser, action, &amount) != EOF) {
        if (strcmp(fileUser, current_user.name) == 0) {
            printf("\t%s\n", action);
            found = 1;
        }
    }
    if (!found) {
        printf("\tNo transactions found.\n");
    }
    fclose(logFile);
    printf("\n\tEnter any key to go back to menu: ");
    getch();
    menu_handling();
}

// main logic for taking password by replacing with *
void password_taker(char password[]){
    // this is function to take password from user and instantly replace each character of password by *

    char ch;        // to take individual character of the entered passoword
    int charposition=0;   // to take hold of current index of password's character

    while(1)
    {
        ch=getch();
        if (ch==13){      // is user has entered enter
            password[charposition]='\0';
            break;
        }
        else if (ch==8)       // if user enters backspace, it will delete current character
            {
                if (charposition >0)
                {
                    charposition--;
                    password[charposition]='\0';
                    printf("\b \b");
                }
            }
            else if (ch==32 || ch==9)    // if user enters tab or space
                continue;
            else{
                password[charposition]=ch;
                charposition++;
                printf("*");
                }
    }
}

// function to handle user login into main menu
void login(){
    struct userdata filedata[1000];               // array to hold previous user details present in file
    int is_login_successful=0;                     // to keep in track of login attempts
    login_data_file = fopen("logindata.dat", "rb");

    // just login section things
    display_login();
    // initializing n variable to count number of user logged into the system
    int n=0;
    while(fread(&filedata[n], sizeof(struct userdata), 1, login_data_file) == 1){
        decrypt(filedata[n].password);                          // decrypting password while loading
        n++;
        }

    printf("\tEnter your login details correctly: \n");
    printf("\tName:  ");
    scanf("%s", current_user.name);
    printf("\tPhone Number: ");
    scanf("%lf", &current_user.phone);
    printf("\tPassword: ");
    password_taker(current_user.password);

    for(int i=0; i<n; i++){
        if(strcmp(filedata[i].name, current_user.name) == 0 && strcmp(filedata[i].password, current_user.password)==0 && filedata[i].phone == current_user.phone){
            // if all details match;
            printf("\n\tLogin Successful !!!  ");
            is_login_successful = 1;
            delay(1);
            break;
        }
        is_login_successful = 0;                // because loop will terminate and wont break
                                                //and reaches to this line if data are incorrect
    }

    fclose(login_data_file);                    //close the file
    if(!is_login_successful){                   // if details entered is incorrect
        printf("\n\tIncorrect Details Entered!!! ");
        delay(2);
        login();
    }
}


// function to handle signup of user to the program:
void signup(){
    char ch, verify_password[200];     // ch: for star mechanism to hide password, verify for re-entered password
    int charposition = 0, signup_attempt = 0, i;
    struct userdata signupdata;

    // signup starts
    login_data_file = fopen("logindata.dat", "ab");
    if (login_data_file == NULL) {
        login_data_file = fopen("logindata.dat", "wb");
    }

    reenter_password:
    display_signup();
    // just signup section things
    printf("\tEnter Following Details \n");

    reenter_username:
    printf("\tUsername: ");
    if (signup_attempt != 1)
        scanf("%99[^\n]", signupdata.name);
    else
        printf("%s\n", signup_verify.name);

    if (signupdata.name[0] == '\0') {
        printf("\tNo spaces allowed. Please re-enter your username.\n\n");
        while (getchar() != '\n'); // Clear input buffer
        goto reenter_username;
    }
    // Check for spaces in the username (optional)
    for (int i = 0; signupdata.name[i] != '\0'; i++) {
        if (signupdata.name[i] == ' ') {
            printf("\tNo spaces allowed. Please re-enter your username.\n\n");
            while (getchar() != '\n'); // Clear input buffer
            goto reenter_username;
        }
    }

    reenter_email:
    printf("\tEmail: ");

    // Clear any leftover newline character in the input buffer
    while (getchar() != '\n');  // Clear the buffer

    if (signup_attempt != 1) {
        scanf("%99[^\n]", signupdata.email);
        if (signupdata.email[0] == '\0') {
            printf("\tNo input provided. Please re-enter your email.\n\n");
            while (getchar() != '\n'); // Clear input buffer
            goto reenter_email;
        }
    } else {
        printf("%s\n", signup_verify.email);
    }

    // Check for spaces in the email (optional)
    for (int i = 0; signupdata.email[i] != '\0'; i++) {
        if (signupdata.email[i] == ' ') {
            printf("\tInvalid Email. Please re-enter your email.\n\n");
            while (getchar() != '\n'); // Clear input buffer
            goto reenter_email;
        }
    }

    reenter_phone:
    printf("\tPhone number: ");
    if (signup_attempt != 1) scanf("%lf", &signupdata.phone);
    else printf("%.0lf\n", signup_verify.phone);
    if (!((signupdata.phone >= 9800000000 && signupdata.phone <= 9869999999) ||
        (signupdata.phone >= 9610000000 && signupdata.phone <= 9619999999) ||
        (signupdata.phone >= 9880000000 && signupdata.phone <= 9889999999) ||
        (signupdata.phone >= 9700000000 && signupdata.phone <= 9709999999) ||
        (signupdata.phone >= 9740000000 && signupdata.phone <= 9769999999)
        )) {
        printf("\tInvalid Phone Number\n\n");
        delay(1);
        goto reenter_phone;
    }

    passlength_error:
    printf("\tPassword: ");
    password_taker(signupdata.password);
    if (strlen(signupdata.password) < 4) {
        printf("\tMinimum 4 Characters\n\n");
        delay(1);
        goto passlength_error;
    }
    printf("\n\tRe-enter the password: ");
    password_taker(verify_password);

    if (strcmp(verify_password, signupdata.password) == 0)
        goto verification_successful;
    else {
        printf("\n\n\t-----------------------------------------------------");
        printf("\n\n\t\t\tUnmatching Passwords !!!\n\t\tRe-enter your password correctly\n");
        printf("\n\t-----------------------------------------------------");
        delay(2);
        strcpy(signup_verify.name, signupdata.name);
        signup_verify.phone = signupdata.phone;
        signup_attempt = 1;
        goto reenter_password;
    }

    verification_successful:
    signupdata.balance = initial_balance; // initializing balance to 1000

    encrypt(signupdata.password);
    fwrite(&signupdata, sizeof(struct userdata), 1, login_data_file);
    fclose(login_data_file);
    printf("\n\n\t-----------------------------------------------------");
    printf("\n\n\t\t***Sign-Up Successful***\t\n");
    printf("\t\t***You can now Login***");
    printf("\n\n\t-----------------------------------------------------");
    delay(1.5);
    login();
}



// function to encrypt user's password
void encrypt(char pass[]){
    int i;
    for(i=0; pass[i]!='\0'; i++){
        pass[i] += 350;
    }
}

// function to decrypt encrypted password
void decrypt(char pass[]){
    int i;
    for(i=0; pass[i]!='\0'; i++) pass[i]-=350;
}

// function to create a certain dealy of time while displaying in console
void delay(int number_of_seconds){
    int milli_seconds = 1000 * number_of_seconds;       // Converting time into milli_seconds
    clock_t start_time = clock();                       // Storing start time
    while (clock() < start_time + milli_seconds);       // looping till required time is not achieved
}

void menu_handling(){
    char menu_choice;
    do{
    display_menu(current_user.name);
    printf("\tPlease Enter your choice [1-6]:  ");
    menu_choice = getche();
    }
    while(!(menu_choice >= 49 && menu_choice <=54));    // using character location '1'->49,'2'->50...... '5'->53
    switch(menu_choice){
        case '1':{
            showdetails();                                  // calling function to show details of all the users:
            break;
        }

        case '2':{
            send_money();                                  // calling function to send money
            break;
        }

        case '3':{
            pay_school_fee();                               // calling function to pay school fee
            break;
        }

        case '4':
        {
            viewTransactionLogs();
        }
        case '5':{
            main();
            break;
        }
        case '6':{
            exit_program();                                 // calling function to exit program
        }


    }
}

void showdetails(){
    struct userdata users[1000];                     // initial user's data to be hold
    // FILE = *user_data_file
    user_data_file = fopen("logindata.dat", "rb");   // open in binary mode

    int n=0;  // to keep in track of num of users in that file
    while(fread(&users[n], sizeof(struct userdata), 1, user_data_file) == 1){
        if(strcmp(users[n].name, current_user.name) == 0 && users[n].phone == current_user.phone){
            current_user.balance = users[n].balance;
        }
    n++;
    }
    fclose(user_data_file);
    system("cls");
    printf("\n\n\t\tDETAILS\n\t-------------------------------\n");
    printf("\n\tName: %s\n\tPhone: %.0f\n\tCurrent Balance: Rs %.2f\n\t-------------------------------",
    current_user.name, current_user.phone, current_user.balance);
    printf("\n\tEnter any key to go back to menu: ");       // returning to main menu
    getch();
    menu_handling();
}

void send_money(){
    struct userdata receiver, users[1000];
    int is_transaction_successful = 0;
    user_data_file = fopen("logindata.dat", "rb");       // opening previous file

    // load all the previous data
    int n=0;
    while(fread(&users[n], sizeof(struct userdata), 1, user_data_file) == 1) n++;
    fclose(user_data_file); // close file

    system("cls");
    printf("\n\t\tFUND - TRANSFER");
    printf("\n\t----------------------------------------\n");
    printf("\n\tWhom do you want to send money? \n");
    printf("\n\tUser\tPhone"
            "\n\t------------------------");

    // print all user's name and phone number
    for(int i=0; i<n; i++){
        if(!(strcmp(users[i].name, current_user.name) ==0 && users[i].phone == current_user.phone))
        printf("\n\t%s\t%.0f", users[i].name, users[i].phone);
        else current_user.balance = users[i].balance;                   // take hold of current user balance
    }

    printf("\n\t------------------------\n");
    printf("\n\tEnter receiver username: ");
    scanf("%s", receiver.name);
    printf("\tEnter receiver phone number: ");
    scanf("%lf", &receiver.phone);

    // check if receiver name and phone number are correct
    for(int i=0; i<n; i++){
        if(strcmp(users[i].name, receiver.name) ==0 && users[i].phone == receiver.phone){
            printf("\tEnter amount to send:  ");
            scanf("%lf", &receiver.balance);

            // if sending balance is greater than current balance
            if(!(receiver.balance > current_user.balance)){
                users[i].balance += abs(receiver.balance);           // taking absolute value of sending money
                is_transaction_successful = 1;
                break;
            }

        // if balance is insufficient
            else{
                printf("\n\t*** Insufficient Balance ***\n\tYour current balance is: %.2f\n", current_user.balance);
            }
        }
    }

    // transaction becomes successful then deduce money and modify the file
    if(is_transaction_successful){
        for(int j=0; j<n; j++){
            if(strcmp(users[j].name, current_user.name) == 0 && users[j].phone == current_user.phone){
            users[j].balance -= abs(receiver.balance);
            printf("\n\tYour current balance is: %.2f", users[j].balance);
            printf("\n\tTransaction Successful !");
            logTransaction(current_user.name, "Sent", receiver.balance);
            logTransaction(receiver.name, "Received", receiver.balance);
            }
        }

    user_data_file = fopen("logindata.dat", "wb");
    fwrite(&users, sizeof(struct userdata), n, user_data_file);
    fclose(user_data_file);
    }
    else printf("\n\tYou entered invalid receiver details!\n\tTransaction is not successful!\n");
    printf("\n\t--------------------------------------------\n");
    printf("\n\tEnter any key to go back to menu: ");
    getch();
    menu_handling();
}


void pay_school_fee(){
    struct userdata users[100];  // holds details of all the user initially present in file
    int school_choice;           // to take choice of school
    struct school{               // sturcture definition for school
        char name[150];
        float fee;
    } s[5];

    strcpy(s[0].name, "IOE Pulchowk"); s[0].fee = 1000;
    strcpy(s[1].name, "IOE WRC"); s[1].fee = 1500;
    strcpy(s[2].name, "Thapathali"); s[2].fee = 1500;
    strcpy(s[3].name, "IOE ERC"); s[3].fee = 1500;
    strcpy(s[4].name, "Advanced College"); s[4].fee = 3000;

    user_data_file = fopen("logindata.dat", "rb");   // reading initial data
    int n=0;
    while(fread(&users[n], sizeof(struct userdata), 1, user_data_file) == 1){
        if(strcmp(users[n].name, current_user.name) == 0 && users[n].phone == current_user.phone){
            current_user.balance = users[n].balance; // keep track of the initial balance
        }
    n++;
    }
    fclose(user_data_file);

    do{    // taking correct choice
    system("cls");
    printf("\n\n\t\tCollege Fee Payment");
    printf("\n\t----------------------------------------\n");
    printf("\n\tThe College list is:\n");
    for(int i=0; i<5; i++){
        printf("\t%d. %-25s %.2f\n", i+1, s[i].name, s[i].fee);
    }
    printf("\n\tEnter your choice [1-5]: ");
    scanf("%d", &school_choice);
    }while(!(school_choice >0 && school_choice <6));

    if(current_user.balance < s[school_choice-1].fee){   // checking for sufficiency
        printf("\n\tInsufficient balance!!!");
    }
    else {
    current_user.balance -= s[school_choice-1].fee;         // deducing money
    printf("\n\tTransaction Successful !");
    printf("\n\tYour current balance is: %.2f\n", current_user.balance);

    // loopiong through the array:
    for(int i=0; i<n; i++){
        if(strcmp(users[i].name, current_user.name)==0 && users[i].phone == current_user.phone){
            users[i].balance = current_user.balance;                // keep track of the initial balance
        }
    }

    user_data_file = fopen("logindata.dat", "wb");
    fwrite(&users, sizeof(struct userdata), n, user_data_file);
    fclose(user_data_file);
      logTransaction(current_user.name, "Paid School Fee", s[school_choice - 1].fee);
    }
    printf("\n\t--------------------------------------------\n");
    printf("\n\tEnter any key to go back to menu: ");
    getch();
    menu_handling();
}

// exiting the program
void exit_program(){
    system("cls");
    epay();
    delay(1);
    printf("\n\n\tThanks for using E-Pay\n");
    delay(2);
    exit(0);
}
