#include <nds.h>
#include <nds/arm9/background.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fat.h>
#include <stdint.h>

/* Elemental Legends: original DS homebrew, no commercial assets. */
#define SW 256
#define SH 192
#define MAPW 32
#define MAPH 24
#define RGB(r,g,b) RGB15(r,g,b)
static u16 *top = NULL;
static u16 *bottom = NULL;
static int mode=0, px=3,py=3, facing=0, frame=0, level=1, hp=120,chi=100, foeHP=0,foeMax=0,foe=0,element=0,avatar=0,herbs=3,balls=6, captured=0, steps=0, wins=0;
static char msg[80]="Explore the Spirit Grove";
static const char *enames[]={"AIR","WATER","EARTH","FIRE"};
static const char *moves[]={"CYCLONE","TIDAL DRAGON","METEOR","PHOENIX"};
static const char *foes[]={"MOSS WISP","EMBER FOX","STONE CUB","STORM OWL","VOID WARDEN"};
static const int maxhp[]={60,85,105,125,210};
static const u16 ec[]={RGB(20,29,26),RGB(5,20,31),RGB(25,19,7),RGB(31,12,5)};
static int clamp(int n,int a,int b){return n<a?a:n>b?b:n;}
static void rect(u16 *v,int x,int y,int w,int h,u16 c){int x0=clamp(x,0,SW),y0=clamp(y,0,SH),x1=clamp(x+w,0,SW),y1=clamp(y+h,0,SH);for(int j=y0;j<y1;j++)for(int i=x0;i<x1;i++)v[j*SW+i]=c|BIT(15);}
static void disc(u16*v,int cx,int cy,int r,u16 c){for(int y=-r;y<=r;y++)for(int x=-r;x<=r;x++)if(x*x+y*y<=r*r){int xx=cx+x,yy=cy+y;if(xx>=0&&xx<SW&&yy>=0&&yy<SH)v[yy*SW+xx]=c|BIT(15);}}
static const unsigned char font[][5]={
{0,0,0,0,0}, {7,5,5,0,5}, {5,5,0,0,0}, {5,7,5,7,5}, {2,7,2,7,2}, {5,1,2,4,5}, {2,5,2,5,2}, {2,2,0,0,0}, {1,2,2,2,1}, {4,2,2,2,4}, {5,2,7,2,5}, {0,2,7,2,0}, {0,0,0,2,4}, {0,0,7,0,0}, {0,0,0,0,2}, {1,1,2,4,4},
{7,5,5,5,7},{2,6,2,2,7},{7,1,7,4,7},{7,1,7,1,7},{5,5,7,1,1},{7,4,7,1,7},{7,4,7,5,7},{7,1,1,1,1},{7,5,7,5,7},{7,5,7,1,7},{0,2,0,2,0},{0,2,0,2,4},{1,2,4,2,1},{0,7,0,7,0},{4,2,1,2,4},{7,1,3,0,2},
{7,5,7,4,7},{2,5,7,5,5},{6,5,6,5,6},{3,4,4,4,3},{6,5,5,5,6},{7,4,6,4,7},{7,4,6,4,4},{3,4,5,5,3},{5,5,7,5,5},{7,2,2,2,7},{1,1,1,5,2},{5,5,6,5,5},{4,4,4,4,7},{5,7,7,5,5},{5,7,7,7,5},{2,5,5,5,2},{6,5,6,4,4},{2,5,5,7,3},{6,5,6,5,5},{3,4,2,1,6},{7,2,2,2,2},{5,5,5,5,7},{5,5,5,5,2},{5,5,7,7,5},{5,5,2,5,5},{5,5,2,2,2},{7,1,2,4,7}};
static void txt(u16*v,int x,int y,const char*s,u16 color,int scale){for(;*s;s++,x+=4*scale){int ch=(unsigned char)*s;int idx=ch-32;if(ch>='a'&&ch<='z')idx=ch-'a'+'A'-32;if(idx<0||idx>=59)idx=0;for(int row=0;row<5;row++)for(int col=0;col<3;col++)if(font[idx][row]&(4>>col))rect(v,x+col*scale,y+row*scale,scale,scale,color);}}
static void button(int x,int y,int w,int h,const char*label,u16 color){rect(bottom,x,y,w,h,RGB(3,7,13));rect(bottom,x+2,y+2,w-4,h-4,color);txt(bottom,x+8,y+h/2-4,label,RGB(31,31,31),1);}
static int groveTerrain(int x,int y){if(x<0||y<0||x>=MAPW||y>=MAPH)return 1;if(x==0||y==0||x==MAPW-1||y==MAPH-1)return 1;if((x*17+y*11)%29==0)return 1;if((x>13&&x<18&&y>7&&y<18))return 2;if((x*3+y*7)%17<4)return 3;return 0;}
static void avatarSprite(u16*v,int x,int y,int tick){disc(v,x,y-9,5,RGB(28,21,15));rect(v,x-5,y-5,10,12,RGB(30,20,6));rect(v,x-4,y+7,3,5,RGB(10,12,23));rect(v,x+1,y+7,3,5,RGB(10,12,23));rect(v,x-6,y-13,12,3,RGB(10,10,13));if(tick%24<12)rect(v,x-8,y,3,6,RGB(28,21,15));else rect(v,x+5,y,3,6,RGB(28,21,15));}
static void creature(u16*v,int x,int y,int type,int tick){u16 c=ec[type%4];disc(v,x,y+(tick%32<16?0:-2),20,c);disc(v,x-12,y-17,9,c);disc(v,x+12,y-17,9,c);disc(v,x-8,y-2,4,RGB(31,31,31));disc(v,x+8,y-2,4,RGB(31,31,31));disc(v,x-8,y-2,2,RGB(0,0,0));disc(v,x+8,y-2,2,RGB(0,0,0));rect(v,x-5,y+9,10,3,RGB(31,25,22));}

