/*****************************************************************************/
/* File:        robot.c (Khepera Simulator)                                  */
/* Author:      Olivier MICHEL <om@alto.unice.fr>                            */
/* Date:        Thu Sep 21 14:39:05 1995                                     */
/* Description: robot drivers                                                */
/*                                                                           */
/* Copyright (c) 1995                                                        */
/* Olivier MICHEL                                                            */
/* MAGE team, i3S laboratory,                                                */
/* CNRS, University of Nice - Sophia Antipolis, FRANCE                       */
/*                                                                           */
/* Permission is hereby granted to copy this package for free distribution.  */
/* The author's name and this copyright notice must be included in any copy. */
/* Commercial use is forbidden.                                              */
/*****************************************************************************/

#include "header.h"
#include "robot.h"
#include "graphics.h"
#include "sim.h"
#include "world.h"

extern long int          Rnd(int v);
extern boolean           StepRobot(struct Robot *r);
extern void              FastStepRobot(struct Robot *r);
extern void              WriteRobot(FILE *f,struct Robot *r);
extern void              SaveRobot(struct Robot *r,FILE *f);
extern void              ReadRobot(FILE *f,struct Robot *r);
extern void              LoadRobot(struct Robot *r,FILE *f);
extern void              DrawRobotEffectors(struct Robot *r);
extern void              DrawLittleRobot(struct Robot *sr,struct Robot *r);
extern void              DrawRobotIRSensors(struct Robot *r);
extern void              DisplayComment(struct Context *c,char *text);
extern boolean           khep_talk(int fd,char *send,char *receive,int rl);
extern int               serial_open(char *portname);
extern void              ResetRobot(struct Robot *r);

int khepera_fd;
char Serial_Port[TEXT_BUF_SIZE];

struct Robot *CreateRobot()
{
  short int    i,j;
  struct Robot *robot;

  robot = (struct Robot *)malloc(sizeof(struct Robot));
  strcpy(robot->Name,"default");
  robot->X          = 500;
  robot->Y          = 500;
  robot->Alpha      = M_PI/2.0;
  robot->State      = (u_char)0;

  robot->Motor[0].X     = 0.0;
  robot->Motor[0].Y     = 25.0;
  robot->Motor[0].Alpha = 0.0;
  robot->Motor[0].Value = 0;

  robot->Motor[1].X     = 0.0;
  robot->Motor[1].Y     = -25.0;
  robot->Motor[1].Alpha = 0.0;
  robot->Motor[1].Value = 0;

  robot->IRSensor[0].X     = 9.5;
  robot->IRSensor[0].Y     = 23.0;
  robot->IRSensor[0].Alpha = M_PI/2.0;
  robot->IRSensor[0].DistanceValue = 0;
  robot->IRSensor[0].LightValue    = 500;

  robot->IRSensor[1].X     = 18.0;
  robot->IRSensor[1].Y     = 17.5;
  robot->IRSensor[1].Alpha = M_PI/4.0;
  robot->IRSensor[1].DistanceValue = 0;
  robot->IRSensor[1].LightValue    = 500;

  robot->IRSensor[2].X     = 24.5;
  robot->IRSensor[2].Y     = 5.5;
  robot->IRSensor[2].Alpha = 0.0;
  robot->IRSensor[2].DistanceValue = 0;
  robot->IRSensor[2].LightValue    = 500;

  robot->IRSensor[3].X     = 24.5;
  robot->IRSensor[3].Y     = -5.5;
  robot->IRSensor[3].Alpha = 0.0;
  robot->IRSensor[3].DistanceValue = 0;
  robot->IRSensor[3].LightValue    = 500;

  robot->IRSensor[4].X     = 18.0;
  robot->IRSensor[4].Y     = -17.5;
  robot->IRSensor[4].Alpha = -M_PI/4.0;
  robot->IRSensor[4].DistanceValue = 0;
  robot->IRSensor[4].LightValue    = 500;

  robot->IRSensor[5].X     = 9.5;
  robot->IRSensor[5].Y     = -23.0;
  robot->IRSensor[5].Alpha = -M_PI/2.0;
  robot->IRSensor[5].DistanceValue = 0;
  robot->IRSensor[5].LightValue    = 500;

