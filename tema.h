#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
#define lenmax 500
#define len_unit 3
#define len_description_sentinel 14
    typedef struct unit{
        int id;
        char type;
        int availability;
    }unit; 
    typedef struct incident{
        int id;
        char priority[7];
        char *description;
        char status[11];
    }incident;

    typedef struct intervention{
        struct incident *incident;
        struct unit *unit;
    }intervention;

    typedef struct Lincident{
    incident *info;        
    struct Lincident *prev, *next;
    }C_incident,*L_incidents;
    
    typedef struct Lintervention{
        incident *i;       
        unit *u;
        struct Lintervention *prev,*next;
    }C_intervention,*L_intervention;
    
    typedef struct LincidentQL{//queue that is low prio
    struct Lincident *incident;        
    struct LincidentQL *next;
    }C_incidentL,*L_incidentsL;

    typedef struct LincidentQM{//queue that is medium prio
    struct Lincident *incident;        
    struct LincidentQM *next;
    }C_incidentM,*L_incidentsM;
    
    typedef struct LincidentQH{//queue that is high prio
    struct Lincident *incident;        
    struct LincidentQH *next;
    }C_incidentH,*L_incidentsH;

    //struct for each type of queue//
    typedef struct QL
    { 
    L_incidentsL end;       
    } TQL;

    typedef struct QM
    { 
    L_incidentsM end;       
    } TQM;

    typedef struct QH
    { 
    L_incidentsH end;       
    } TQH;
    //struct for each type of queue  end//
    //history cell
   typedef struct stack_node {
    incident *i;               
    unit *u;                   
    struct stack_node *next;  
    } C_history, *L_history;
    
    typedef struct history_stack {
    L_history top;      //history top       
    } HistoryStack;
    //structs for the availability queue
    typedef struct unit_cell
    {
        unit *unit_info; 
        struct unit_cell *next;
    }unit_C,*unit_L;
    typedef struct unit_q
    {
        unit_L end;
    }unit_Q;
    //end of structs for availability queue

    unit_Q* Init_unit_Q();
    TQL* InitQL ();
    TQM* InitQM ();
    TQH* InitQH ();      
    L_incidents InitList_inc();
    L_intervention Init_intervention();
    HistoryStack* Init_history_stack();
    int check_command(char *command);
    int ADD_INCIDENT(L_incidents L, char *command, TQL *QL, TQM *QM, TQH *QH);
    int insert_unit(unit_Q *q,unit *u);
    int chk_u_a(unit_Q *q,FILE *f2);
    int show_unit(int num_units, unit *u, FILE *f2);
    int show_incident(int id, L_incidents L, FILE *f2);
    int dispatch(TQH *QH, TQM *QM, TQL *QL, unit_Q *UL, L_intervention LI, FILE *f2, HistoryStack *H);
    int undo_last_dispatch(L_intervention LI, TQH *high, TQM *medium, TQL *low,L_incidents L , HistoryStack *h, unit_Q *units, FILE *f2);
    void show_interventions(L_intervention L, FILE *f2);
    void solved_incident(L_intervention L, int id, unit_Q *U, FILE *f2);
    void cleanup(unit *u,unit_Q *unit_queue,L_incidents incidents,TQL *QL, TQM *QM, TQH *QH,L_intervention interventions,HistoryStack *history,char *command,FILE *f1, FILE *f2);