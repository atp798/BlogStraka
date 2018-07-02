#include<stdio.h>
#include<math.h>
#include<time.h>

int CalDaysBetween(int yStart,int yEnd){
	int days=0;int i;
	for(i=yStart;i<yEnd;i++)	days+=(i%400==0||(i%100!=0&&i%4==0))?366:365;
	return days;
}

int MultiplesBetween(int start,int end,int div){
	//if(start > end )	return 0;  //can be remarked because of usage
	int modstart,modend;
	modstart=start%div==0?1:0;
	modend=end%div==0?1:0;
	//printf("floor: %d --", floor((end-1)/div)-floor((start+1)/div));
    return modend + modstart + floor((end-0.5)/div)-floor((start+0.5)/div);  //floor
}

int CalDaysBetween2(int yStart,int yEnd){
	yEnd-=1;
    if (yStart>yEnd){ return 0; }
	int num4,num100,num400;
	num4=MultiplesBetween(yStart,yEnd,4);
	num100=MultiplesBetween(yStart,yEnd,100);
	num400=MultiplesBetween(yStart,yEnd,400);
	//printf("year--%d # %d $$%d %d %d \n",yStart,yEnd,num4,num100,num400);
	int count_grandyear=num4-num100+num400;
	return (365*(yEnd-yStart+1)+count_grandyear);
}

int inline yearType(int year){
    return year%400==0||(year%100!=0&&year%4==0) ?1:0;
    //return ((year%4==0&&year%100!=0)||(year%400==0&&year%3200!=0)||year%172800==0)?1:0;
}

int CalDaysBetween3(int yStart,int yEnd){
    //return 365*yEnd + yEnd/4-yEnd/100+yEnd/400 - (365*yStart + yStart/4-yStart/100+yStart/400) + (yStart%400==0||(yStart%100!=0&&yStart%4==0) ?1:0) - (yEnd%400==0||(yEnd%100!=0&&yEnd%4==0) ?1:0);
    //return 365*yEnd + yEnd/4-yEnd/100+yEnd/400 - (365*yStart + yStart/4-yStart/100+yStart/400) + yearType(yStart) - yearType(yEnd);
    return 365*(yEnd-yStart) + yEnd/4-yEnd/100+yEnd/400 - (yStart/4-yStart/100+yStart/400)  + (yStart%400==0||(yStart%100!=0&&yStart%4==0) ?1:0) - (yEnd%400==0||(yEnd%100!=0&&yEnd%4==0) ?1:0);
}

int CalDaysOfYear(int year,int month,int day){
    int monthLen[2][12]={{31,28,31,30,31,30,31,31,30,31,30,31},{31,29,31,30,31,30,31,31,30,31,30,31}};
	int yearT= yearType(year);
	//this two line below will support overflows params
	month = (month-1)%12;
	//day = day%monthLen[yearStyle][month];
	int ret=0;
	int i=0;
	for(i=0;i<month;i++)	ret+=monthLen[yearT][i];
	ret+=day;
	return ret;
}

int CalDaysOfYear2(int year,int month,int day){
    int daysSum[2][12]={{0,31,59,90,120,151,181,212,243,273,304,334},{0,31,60,91,121,152,182,213,244,274,305,335}};
	int yearT= yearType(year);
	return daysSum[yearT][month-1] + day;
}

int CalDaysInterval(int yStart,int mStart, int dStart,int yEnd, int mEnd,int dEnd){
    int daysStart = CalDaysOfYear2(yStart,mStart,dStart);
    int daysEnd = CalDaysOfYear2(yEnd,mEnd,dEnd);
    int daysBetween = CalDaysBetween(yStart,yEnd);
    //printf("%d-%d-%d\n",daysStart,daysBetween,daysEnd);
    return daysBetween - daysStart + daysEnd;
}

int CalDaysInterval2(int yStart,int mStart, int dStart,int yEnd, int mEnd,int dEnd){
    int startT = yStart%400==0||(yStart%4==0&&yStart%100!=0) ?1:0;
    int endT = yEnd%400==0||(yEnd%4==0&&yEnd%100!=0) ?1:0;
    int daysSum[2][12]={{0,31,59,90,120,151,181,212,243,273,304,334},{0,31,60,91,121,152,182,213,244,274,305,335}};
    return 365*(yEnd-yStart) + yEnd/4-yEnd/100+yEnd/400 - (yStart/4-yStart/100+yStart/400) + startT - endT - (daysSum[startT][mStart-1] + dStart) + daysSum[endT][mEnd-1] + dEnd;
}

int DaysFrom0000(int year,int month, int day){
    int m = (month + 9)%12;
    int y = year-m/10;
    return 365*y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + day;
}