  robot->IRSensor[6].X     = -23.0;
  robot->IRSensor[6].Y     = -9.5;
  robot->IRSensor[6].Alpha = M_PI;
  robot->IRSensor[6].DistanceValue = 0;
  robot->IRSensor[6].LightValue    = 500;

  robot->IRSensor[7].X     = -23.0;
  robot->IRSensor[7].Y     = 9.5;
  robot->IRSensor[7].Alpha = M_PI;
  robot->IRSensor[7].DistanceValue = 0;
  robot->IRSensor[7].LightValue    = 500;

  return(robot);
}

void CreateDefaultRobot(struct Context *context)
{
  context->Robot = CreateRobot();
  context->Robot->State |= DISTANCE_SENSOR_FLAG | MOTOR_VALUES_FLAG;
}


void FreeRobot(struct Context *context)
{
  if (context->Robot)
  {
    free(context->Robot);
  }
}

void WriteRobot(FILE *ffile,struct Robot *robot)
{
  short int i;

  fprintf(ffile,"%lf,%lf,%lf,%lf,%hd\n",
          robot->X,robot->Y,robot->Alpha,robot->Diameter,robot->State);
  for(i=0;i<2;i++)
   fprintf(ffile,"%lf,%lf,%lf,%hd\n",
           robot->Motor[i].X,robot->Motor[i].Y,robot->Motor[i].Alpha,
           robot->Motor[i].Value);
  for(i=0;i<8;i++)
   fprintf(ffile,"%lf,%lf,%lf,%hd\n",
           robot->IRSensor[i].X,robot->IRSensor[i].Y,robot->IRSensor[i].Alpha,
           robot->IRSensor[i].DistanceValue);
}


void WriteRobotToFile(struct Robot *robot,FILE *ffile)
{
  WriteRobot(ffile,robot);
  SaveRobot(robot,ffile);
}

void ReadRobot(FILE *ffile,struct Robot *robot)
{
  short int i;

  fscanf(ffile,"%lf,%lf,%lf,%lf,%hd\n",
         &(robot->X),&(robot->Y),&(robot->Alpha),&(robot->Diameter),&i);
  robot->State = (unsigned short)i;
  for(i=0;i<2;i++)
   fscanf(ffile,"%lf,%lf,%lf,%hd\n",
          &(robot->Motor[i].X),&(robot->Motor[i].Y),&(robot->Motor[i].Alpha),
          &(robot->Motor[i].Value));
  for(i=0;i<8;i++)
   fscanf(ffile,"%lf,%lf,%lf,%hd\n",
          &(robot->IRSensor[i].X),&(robot->IRSensor[i].Y),
          &(robot->IRSensor[i].Alpha),&(robot->IRSensor[i].DistanceValue));
}

void ReadRobotFromFile(struct Robot *robot,FILE *ffile)
{
  ReadRobot(ffile,robot);
  LoadRobot(robot,ffile);
}

u_short IRSensorDistanceValue(struct World *world,short int x,short int y,
                              double alpha)
{
  struct Object    *objects;
  u_short          value,b[3]={0,0,0},i,j,k;
  double           cosa,sina;
  static short int sx[15]={-2,-4,-5,-7,-9,0, 0, 0, 0, 0, 2,4,5,7,9},
                   sy[15]={ 2, 4,8, 13, 20,6,10,16,22,29,2,4,8,13,20},
                    v[15]={1023,800,600,400,60,900,750,650,160,40,
                           1023,800,600,400,60},
                   ex,ey;

  cosa = cos(alpha);
  sina = sin(alpha);
  x /= 2;
  y /= 2;

  for(i=0;i<3;i++)
  {
    for(j=0;j<5;j++)
    {
      k = i*5+j;
      ex = x + (short int)(sy[k] * cosa + sx[k] * sina);
      ey = y + (short int)(sx[k] * cosa - sy[k] * sina);
      /* XDrawPoint(display,window,gc,ex+WORLD_OFFSET_X,ey+WORLD_OFFSET_Y); */
      if (world->Image[ex/32][ey] & (1 << (ex%32)))
      {
        b[i]=v[k];
        break;
      }
    }
  }
  value = b[0]+b[1]+b[2];
  if (value < 7)         value = Rnd(7);
  else if (value > 1022) value = 1023;
  else
  {
    value = (u_short)(value * (0.90 + (double)(Rnd(2000))/10000.0));
    if (value > 1023) value = 1023;
  }
  return(value);
}

