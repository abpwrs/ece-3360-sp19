int toDeci(char*,int);
int val(char c);


int main(int argc,char * argv[]){
    if(argc < 2){
        printf("Invalid params\n");
        return -1;
    }
    printf("%s in base %d is %d\n", argv[1], atoi(argv[2]), toDeci(argv[1],atoi(argv[2])));

    return 0;
}


// https://www.geeksforgeeks.org/convert-base-decimal-vice-versa/
int toDeci(char *str, int base) 
{ 
    int len = strlen(str);
    int power = 1;
    int num = 0;  
    int i; 

    for (i = len - 1; i >= 0; i--) 
    { 
        if (val(str[i]) >= base) 
        { 
           printf("Invalid Number"); 
           return -1; 
        } 
        num += val(str[i]) * power; 
        power = power * base; 
    } 
    return num; 
} 

int val(char c) 
{ 
    if (c >= '0' && c <= '9') 
        return (int)c - '0'; 
    else
        return (int)c - 'A' + 10; 
} 