int CalDaysInterval3(int yStart,int mStart, int dStart,int yEnd, int mEnd,int dEnd){
    int daysStart = DaysFrom0000(yStart,mStart, dStart);
    int daysEnd = DaysFrom0000(yEnd,mEnd,dEnd);
    //printf("%d-%d\n",daysStart,daysEnd);
    return daysEnd - daysStart;
}

int CalDaysInterval4(int yStart,int mStart, int dStart,int yEnd, int mEnd,int dEnd){
    int m1,y1,m2,y2;
    m1 = (mStart + 9)%12;
    y1 = yStart-m1/10;
    m2 = (mEnd + 9)%12;
    y2 = yEnd-m2/10;
    return 365*(y2-y1) + y2/4 - y2/100 + y2/400 + (m2*306 + 5)/10 + dEnd - ( y1/4 - y1/100 + y1/400 + (m1*306 + 5)/10 + dStart);
}

int CalDaysInterval5(int yStart,int mStart, int dStart,int yEnd, int mEnd,int dEnd){
    int startT = ((yStart%4==0&&yStart%100!=0)||(yStart%400==0&&yStart%3200!=0)||yStart%172800==0)?1:0;
    int endT = ((yEnd%4==0&&yEnd%100!=0)||(yEnd%400==0&&yEnd%3200!=0)||yEnd%172800==0)?1:0;
    int daysSum[2][12]={{0,31,59,90,120,151,181,212,243,273,304,334},{0,31,60,91,121,152,182,213,244,274,305,335}};
    return 365*(yEnd-yStart) + yEnd/4-yEnd/100+yEnd/400-yEnd/3200+yEnd/172800 - (yStart/4-yStart/100+yStart/400-yStart/3200+yStart/172800) + startT - endT - (daysSum[startT][mStart-1] + dStart) + daysSum[endT][mEnd-1] + dEnd;
}

int CalDaysInterval6(int yStart,int mStart, int dStart,int yEnd, int mEnd,int dEnd){
    int m1,y1,m2,y2;
    m1 = (mStart + 9)%12;
    y1 = yStart-m1/10;
    m2 = (mEnd + 9)%12;
    y2 = yEnd-m2/10;
    return 365*(y2-y1) + y2/4 - y2/100 + y2/400 - y2/3200 + y2/172800 + (m2*306 + 5)/10 + dEnd - ( y1/4 - y1/100 + y1/400- y1/3200 + y1/172800 + (m1*306 + 5)/10 + dStart);
}

void TestYearMethod(){
    clock_t start,mid,mid2,end;
	double total,total1,total2;
	total = total1 = total2= 0;
	int daymid1,daymid2;
	int k=0;
	int i=0;
	printf("year\t   method-1  method-2  method-3\n");
	int timearr[8][2]={{1990,2000},{1980,2000},{1970,2000},{1950,2000},{1800,2000},{1500,2000},{1000,2000},{1700,1900}};
	for(i=0;i<8;i++){
	    int cycles = 500000;
        start = clock();
        for(k=0;k<cycles;k++){
            daymid1=CalDaysBetween(timearr[i][0],timearr[i][1]);
        }
        mid = clock();
        for(k=0;k<cycles;k++){
            daymid2=CalDaysBetween2(timearr[i][0],timearr[i][1]);
        }
        mid2 = clock();
        for(k=0;k<cycles;k++){
            CalDaysBetween3(timearr[i][0],timearr[i][1]);
        }
        end = clock();
        if(daymid1!=daymid2){
            printf("%d-%d %d-%d\n",timearr[i][0],timearr[i][1],daymid1,daymid2);
        }
        total += mid-start;
        total1 += mid2 -mid;
        total2 += end -mid2;
        printf("%d-%d   %-9.3lf %-9.3lf %-9.3lf\n",timearr[i][0],timearr[i][1],total/1000,total1/1000,total2/1000);
	}
}

void TestDaysMethod(){
	int y1,m1,d1,y2,m2,d2;
	y1=1599;m1=3;d1=1;
	y2=1600;m2=3;d2=1;
    clock_t start,mid,end;
	double total,total1;
	total = total1 =  0;
	int daymid1,daymid2;
	int k=0;
	int i=0;
	printf("year\t\t method-1 method-2(new)\n");
	start = clock();
	    int cycles = 500000;
        for(k=0;k<cycles;k++){
            daymid1=CalDaysInterval5(y1,m1,d1,y2,m2,d2);
        }
        mid = clock();
        for(k=0;k<cycles;k++){
            daymid2=CalDaysInterval6(y1,m1,d1,y2,m2,d2);
        }
        end = clock();
        total += mid-start;
        total1 += end -mid;
        printf("%d-%d-%d %d-%d-%d  %.3lf     %.3lf\n",daymid1,daymid2,d1,y2,m2,d2,total/1000,total1/1000);
}

int main(){
    TestYearMethod();
	TestDaysMethod();
}