#define PARTY_MAX 4
#define SAVE_MAGIC 0x454C4453u
#define SAVE_VERSION 2
#define MODE_WORLD 0
#define MODE_BATTLE 1
#define MODE_DIALOG 2
#define MODE_PARTY 3
#define MODE_QUEST 4
#define MODE_FAINT 5
#define MODE_SAVE 6
#define MAP_GROVE 0
#define MAP_VILLAGE 1
#define MAP_TEMPLE 2
#define QUEST_COUNT 3
static int zone=MAP_GROVE, dialogue=0, questFlags=0, questProgress[QUEST_COUNT]={0}, selected=0, partyCount=1, active=0, gameMessageTimer=0;
static int partyKind[PARTY_MAX]={0},partyHP[PARTY_MAX]={55},partyXP[PARTY_MAX]={0},partyLevel[PARTY_MAX]={1};
static int saveReady=0, saveResult=0, encounterLock=0, battleKind=0;
static const char *zoneNames[]={"SPIRIT GROVE","LOTUS VILLAGE","ANCIENT TEMPLE"};
static const char *creatureNames[]={"BREEZE LYNX","EMBER FOX","STONE CUB","TIDE OTTER","VOID WARDEN"};
static const char *npcNames[]={"ELDER SORA","HEALER MIRA","RANGER KAI"};
static const char *npcLines[][3]={
 {"SPIRITS ARE RESTLESS.","DEFEAT THREE WILD SPIRITS.","RESTORE BALANCE TO GROVE."},
 {"WELCOME TO LOTUS VILLAGE.","I CAN HEAL YOUR PARTY.","RETURN WHENEVER NEEDED."},
 {"MY ORBS ARE SCATTERED.","CAPTURE TWO CREATURES.","I WILL REWARD YOUR WORK."}
};
static const char *questNames[]={"GROVE GUARDIAN","SPIRIT COLLECTOR","TEMPLE TRIAL"};
static const int questTarget[]={3,2,1};
static const char *questDetail[]={"WIN 3 WILD BATTLES","CAPTURE 2 CREATURES","DEFEAT VOID WARDEN"};
static int max2(int a,int b){return a>b?a:b;}
static int min2(int a,int b){return a<b?a:b;}
static int passable(int x,int y){
 if(x<1||y<1||x>=MAPW-1||y>=MAPH-1)return 0;
 if(zone==MAP_GROVE){int t=groveTerrain(x,y);return t!=1&&t!=2;}
 if(zone==MAP_VILLAGE){if((x>=5&&x<=10&&y>=5&&y<=9)||(x>=18&&x<=25&&y>=5&&y<=10))return 0;return 1;}
 if((x>=7&&x<=11&&y>=8&&y<=14)||(x>=21&&x<=25&&y>=8&&y<=14))return 0;
 return 1;
}
static int cell(int x,int y){
 if(x<0||y<0||x>=MAPW||y>=MAPH)return 1;
 if(zone==MAP_GROVE)return groveTerrain(x,y);
 if(zone==MAP_VILLAGE){if(!passable(x,y))return 4;if((x+y)%11==0)return 3;return 0;}
 if(!passable(x,y))return 4;
 return (x+y)%7==0?3:0;
}
static void setMsg(const char *m){snprintf(msg,sizeof(msg),"%s",m);gameMessageTimer=180;}
static void resetGame(void){zone=MAP_VILLAGE;px=15;py=13;hp=120;chi=100;level=1;wins=0;steps=0;captured=0;balls=8;herbs=4;avatar=0;mode=MODE_WORLD;partyCount=1;active=0;selected=0;questFlags=0;for(int i=0;i<QUEST_COUNT;i++)questProgress[i]=0;for(int i=0;i<PARTY_MAX;i++){partyKind[i]=0;partyHP[i]=55;partyXP[i]=0;partyLevel[i]=1;}setMsg("TALK TO ELDER SORA");}
typedef struct {
 uint32_t magic,version,checksum;
 int32_t zone,px,py,hp,chi,level,wins,captured,balls,herbs,questFlags;
 int32_t questProgress[QUEST_COUNT],partyCount,active,partyKind[PARTY_MAX],partyHP[PARTY_MAX],partyXP[PARTY_MAX],partyLevel[PARTY_MAX];
} SaveData;
static uint32_t checksum(const SaveData *d){const unsigned char *b=(const unsigned char*)d;uint32_t h=2166136261u;for(unsigned int i=12;i<sizeof(*d);i++){h^=b[i];h*=16777619u;}return h;}
static int saveGame(void){if(!saveReady)return 0;SaveData d;memset(&d,0,sizeof(d));d.magic=SAVE_MAGIC;d.version=SAVE_VERSION;
 d.zone=zone;d.px=px;d.py=py;d.hp=hp;d.chi=chi;d.level=level;d.wins=wins;d.captured=captured;d.balls=balls;d.herbs=herbs;d.questFlags=questFlags;d.partyCount=partyCount;d.active=active;
 for(int i=0;i<QUEST_COUNT;i++)d.questProgress[i]=questProgress[i];
 for(int i=0;i<PARTY_MAX;i++){d.partyKind[i]=partyKind[i];d.partyHP[i]=partyHP[i];d.partyXP[i]=partyXP[i];d.partyLevel[i]=partyLevel[i];}d.checksum=checksum(&d);
 FILE*f=fopen("fat:/elemental_legends.sav","wb");if(!f)return 0;int ok=fwrite(&d,1,sizeof(d),f)==sizeof(d);if(fclose(f)!=0)ok=0;return ok;
}
static int loadGame(void){if(!saveReady)return 0;SaveData d;FILE*f=fopen("fat:/elemental_legends.sav","rb");if(!f)return 0;int ok=fread(&d,1,sizeof(d),f)==sizeof(d);fclose(f);if(!ok||d.magic!=SAVE_MAGIC||d.version!=SAVE_VERSION||d.checksum!=checksum(&d))return 0;
 if(d.level<1||d.level>999||d.wins<0||d.wins>99999||d.captured<0||d.captured>99999||d.questFlags<0||d.questFlags>127)return 0;
 if(d.zone<0||d.zone>2||d.px<1||d.px>=MAPW-1||d.py<1||d.py>=MAPH-1||d.partyCount<1||d.partyCount>PARTY_MAX||d.active<0||d.active>=d.partyCount||d.hp<1||d.hp>120||d.chi<0||d.chi>100||d.balls<0||d.balls>999||d.herbs<0||d.herbs>999)return 0;
 for(int i=0;i<QUEST_COUNT;i++)if(d.questProgress[i]<0||d.questProgress[i]>questTarget[i])return 0;
 for(int i=0;i<PARTY_MAX;i++)if(d.partyKind[i]<0||d.partyKind[i]>4||d.partyHP[i]<0||d.partyHP[i]>150||d.partyLevel[i]<1||d.partyLevel[i]>99||d.partyXP[i]<0)return 0;
 zone=d.zone;px=d.px;py=d.py;hp=d.hp;chi=d.chi;level=d.level;wins=d.wins;captured=d.captured;balls=d.balls;herbs=d.herbs;questFlags=d.questFlags;partyCount=d.partyCount;active=d.active;for(int i=0;i<QUEST_COUNT;i++)questProgress[i]=clamp(d.questProgress[i],0,questTarget[i]);for(int i=0;i<PARTY_MAX;i++){partyKind[i]=d.partyKind[i];partyHP[i]=d.partyHP[i];partyXP[i]=d.partyXP[i];partyLevel[i]=d.partyLevel[i];}mode=MODE_WORLD;setMsg("ADVENTURE LOADED");return 1;
}
static void rewardQuest(int q){if(questFlags&(1<<(q+3)))return;questFlags|=1<<(q+3);if(q==0){balls+=4;herbs+=2;setMsg("REWARD 4 ORBS 2 HERBS");}else if(q==1){balls+=5;setMsg("REWARD 5 SPIRIT ORBS");}else{hp=120;chi=100;setMsg("BALANCE RESTORED!");}}
static void questAdvance(int q){if(!(questFlags&(1<<q)))return;if(questProgress[q]<questTarget[q])questProgress[q]++;if(questProgress[q]>=questTarget[q])rewardQuest(q);}
static void npcSprite(u16*v,int x,int y,int n){disc(v,x,y-8,5,RGB(29,23,18));rect(v,x-5,y-3,10,13,n==0?RGB(14,15,28):n==1?RGB(8,23,22):RGB(26,14,9));rect(v,x-4,y+10,3,4,RGB(6,8,12));rect(v,x+1,y+10,3,4,RGB(6,8,12));}
static void worldRender(void){
 rect(top,0,0,256,192,RGB(8,14,18));for(int sy=0;sy<12;sy++)for(int sx=0;sx<16;sx++){
 int mx=px-8+sx,my=py-6+sy,x=sx*16,y=sy*16,t=cell(mx,my);
 u16 c=zone==MAP_GROVE?(t==1?RGB(7,14,11):t==2?RGB(7,14,24):t==3?RGB(7,22,10):RGB(12,20,11)):(zone==MAP_VILLAGE?RGB(19,19,13):RGB(14,13,19));
 rect(top,x,y,16,16,c);if(t==1&&zone==MAP_GROVE){disc(top,x+8,y+7,7,RGB(5,23,9));rect(top,x+6,y+10,4,6,RGB(12,8,5));}else if(t==2){rect(top,x,y+(frame/10+sx)%3,16,2,RGB(9,21,29));}else if(t==3){rect(top,x+3,y+6,2,5,RGB(15,28,12));rect(top,x+11,y+4,2,5,RGB(15,28,12));}else if(t==4){rect(top,x,y,16,16,RGB(19,10,6));rect(top,x+3,y+3,10,8,RGB(26,17,10));}}
 if(zone==MAP_VILLAGE){const int nx[]={13,18,10},ny[]={10,13,16};for(int i=0;i<3;i++){int sx=(nx[i]-px+8)*16+8,sy=(ny[i]-py+6)*16+8;if(sx>=0&&sx<256&&sy>=0&&sy<192)npcSprite(top,sx,sy,i);}}
 if(zone==MAP_TEMPLE){int sx=(16-px+8)*16+8,sy=(9-py+6)*16+8;if(sx>=0&&sx<256&&sy>=0&&sy<192)creature(top,sx,sy,4,frame);}
 avatarSprite(top,128,96,frame);rect(top,0,0,256,13,RGB(2,5,8));txt(top,5,4,zoneNames[zone],RGB(31,28,18),1);char b[45];sprintf(b,"HP %d CHI %d",hp,chi);txt(top,162,4,b,RGB(31,31,31),1);
 rect(top,0,177,256,15,RGB(2,5,8));txt(top,5,181,msg,RGB(26,30,23),1);
 rect(bottom,0,0,256,192,RGB(4,9,15));txt(bottom,9,8,"ELEMENTAL LEGENDS",RGB(31,26,13),2);
 txt(bottom,9,31,"D PAD MOVE  A TALK / ENTER",RGB(24,28,29),1);
 button(6,50,119,32,"PARTY",RGB(6,17,24));button(131,50,119,32,"QUESTS",RGB(18,13,24));
 button(6,88,119,32,"SAVE",RGB(8,21,14));button(131,88,119,32,"LOAD",RGB(20,15,7));
 button(6,126,119,32,"GROVE",RGB(8,20,12));button(131,126,119,32,"VILLAGE",RGB(20,17,10));
 txt(bottom,7,168,"START PARTY  SELECT QUESTS",RGB(25,27,29),1);
 txt(bottom,7,180,"X SAVE  Y LOAD",RGB(25,27,29),1);
}
static void dialogueRender(void){worldRender();rect(top,8,119,240,56,RGB(2,6,12));txt(top,14,126,npcNames[dialogue],RGB(31,27,14),2);txt(top,14,146,npcLines[dialogue][0],RGB(31,31,31),1);txt(top,14,157,npcLines[dialogue][1],RGB(26,29,30),1);
 rect(bottom,0,0,256,192,RGB(4,9,15));txt(bottom,12,16,"DIALOGUE",RGB(31,27,15),2);txt(bottom,12,47,npcLines[dialogue][2],RGB(29,30,31),1);
 if(dialogue==0)button(12,87,232,36,(questFlags&1)?"QUEST ACTIVE":"ACCEPT GROVE QUEST",RGB(12,21,15));else if(dialogue==2)button(12,87,232,36,(questFlags&2)?"QUEST ACTIVE":"ACCEPT CAPTURE QUEST",RGB(12,21,15));else button(12,87,232,36,"HEAL PARTY",RGB(10,22,20));button(12,137,232,34,"CLOSE",RGB(15,15,22));}
