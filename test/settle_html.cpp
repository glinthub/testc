#include<stdio.h>
int settle_html_main()
{
    char *str, *str1, *p, *sfilename;
    FILE *fp, *fp1;
    int writeflag;

    clrscr();
    writeflag = 0;
    fp=fopen("cf.htm", "r");
    while (!feof(fp)) {
        fgets(str, 1024, fp);
        if (p = strstr(str,"函数名")) {
            str1 = p + 8;
            /*puts(str1);*/
            if (p = strstr(str1,"函数名")) {
                str1 = p + 8;
                if (p=strstr(str1,"\n"))
                {
                    *p=NULL;
                    writeflag = 1;
                    *sfilename = NULL;
                    strcat(sfilename,str1);
                    strcat(sfilename,".htm");

                    /**/
                    fclose(fp1);
                    fp1 = fopen(sfilename, "a");
                    fputs("<title>",fp1);
                    fputs(str1,fp1);
                    fputs("</title>\n",fp1);
                    /**/

                    /**/printf("new file:%s\n",sfilename); /**/
                    /** if (getch()==27) exit(); **/
                }
            }
        }
        if (writeflag) {
            /**/fputs(str, fp1); /**/
            /* fputs("\n", fp1); */
        }
    }

    fclose(fp1);
    fclose(fp);
    puts("congraduations! Task completed!")
    getch();
}
/**/