u_short IRSensorLightValue(struct World *world,short int x,short int y,
                           double alpha)
{
  struct Object *obj;
  double angle,d,dx,dy;
  int    value = 500; /* corresponding to the dark */

  obj = world->Objects;
  while((obj->Type == LAMP_ON)||(obj->Type == LAMP_OFF))
  {
    if (obj->Type == LAMP_ON)
    {
      dx = obj->X - x;
      dy = obj->Y - y;
      d = (250 - sqrt(dx*dx + dy*dy))/250.0;
      if (d>0)
      {
        angle = atan2(-dy,dx) - alpha;
        NormRad(angle);
        if ((angle > -M_PI/3)&&(angle < M_PI/3))
        {
          value -= (int)(cos(angle * 1.5)*d*450); /* 50 = maximal lightning */
        }
      }
    }
    obj = obj->Next;
  }
  if (value < 50) value = 50;
  value -= (value*(Rnd(200)-100))/2000; /* noise = +-5% */
  return((u_short)value);
}

void InitSensors(struct Context *context)
{
  short int i,xc,yc;
  double x,y;
  double alpha,alpha0;
  struct Robot *robot;
  char text[256];

  robot = context->Robot;

  alpha0 = robot->Alpha;
  for(i=0;i<8;i++)
  {
    x    = robot->IRSensor[i].X;
    y    = robot->IRSensor[i].Y;

    xc    = (short int)(robot->X - y*sin(alpha0) + x*cos(alpha0));
    yc    = (short int)(robot->Y - y*cos(alpha0) - x*sin(alpha0));
    alpha = alpha0 + robot->IRSensor[i].Alpha;
    NormRad(alpha);
    robot->IRSensor[i].DistanceValue = 
                            IRSensorDistanceValue(context->World,xc,yc,alpha);
    robot->IRSensor[i].LightValue = 
                            IRSensorLightValue(context->World,xc,yc,alpha);
  }
}

void SolveEffectors(struct Context *context)
{
  struct Robot *robot;
  double       delta_direction,noise1,noise2,noise3;

  robot = context->Robot;

  noise1 = 0.9 + (double)Rnd(200)/1000.0;
  noise2 = 0.9 + (double)Rnd(200)/1000.0;
  noise3 = 0.95 + (double)Rnd(100)/1000.0;
  delta_direction = (double)(robot->Motor[0].Value - robot->Motor[1].Value);
  delta_direction /= 200;
  delta_direction *= noise3;
  robot->Alpha += delta_direction;
  NormRad(robot->Alpha);
  robot->X+=(double)(robot->Motor[0].Value + robot->Motor[1].Value)*noise1
                     *cos(robot->Alpha)/4;
  robot->Y-=(double)(robot->Motor[0].Value + robot->Motor[1].Value)*noise2
                     *sin(robot->Alpha)/4;
  robot->Alpha += delta_direction;
  NormRad(robot->Alpha);
}

boolean TestCollision(struct Context *context,double ox,double oy,
                      double oalpha)
{
  u_char       collision = FALSE;
  static short int offset[16]={-15,0,-11,-11,0,-15,11,-11,15,0,11,11,0,15,
                               -11,11};
  short int     i,xw,yw,x,y,front,back;
  struct World *world;
  struct Robot *robot;

  world = context->World;
  robot = context->Robot;

  x  = (short int)(robot->X/ROBOT_SCALE);
  y  = (short int)(robot->Y/ROBOT_SCALE);
  front = (short int)(0.5 + (4*robot->Alpha) / M_PI);
  while (front < 0) front += 8;
  while (front > 7) front -= 8;
  front *= 2;
  back = front + 8;
  while (back > 15) back -= 16;
  front += 2;
  back  += 2;
  for(i=0;i<16;)
  {
    xw = x + offset[i++];
    yw = y + offset[i++];
    if (world->Image[xw/32][yw] & (1 << (xw%32)))
    {
      collision = TRUE;
      if ((i==front)||(i==back))  /* when the robot hits an obstacle on the */
      {                           /* front or on the back, it can't slide   */
        robot->X = ox;            /* and won't turn                         */
        robot->Y = oy;
        robot->Alpha = oalpha;
      }
      else                        /* else, the robot will slightly slide on */
      {                           /* the obstacle and turn                  */
        robot->X     = ox - offset[i-2]/11;
        robot->Y     = oy - offset[i-1]/11;
      }
      i = 16;
    }
  }
  return(collision);
}