static void partyRender(void){rect(top,0,0,256,192,RGB(7,14,22));txt(top,10,10,"CREATURE PARTY",RGB(31,27,13),2);for(int i=0;i<partyCount;i++){int y=36+i*36;rect(top,8,y,240,31,i==selected?RGB(13,22,27):RGB(7,12,17));creature(top,30,y+15,partyKind[i],frame);char b[50];sprintf(b,"%s LV %d",creatureNames[partyKind[i]],partyLevel[i]);txt(top,62,y+6,b,RGB(31,31,31),1);sprintf(b,"HP %d XP %d",partyHP[i],partyXP[i]);txt(top,62,y+19,b,RGB(25,28,30),1);if(i==active)txt(top,210,y+19,"LEAD",RGB(31,26,9),1);}
 rect(bottom,0,0,256,192,RGB(4,9,15));txt(bottom,10,12,"SELECT A COMPANION",RGB(31,28,16),2);for(int i=0;i<partyCount;i++){char b[24];sprintf(b,"SLOT %d",i+1);button(8+(i%2)*124,45+(i/2)*42,116,35,b,i==selected?RGB(17,22,11):RGB(9,17,23));}button(8,142,116,37,"SET LEADER",RGB(9,22,14));button(132,142,116,37,"BACK",RGB(17,11,16));}
