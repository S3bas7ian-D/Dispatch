#include "tema.h" 
#include <stdio.h>
	int main(void)
{
int num_units=0 ,num_commands = 0, id_command, num_available_units, id;//n of comands
FILE *f1 = NULL, *f2 = NULL;//input and output files
char *command = NULL, *units_info=NULL;//where command is saved
unit *u = NULL;
unit_Q *unit_queue=Init_unit_Q();
if(!unit_queue)
	return 1;
L_incidents List_of_incidents=NULL;
//initialising the queues start
HistoryStack *history = Init_history_stack();
if (!history) {
    // dai free la restul lucrurilor alocate anterior
    return 1;
}
TQL *QL = InitQL();
if(!QL)
	return 0;
TQM *QM = InitQM();
	if(!QM)
	{
		free(QL);
		return 1;
	}
TQH *QH = InitQH();
if(!QH)
	{
		free(QL);
		free(QM);
		return 1;
	}
//initialising the queues end
L_intervention L_of_interventions = Init_intervention();
	if(!L_of_interventions)
		return 0;
f1 = fopen("tema1.in", "r");
f2 = fopen("tema1.out","w");
if(!f1||!f2)
	return 1;//return if failed
if(fscanf(f1,"%d", &num_units)!=1&&num_units!=0)
	return 1;//return if failed
u = malloc(num_units*sizeof(unit));
if(!u)
	return 1;//return if failed
//here units are being registered
for(int i = 0; i<num_units;i++)
{
	fscanf(f1,"%d %c",&u[i].id,&u[i].type);
	fgetc(f1);
	u[i].availability = 1;
	if(!insert_unit(unit_queue, &u[i]))//check if failed
		return 1;
}
if(fscanf(f1,"%d", &num_commands)!=1)
		return 1;
fgetc(f1);
command = malloc(lenmax*sizeof(char));//500 characters
if(!command)
	{
		free(u);
		return 1;
	}
//sentinel is initialised
List_of_incidents=InitList_inc();
//command processing starts here
for(int i = 0; i < num_commands; i++)
{
fgets(command,lenmax,f1);
id_command = check_command(command);
switch(id_command)//for case guide check README or the "int check_command(char *command)" function
{
case 1:
	if(!ADD_INCIDENT(List_of_incidents,command, QL, QM ,QH))
		return 1;
	break;
case 2:	
	chk_u_a(unit_queue, f2);
	break;
case 3:
	dispatch(QH,QM,QL,unit_queue,L_of_interventions,f2,history);
	break;
case 4:
	undo_last_dispatch(L_of_interventions,QH,QM,QL,List_of_incidents,history,unit_queue,f2);
	break;
case 5:
	sscanf(command, "SOLVED_INCIDENT %d", &id);
	solved_incident(L_of_interventions, id, unit_queue ,f2 );
	break;
case 6:
    sscanf(command,"SHOW_UNIT %d",&id);
    {
        int found = -1;
        for(int j = 0; j < num_units; j++) {
            if(u[j].id == id) { found = j; break; }
        }
        if(found == -1)
            fprintf(f2, "INVALID OPERATION! ERROR 404\n");
        else
            show_unit(num_units, &u[found], f2);
    }
    break;
case 7:
	sscanf(command, "SHOW_INCIDENT %d", &id);
	show_incident(id,List_of_incidents,f2);
	break;
case 8:
	show_interventions(L_of_interventions, f2);
	break;

}
}
cleanup(u, unit_queue, List_of_incidents, QL, QM, QH,
        L_of_interventions, history, command, f1, f2);
return 0;    
}