boolean RobotRun(struct Context *context)
{
  struct Robot   *robot,saved_robot;
  boolean        ans = FALSE;

  robot = context->Robot;

  saved_robot.X         = robot->X;
  saved_robot.Y         = robot->Y;
  saved_robot.Alpha     = robot->Alpha;

  InitSensors(context);
  DrawRobotIRSensors(robot);
  ans = StepRobot(robot);
  SolveEffectors(context);
  DrawRobotEffectors(robot);
  if (TestCollision(context,saved_robot.X,saved_robot.Y,saved_robot.Alpha))
  {
    DisplayComment(context,"(Bump)");
    robot->State |= BUMP;
  }
  else robot->State &= ~BUMP;
  DrawLittleRobot(&saved_robot,robot);
  return(ans);
}

void RobotRunFast(struct Context *context)
{
  struct Robot   *robot,saved_robot;
  char           text[256];
  boolean        ans;

  robot = context->Robot;

  saved_robot.X         = robot->X;
  saved_robot.Y         = robot->Y;
  saved_robot.Alpha     = robot->Alpha;

  InitSensors(context);
  FastStepRobot(robot);
  SolveEffectors(context);
  if (TestCollision(context,saved_robot.X,saved_robot.Y,saved_robot.Alpha))
  robot->State |= BUMP; else robot->State &= ~BUMP;
}

boolean PipedRobotRun(struct Context *context,int serial_in,int serial_out)
{
  struct Robot   *robot,saved_robot;
  boolean        ans = TRUE;
  char           q[64],a[64];
  int            lm,rm;
  static int     i=0;

  robot = context->Robot;

  saved_robot.X         = robot->X;
  saved_robot.Y         = robot->Y;
  saved_robot.Alpha     = robot->Alpha;

  InitSensors(context);
  DrawRobotIRSensors(robot);

  a[0] = '\0';
  if (read(serial_in,&q[0],1)!=0)
  {
    switch (q[0])
    {
    case 'A': fprintf(stderr,
               "Warning: serial command A (Configure) not implemented");
              strcpy(a,"a\n");
              break;
    case 'B': fprintf(stderr,
          "Warning: serial command B (Read Software version) not implemented");
              strcpy(a,"b,0\n");
              break;
    case 'D': i=0;
              do
              {
                i++;
                read(serial_in,&q[i],1);
              }
              while (q[i]!='\n');
              if (sscanf(q,"D,%d,%d",&lm,&rm)==2)
              {
                robot->Motor[1].Value=lm;
                robot->Motor[0].Value=rm;
              }
              strcpy(a,"d\n");
              break;
    case 'E': sprintf(a,"e,%d,%d\n",robot->Motor[1].Value,
                         robot->Motor[0].Value);
              break;
    case 'G': fprintf(stderr,
               "Warning: serial command G (Set position) not implemented");
              strcpy(a,"g\n");
              break;
    case 'H': fprintf(stderr,
               "Warning: serial command H (Read position) not implemented");
              strcpy(a,"h,0,0\n");
              break;
    case 'N': sprintf(a,"n,%d,%d,%d,%d,%d,%d,%d,%d\n",
              robot->IRSensor[0].DistanceValue,
              robot->IRSensor[1].DistanceValue,
              robot->IRSensor[2].DistanceValue,
              robot->IRSensor[3].DistanceValue,
              robot->IRSensor[4].DistanceValue,
              robot->IRSensor[5].DistanceValue,
              robot->IRSensor[6].DistanceValue,
              robot->IRSensor[7].DistanceValue);
              break;
    case 'O': sprintf(a,"o,%d,%d,%d,%d,%d,%d,%d,%d\n",
              robot->IRSensor[0].LightValue,
              robot->IRSensor[1].LightValue,
              robot->IRSensor[2].LightValue,
              robot->IRSensor[3].LightValue,
              robot->IRSensor[4].LightValue,
              robot->IRSensor[5].LightValue,
              robot->IRSensor[6].LightValue,
              robot->IRSensor[7].LightValue);
              break;
    }
    if (a[0]!='\0') write(serial_out,a,strlen(a));
  }
  SolveEffectors(context);
  DrawRobotEffectors(robot);
  if (TestCollision(context,saved_robot.X,saved_robot.Y,saved_robot.Alpha))
  {
    DisplayComment(context,"(Bump)");
    robot->State |= BUMP;
  }
  else robot->State &= ~BUMP;
  DrawLittleRobot(&saved_robot,robot);
  return(ans);
}