static void questRender(void){rect(top,0,0,256,192,RGB(8,12,20));txt(top,8,10,"QUEST JOURNAL",RGB(31,26,14),2);for(int i=0;i<QUEST_COUNT;i++){int y=39+i*47;char b[60];txt(top,10,y,questNames[i],RGB(31,31,31),1);sprintf(b,"%d / %d %s",questProgress[i],questTarget[i],(questFlags&(1<<(i+3)))?"DONE":((questFlags&(1<<i))?"ACTIVE":"LOCKED"));txt(top,10,y+13,b,RGB(24,30,26),1);txt(top,10,y+26,questDetail[i],RGB(27,24,18),1);}
 rect(bottom,0,0,256,192,RGB(4,9,15));txt(bottom,10,15,"QUEST PROGRESS",RGB(31,26,14),2);txt(bottom,10,55,"SORA: GROVE GUARDIAN",RGB(28,29,30),1);txt(bottom,10,75,"KAI: SPIRIT COLLECTOR",RGB(28,29,30),1);txt(bottom,10,95,"TEMPLE: FINAL TRIAL",RGB(28,29,30),1);button(8,139,240,40,"BACK TO WORLD",RGB(12,17,24));}
static void battleRender(void){rect(top,0,0,256,192,RGB(9,15,25));disc(top,200,42,29,RGB(14,21,29));rect(top,0,140,256,52,RGB(9,19,12));disc(top,183,115,50,RGB(13,24,16));disc(top,64,158,54,RGB(10,21,14));creature(top,178,83,battleKind,frame);avatarSprite(top,64,129,frame);char b[70];txt(top,8,8,creatureNames[battleKind],RGB(31,31,31),2);sprintf(b,"FOE %d/%d",foeHP,foeMax);txt(top,9,28,b,RGB(31,25,23),1);rect(top,10,38,110,7,RGB(6,7,8));rect(top,10,38,110*foeHP/max2(1,foeMax),7,RGB(28,7,6));sprintf(b,"HP %d CHI %d",hp,chi);txt(top,9,165,b,RGB(31,31,31),2);if(avatar)txt(top,9,182,"AVATAR STATE",RGB(31,29,9),1);
 rect(bottom,0,0,256,192,RGB(4,8,14));txt(bottom,7,7,msg,RGB(31,29,22),1);for(int i=0;i<4;i++){char a[24];sprintf(a,"%s%s",i==element?">":"",enames[i]);button(4+(i%2)*126,25+(i/2)*32,122,29,a,ec[i]);}
 button(4,95,122,30,"A STRIKE",RGB(8,15,24));button(130,95,122,30,"X SPECIAL",RGB(16,8,26));button(4,129,122,28,"Y AVATAR",RGB(23,17,5));button(130,129,122,28,"B COMPANION",RGB(8,19,12));button(4,161,122,28,"L HEAL",RGB(6,20,10));button(130,161,122,28,"R CAPTURE",RGB(24,8,9));}
