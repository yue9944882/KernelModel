char *strstr(const char *s1, const char *s2)   
{  
    int n;  
    if (*s2)   
    {   
        while (*s1)   
        {   
            for (n = 0; *(s1 + n) == *(s2 + n); n ++)  
            {   
                if (!*(s2 + n + 1))   
                    return (char *)s1;   
            }   
            s1++;   
        }  
        return NULL;   
    }   
    else   
        return (char *)s1;   
}  

char *strtok(char *str, char *ctrl)
{
     char *p=str,*q=ctrl,*r;
     while(p&&q&&*p&&*q){
         for(r=p,q=ctrl;*r&&*q&&*r==*q;r++,q++);
         if(q&&*q) p++; 
         else {*p='\0'; break;} 
     }
     return str;
}