void InitKheperaSerial(char *name)
{
  strcpy(Serial_Port,name);
}

void InitKheperaSensors(struct Context *context)
{
  struct Robot *robot;
  char message[TEXT_BUF_SIZE],answer[TEXT_BUF_SIZE];

  robot = context->Robot;
  if (khep_talk(khepera_fd,"N\n",answer,TEXT_BUF_SIZE-1))
  {
    sscanf(answer,"n,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu",
                 &robot->IRSensor[0].DistanceValue,
                 &robot->IRSensor[1].DistanceValue,
                 &robot->IRSensor[2].DistanceValue,
                 &robot->IRSensor[3].DistanceValue,
                 &robot->IRSensor[4].DistanceValue,
                 &robot->IRSensor[5].DistanceValue,
                 &robot->IRSensor[6].DistanceValue,
                 &robot->IRSensor[7].DistanceValue);
  } else DisplayComment(context,"serial communication error");

  if (khep_talk(khepera_fd,"O\n",answer,TEXT_BUF_SIZE-1))
  {
    sscanf(answer,"o,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu",
                 &robot->IRSensor[0].LightValue,
                 &robot->IRSensor[1].LightValue,
                 &robot->IRSensor[2].LightValue,
                 &robot->IRSensor[3].LightValue,
                 &robot->IRSensor[4].LightValue,
                 &robot->IRSensor[5].LightValue,
                 &robot->IRSensor[6].LightValue,
                 &robot->IRSensor[7].LightValue);
  } else DisplayComment(context,"serial communication error");
}

void SolveKheperaEffectors(struct Context *context)
{
  struct Robot *robot;
  char message[TEXT_BUF_SIZE],answer[TEXT_BUF_SIZE];

  robot = context->Robot;
  if (robot->Motor[0].Value >  10) robot->Motor[0].Value =  10;
  if (robot->Motor[0].Value < -10) robot->Motor[0].Value = -10;
  if (robot->Motor[1].Value >  10) robot->Motor[1].Value =  10;
  if (robot->Motor[1].Value < -10) robot->Motor[1].Value = -10;
  sprintf(message,"D,%d,%d\n",robot->Motor[1].Value,robot->Motor[0].Value);
  if (khep_talk(khepera_fd,message,answer,TEXT_BUF_SIZE-1)==FALSE)
   DisplayComment(context,"serial communication error");
}

void OpenKheperaSerial(struct Context *context)
{
  char answer[TEXT_BUF_SIZE];

  khepera_fd = serial_open(Serial_Port);
  if (khepera_fd > 0)
  {
    context->Robot->State |= REAL_ROBOT_FLAG;
    if (khep_talk(khepera_fd,"A,3800,800,100\n",answer,TEXT_BUF_SIZE-1))
     DisplayComment(context,"Ready to run Khepera robot");
    else DisplayComment(context,"serial communication error");
    InitKheperaSensors(context);
    DrawRobotIRSensors(context->Robot);
  }
  else
  {
    context->Robot->State &= ~REAL_ROBOT_FLAG;
    DisplayComment(context,"unable to open serial port");
  }
}

void CloseKheperaSerial(struct Context *context)
{              
  close(khepera_fd);
  context->Robot->State &= ~REAL_ROBOT_FLAG;
  DisplayComment(context,"back to simulated robot");
  InitSensors(context);
  DrawRobotIRSensors(context->Robot);
}

boolean RunKhepera(struct Context *context)
{
  struct Robot *robot;
  boolean ans;

  robot = context->Robot;
  InitKheperaSensors(context);
  DrawRobotIRSensors(robot);
  ans = StepRobot(robot);
  SolveKheperaEffectors(context);
  DrawRobotEffectors(robot);
  return(ans);
}

void InitRobot(struct Context *context)
{
  short int i,j;
  struct Robot *robot;

  robot = context->Robot;
  robot->Motor[0].Value = 0;
  robot->Motor[1].Value = 0;
  if (robot->State & REAL_ROBOT_FLAG) InitKheperaSensors(context);
   else InitSensors(context);
  ResetRobot(robot);
}