static void startBattle(int kind){battleKind=kind;foeMax=kind==4?220:65+kind*17+level*3;foeHP=foeMax;avatar=0;mode=MODE_BATTLE;setMsg("A WILD SPIRIT APPEARED");}
static void finishBattle(int capturedNow){mode=MODE_WORLD;wins++;hp=clamp(hp+12,0,120);chi=clamp(chi+18,0,100);level=1+wins/3;partyXP[active]+=15;if(partyXP[active]>=partyLevel[active]*35&&partyLevel[active]<99){partyXP[active]=0;partyLevel[active]++;partyHP[active]=min2(150,partyHP[active]+12);}if(battleKind!=4)questAdvance(0);
 if(battleKind==4){questAdvance(2);setMsg("VOID WARDEN DEFEATED");}else if(capturedNow){questAdvance(1);setMsg("NEW SPIRIT BONDED");}else{setMsg("BATTLE WON");}encounterLock=4;}
static void enemyTurn(void){int d=5+battleKind*3+rand()%9;if(element==2)d=d*7/10;if(avatar)d=d/2;hp=clamp(hp-d,0,120);chi=clamp(chi+12,0,100);if(avatar)avatar--;if(hp==0){mode=MODE_FAINT;setMsg("YOU FAINTED");}}
static void strike(int special){int cost=special?30:0;if(chi<cost){setMsg("NOT ENOUGH CHI");return;}chi-=cost;int damage=(special?29:14)+rand()%(special?17:11);if(avatar)damage*=2;foeHP=clamp(foeHP-damage,0,foeMax);snprintf(msg,sizeof(msg),"%s DEALT %d",special?moves[element]:"STRIKE",damage);if(!foeHP)finishBattle(0);else enemyTurn();}
static void battleAction(int k){if(k&KEY_UP)element=0;if(k&KEY_RIGHT)element=1;if(k&KEY_DOWN)element=2;if(k&KEY_LEFT)element=3;
 if(k&KEY_A)strike(0);else if(k&KEY_X)strike(1);else if(k&KEY_Y){if(chi>=65&&!avatar){chi-=65;avatar=3;setMsg("AVATAR STATE ACTIVE");}else setMsg("NEED 65 CHI");}else if(k&KEY_B){if(chi>=12&&partyHP[active]>0){chi-=12;int dmg=20+partyLevel[active]*3;foeHP=clamp(foeHP-dmg,0,foeMax);partyHP[active]=max2(0,partyHP[active]-3);setMsg("COMPANION ASSIST");if(!foeHP)finishBattle(0);else enemyTurn();}else setMsg("COMPANION UNAVAILABLE");}else if(k&KEY_L){if(herbs){herbs--;hp=clamp(hp+50,0,120);setMsg("HEALED");enemyTurn();}else setMsg("NO HERBS LEFT");}else if(k&KEY_R){if(!balls)setMsg("NO SPIRIT ORBS");else if(foeHP*100>foeMax*40)setMsg("WEAKEN BELOW 40 PERCENT");else{balls--;if(rand()%100<75){if(partyCount<PARTY_MAX){partyKind[partyCount]=battleKind;partyHP[partyCount]=60;partyXP[partyCount]=0;partyLevel[partyCount]=1;partyCount++;captured++;finishBattle(1);}else{setMsg("PARTY FULL");balls++;}}else{setMsg("CAPTURE FAILED");enemyTurn();}}}}
static void interact(void){if(zone==MAP_VILLAGE){const int nx[]={13,18,10},ny[]={10,13,16};for(int i=0;i<3;i++)if(abs(px-nx[i])+abs(py-ny[i])<=2){dialogue=i;mode=MODE_DIALOG;return;}if(px>=29){zone=MAP_GROVE;px=3;py=12;setMsg("ENTERED SPIRIT GROVE");return;}if(py<=2&&(questFlags&8)){zone=MAP_TEMPLE;px=16;py=20;questFlags|=4;setMsg("ENTERED ANCIENT TEMPLE");return;}}
 else if(zone==MAP_GROVE){if(px<=2){zone=MAP_VILLAGE;px=28;py=12;setMsg("RETURNED TO VILLAGE");return;}}
 else if(zone==MAP_TEMPLE){if(py>=21){zone=MAP_VILLAGE;px=16;py=3;setMsg("RETURNED TO VILLAGE");return;}if(abs(px-16)+abs(py-9)<=2){startBattle(4);return;}}
 setMsg("NOTHING TO INTERACT WITH");}
static void worldAction(int k){if(k&KEY_START){selected=0;mode=MODE_PARTY;return;}if(k&KEY_SELECT){mode=MODE_QUEST;return;}if(k&KEY_X){saveResult=saveGame();setMsg(saveResult?"GAME SAVED":"SAVE UNAVAILABLE");return;}if(k&KEY_Y){setMsg(loadGame()?"GAME LOADED":"NO VALID SAVE");return;}if(k&KEY_A){interact();return;}
 int nx=px,ny=py;if(k&KEY_LEFT)nx--;else if(k&KEY_RIGHT)nx++;else if(k&KEY_UP)ny--;else if(k&KEY_DOWN)ny++;
 if(nx!=px||ny!=py){if(passable(nx,ny)){px=nx;py=ny;steps++;if(encounterLock)encounterLock--;if(zone==MAP_GROVE&&!encounterLock&&steps>4&&rand()%9==0)startBattle(rand()%4);}}}
static void keyAction(int k){if(mode==MODE_WORLD)worldAction(k);else if(mode==MODE_BATTLE)battleAction(k);else if(mode==MODE_DIALOG){if(k&KEY_A){if(dialogue==0){questFlags|=1;setMsg("GROVE QUEST ACCEPTED");}else if(dialogue==2){questFlags|=2;setMsg("CAPTURE QUEST ACCEPTED");}else{hp=120;chi=100;for(int i=0;i<partyCount;i++)partyHP[i]=min2(150,55+partyLevel[i]*5);setMsg("PARTY FULLY HEALED");}mode=MODE_WORLD;}else if(k&KEY_B)mode=MODE_WORLD;}
 else if(mode==MODE_PARTY){if(k&KEY_DOWN)selected=(selected+1)%partyCount;if(k&KEY_UP)selected=(selected+partyCount-1)%partyCount;if(k&KEY_A){active=selected;setMsg("LEADER CHANGED");mode=MODE_WORLD;}if(k&KEY_B||k&KEY_START)mode=MODE_WORLD;}
 else if(mode==MODE_QUEST){if(k&(KEY_B|KEY_A|KEY_SELECT))mode=MODE_WORLD;}
 else if(mode==MODE_FAINT){if(k&KEY_START){zone=MAP_VILLAGE;px=15;py=13;hp=120;chi=100;mode=MODE_WORLD;setMsg("RESTED IN VILLAGE");}}
}
static void touchAction(int x,int y){if(mode==MODE_WORLD){if(y>=50&&y<82){mode=x<128?MODE_PARTY:MODE_QUEST;selected=0;}else if(y>=88&&y<120){if(x<128)setMsg(saveGame()?"GAME SAVED":"SAVE UNAVAILABLE");else setMsg(loadGame()?"GAME LOADED":"NO VALID SAVE");}else if(y>=126&&y<158){zone=x<128?MAP_GROVE:MAP_VILLAGE;px=x<128?3:15;py=12;setMsg("TRAVEL COMPLETE");}}
 else if(mode==MODE_BATTLE){if(y>=25&&y<89)element=(y>=57?2:0)+(x>=128);else if(y>=95&&y<126)battleAction(x<128?KEY_A:KEY_X);else if(y>=129&&y<159)battleAction(x<128?KEY_Y:KEY_B);else if(y>=161)battleAction(x<128?KEY_L:KEY_R);}
 else if(mode==MODE_DIALOG){if(y>=87&&y<126)keyAction(KEY_A);else if(y>=137)keyAction(KEY_B);}
 else if(mode==MODE_PARTY){if(y>=45&&y<129){int idx=(y>=87?2:0)+(x>=128);if(idx<partyCount)selected=idx;}else if(y>=142){if(x<128)keyAction(KEY_A);else keyAction(KEY_B);}}
 else if(mode==MODE_QUEST){if(y>=139)mode=MODE_WORLD;}
 else if(mode==MODE_FAINT)keyAction(KEY_START);
}


int main(void) {
    consoleDemoInit();

    iprintf("ELEMENTAL LEGENDS DS\n\n");
    iprintf("BOOT TEST SUCCESSFUL\n\n");
    iprintf("Press A to continue\n");

    while (1) {
        scanKeys();

        if (keysDown() & KEY_A) {
            iprintf("\nA BUTTON WORKS!\n");
        }

        swiWaitForVBlank();
    }

    return 0;
}
