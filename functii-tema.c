#include "tema.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
//function to identify the command
int check_command(char *command)
{
  if (strstr(command,"ADD_INCIDENT"))
    return 1;
  if (strstr(command,"CHECK_UNITS_AVAILABILITY"))
    return 2;
  if (strstr(command,"UNDO_LAST_DISPATCH"))  //must come before DISPATCH
    return 4;
  if (strstr(command,"DISPATCH"))
    return 3;
  if (strstr(command,"SOLVED_INCIDENT"))
    return 5;
  if (strstr(command,"SHOW_UNIT"))
    return 6;
  if (strstr(command,"SHOW_INCIDENT"))
    return 7;
  if (strstr(command,"SHOW_INTERVENTIONS"))
    return 8;
  return 0;
}
//function to initialize a list(sentinel)
L_incidents InitList_inc() {
    C_incident *aux=NULL;
    //use aux
    aux = (C_incident*) malloc(sizeof(C_incident));
    if (!aux) {
        return NULL;
    }
    //malloc for info
    aux->info = malloc(sizeof(incident));                  
    if(!aux->info)
      {
        free(aux);
         return NULL;
      }
      aux->info->description = NULL;
      //malloc for description
      aux->info->description = malloc(len_description_sentinel*sizeof(char));
      if(!aux->info->description)
      {
        free(aux->info);
        free(aux);
        return NULL;
      }
      strcpy(aux->info->description,"test incident");
      aux->info->id = 0; 
    strcpy(aux->info->priority,"low");
    strcpy(aux->info->status,"solved");
    aux->prev = aux;
    aux->next = aux;
    return aux;
}
//start of functions to initialise the low, medium, high queues  
TQL *InitQL ()  
{ 
  TQL* c;          
  c = (TQL*)malloc(sizeof(TQL));
  if ( ! c ) return NULL;                  

  c->end = NULL;
  return c;         
}
TQM *InitQM ()  
{ 
  TQM* c;          
  c = (TQM*)malloc(sizeof(TQM));
  if ( ! c ) return NULL;                  

  c->end = NULL;
  return c;         
}
TQH *InitQH ()  
{ 
  TQH* c;          
  c = (TQH*)malloc(sizeof(TQH));
  if ( ! c ) return NULL;                  

  c->end = NULL;
  return c;         
}
//end of the functions to initialise the low, medium, high queues

//function to add an incident to the circular list and to the queues
int ADD_INCIDENT(L_incidents L, char *command,TQL *QL, TQM *QM, TQH *QH)
{
  int length_description;
  //finding the length based on the "" characters
  length_description = strchr(strchr(command,'"')+1,'"')-strchr(command,'"');
  L_incidents aux=NULL;
  aux=(C_incident*)malloc(sizeof(C_incident));
  if(!aux)
    return 1;
  aux->info = malloc(sizeof(incident));                  
    if(!aux->info)
      {
        free(aux);
         return 0;
      }
  aux->info->description = NULL;
  aux->info->description = malloc(length_description*sizeof(char));
      if(!aux->info->description)
      {
        free(aux->info);
        free(aux);
        return 0;
      }
  sscanf(command,"ADD_INCIDENT %d %s \"%[^\"]\"",&aux->info->id,aux->info->priority,aux->info->description);
  strcpy(aux->info->status,"queued");
  //cell insert
  aux->prev = L->prev;
  aux->next = L;
  L->prev->next = aux;
  L->prev = aux;
  
  //low prio block start//
  if (strcmp(aux->info->priority,"low")==0)
      {
        L_incidentsL aux_q = NULL;
        aux_q = (L_incidentsL)malloc(sizeof(C_incidentL));
        if(!aux_q)
          return 0; 
        aux_q->incident=aux;
        if(QL->end!=NULL)
          {
            aux_q->next = QL->end->next;
            QL->end->next = aux_q; 
            QL->end = aux_q;
          }
        else 
        {
          QL->end = aux_q;
          aux_q->next = aux_q;
        } 
      }
  //low prio block end//
  
  //medium prio block start//
  if (strcmp(aux->info->priority,"medium") == 0)
      {
        L_incidentsM aux_q=NULL;
        aux_q = (L_incidentsM)malloc(sizeof(C_incidentM));
        if(!aux_q)
          return 0; 
        aux_q->incident = aux;
        if(QM->end!=NULL)
          {
            aux_q->next = QM->end->next;
            QM->end->next = aux_q; 
            QM->end = aux_q;
          }
        else 
        {
          QM->end = aux_q;
          aux_q->next = aux_q;
        } 
      }
  //medium prio block end//
  
  //high prio block start//
  if (strcmp(aux->info->priority,"high")==0)
      {
        L_incidentsH aux_q=NULL;
        aux_q = (L_incidentsH)malloc(sizeof(C_incidentH));
        if(!aux_q)
          return 0; 
        aux_q->incident = aux;
        if(QH->end!=NULL)
          {
            aux_q->next = QH->end->next;
            QH->end->next = aux_q; 
            QH->end = aux_q;
          }
        else 
        {
          QH->end = aux_q;
          aux_q->next = aux_q;
        }

      }
  //high prio block end//
      return 1;
}

//function to create the unit availability queue
unit_Q* Init_unit_Q()
{
  unit_Q *c;
  c=(unit_Q*)malloc(sizeof(unit_Q));
  if (!c)
    return NULL;
  c->end = NULL;
  return c;
}

//function to insert a unit into the queue;
int insert_unit(unit_Q *q,unit *u)
{
  unit_L aux = NULL;
  aux = (unit_L)malloc(sizeof(unit_C));
  if(!aux)
    return 0;
  aux->unit_info=u;
  if(q->end!=NULL)
  {
    aux->next = q->end->next;
    q->end->next=aux;
    q->end=aux;
  }
  else 
  {
  q->end=aux;
  aux->next=aux;
  }
 return 1;
}
//function to check the queue size
int chk_u_a(unit_Q *q,FILE *f2)
{
  int cnt = 0;
  if(!q->end)//check if it is empty
  {
    fprintf(f2,"Number of available units: %d\n",cnt);
    return 1;
  }
  
  unit_L aux=q->end->next;
  do
  {
    cnt++;
    aux = aux->next;
  } while (aux != q->end->next);
  fprintf(f2,"Number of available units: %d\n",cnt);
  return 0;
}
//function to show unit status
int show_unit(int num_units, unit *u, FILE *f2)
{
    char s[15];
    if(u->availability == 1)
        strcpy(s, "available");
    else
        strcpy(s, "unavailable");
    fprintf(f2, "Unit %d is type %c and is %s\n", u->id, u->type, s);
    return 0;
}
//function to show incident status
int show_incident(int id, L_incidents L, FILE *f2)
{
  if(L->next==L)
    {
      fprintf(f2,"INVALID OPERATION! ERROR 404\n");
      return 0;
    }
  L_incidents aux= L->next;
  do
  {
    if(aux->info->id==id)
    {
      break;
    }
    aux=aux->next;
  }while(aux->info->id!=0);
  if(aux->info->id==0)
    {
      fprintf(f2,"INVALID OPERATION! ERROR 404\n");
      return 0;
    }
  else
    {
      fprintf(f2,"Incident %d has %s priority, the following description: \"%s\" and is %s\n",aux->info->id, aux->info->priority, aux->info->description, aux->info->status);
      return 1;
    }
  return 0;
}
//function to initialize the intervention list
L_intervention Init_intervention()
{
  C_intervention *c;
  c=(C_intervention*)malloc(sizeof(C_intervention));
  if (!c)
    return NULL;
  c->i=NULL;
  c->u=NULL;
  c->next = c;
  c->prev = c;
  return c;
}
//function to initialize the intervention list
HistoryStack* Init_history_stack() {
    HistoryStack *stack = (HistoryStack*)malloc(sizeof(HistoryStack));
    if (!stack) {
        return NULL; 
    }
    stack->top = NULL; 
    return stack;
}

int dispatch(TQH *QH, TQM *QM, TQL *QL, unit_Q *UL, L_intervention LI, FILE *f2, HistoryStack *H)
{
  L_incidentsH aux_h, aux_h_prev;
  L_incidentsM aux_m, aux_m_prev;
  L_incidentsL aux_l, aux_l_prev;
  if(!UL->end)//check if it is empty or nor
  {
    //list is empty
    fprintf(f2,"INVALID OPERATION! ERROR 404\n");
    return 1;
  }
  unit_L aux_units=UL->end->next;
  unit_L aux_units_prev=UL->end;
  do {
      if(aux_units->unit_info->availability == 1)
        break;
    aux_units_prev=aux_units;
    aux_units = aux_units->next;
  
  }while (aux_units != UL->end->next);
  
  if(aux_units->unit_info->availability!=1)
    {
      fprintf(f2,"INVALID OPERATION! ERROR 404\n");
      return 1;
    }
  //branch for high prio
    if(QH->end)
  {
  aux_h=QH->end->next;
  aux_h_prev=QH->end;
  do {
    
    if(strcmp(aux_h->incident->info->status, "queued") == 0)
      break;
    aux_h_prev=aux_h;
    aux_h = aux_h->next;
  
  }while (aux_h != QH->end->next);
  
  if(strcmp(aux_h->incident->info->status,"queued")==0)
    {
      aux_units->unit_info->availability = 0;
      strcpy(aux_h->incident->info->status,"intervened");
      L_intervention new_intervention = (C_intervention*)malloc(sizeof(C_intervention));
      if(!new_intervention)
        return 1;//failed malloc
      //we insert the info
      new_intervention->i = aux_h->incident->info;
      new_intervention->u = aux_units->unit_info;
      //we place the new cell into the big list
      new_intervention->prev = LI->prev;
      new_intervention->next = LI;
      LI->prev->next = new_intervention;
      LI->prev = new_intervention;
      //we create the new stack history cell
      L_history new_history = (C_history*)malloc(sizeof(C_history));
        if (!new_history) return 1;
      //insert info into the history cell
      new_history->i = aux_h->incident->info;
      new_history->u = aux_units->unit_info;
      //push the cell into the stack
      new_history->next = H->top;
      H->top = new_history;
      //extract from queue
      if (aux_h == aux_h->next) {//if it was the last element
          QH->end = NULL;
      } else {
        //if we extract smthn else  
        aux_h_prev->next = aux_h->next; 
          if (aux_h == QH->end) {
              QH->end = aux_h_prev;
          }
      }
       free(aux_h);
      if(aux_units == aux_units->next)
      {
        UL->end = NULL;
      }
      else {
      aux_units_prev->next = aux_units->next;
      if(aux_units == UL->end)
      {
        UL->end=aux_units_prev;
      }
      }
     free(aux_units);
      return 0;
    }    
  }
  //end of branch for high prio

  //start of branch mid prio
    if(QM->end)
  {
    aux_m_prev = QM->end;
    aux_m = QM->end->next;
  
 do {
    if(strcmp(aux_m->incident->info->status, "queued") == 0)
      break;
    aux_m_prev=aux_m;
    aux_m = aux_m->next;
  
  }while (aux_m != QM->end->next);
  if(strcmp(aux_m->incident->info->status,"queued")==0)
    {
      aux_units->unit_info->availability = 0;
      strcpy(aux_m->incident->info->status,"intervened");
      L_intervention new_intervention = (C_intervention*)malloc(sizeof(C_intervention));
      if(!new_intervention)
        return 1;//failed malloc
      //we insert the info
      new_intervention->i = aux_m->incident->info;
      new_intervention->u = aux_units->unit_info;
      //we place the new cell into the big list
      new_intervention->prev = LI->prev;
      new_intervention->next = LI;
      LI->prev->next = new_intervention;
      LI->prev = new_intervention;
      //we create the new stack history cell
      L_history new_history = (C_history*)malloc(sizeof(C_history));
        if (!new_history) return 1;
      //insert info into the history cell
      new_history->i = aux_m->incident->info;
      new_history->u = aux_units->unit_info;
      //push the cell into the stack
      new_history->next = H->top;
      H->top = new_history;
      //extract from queue
      if (aux_m == aux_m->next) {//if it was the last element
          QM->end = NULL;
      } else {
        //if we extract smthn else  
        aux_m_prev->next = aux_m->next; 
          if (aux_m == QM->end) {
              QM->end = aux_m_prev;
          }
      }
      free(aux_m);
      if(aux_units == aux_units->next)
      {
        UL->end = NULL;
      }
      else {
      aux_units_prev->next = aux_units->next;
      if(aux_units == UL->end)
      {
        UL->end=aux_units_prev;
      }
      }
     free(aux_units);
      return 0;
    }    
  }
  //end of branch mid prio
  
  //branch for low prio
  if(QL->end)
  {
    aux_l_prev = QL->end;
    aux_l = QL->end->next;
  
  do {
    if(strcmp(aux_l->incident->info->status, "queued") == 0)
      break;
    aux_l_prev=aux_l;
    aux_l = aux_l->next;
  
  }while (aux_l != QL->end->next);
  if(strcmp(aux_l->incident->info->status,"queued")==0)
    {
      aux_units->unit_info->availability = 0;
      strcpy(aux_l->incident->info->status,"intervened");
      L_intervention new_intervention = (C_intervention*)malloc(sizeof(C_intervention));
      if(!new_intervention)
        return 1;//failed malloc
      //we insert the info
      new_intervention->i = aux_l->incident->info;
      new_intervention->u = aux_units->unit_info;
      //we place the new cell into the big list
      new_intervention->prev = LI->prev;
      new_intervention->next = LI;
      LI->prev->next = new_intervention;
      LI->prev = new_intervention;
      //we create the new stack history cell
      L_history new_history = (C_history*)malloc(sizeof(C_history));
        if (!new_history) return 1;
      //insert info into the history cell
      new_history->i = aux_l->incident->info;
      new_history->u = aux_units->unit_info;
      //push the cell into the stack
      new_history->next = H->top;
      H->top = new_history;
      //extract from queue
      if (aux_l == aux_l->next) {//if it was the last element
          QL->end = NULL;
      } else {
        //if we extract smthn else  
        aux_l_prev->next = aux_l->next; 
          if (aux_l == QL->end) {
              QL->end = aux_l_prev;
          }
      }
      free(aux_l);
      //free the intervention
      if(aux_units == aux_units->next)
      {
        UL->end = NULL;
      }
      else {
      aux_units_prev->next = aux_units->next;
      if(aux_units == UL->end)
      {
        UL->end=aux_units_prev;
      }
      }
     free(aux_units);
      return 0;
    }    
  }
  //end of low prio branch
    fprintf(f2,"INVALID OPERATION! ERROR 404\n");

  return 1;
}
//function to show interventions
void show_interventions(L_intervention L, FILE *f2)
{
  if(L->next == L)
    {
      fprintf(f2,"No intervention has been initiated\n");
      return;
    }
  L_intervention aux = L->next;
  while(aux != L)
  {
    fprintf(f2,"Incident %d was assigned to unit %d, and has the following status: \"%s\"\n",aux->i->id,aux->u->id,aux->i->status);
    aux = aux->next;

  }
}

//function to mark an incident as solved 
void solved_incident(L_intervention L, int id, unit_Q *U, FILE *f2)
{
  if(L->next == L)//if it is empty
    {
      fprintf(f2,"INVALID OPERATION! ERROR 404\n");
      return;
    }
  L_intervention aux = L->next;
  short found = 0;
  while(aux != L && found == 0)
  {
    if(aux->i->id == id)
    {
      found = 1;
      //check for id match
      if(strcmp(aux->i->status,"intervened") == 0)
        {
          strcpy(aux->i->status,"solved");
          aux->u->availability = 1;
          insert_unit(U, aux->u);
        }
      else
        {
          fprintf(f2,"INVALID OPERATION! ERROR 404\n");
          return;
        }
  
      }
    aux = aux->next;
  }
  if(!found)
    fprintf(f2, "INVALID OPERATION! ERROR 404\n");

}
//undo function
int undo_last_dispatch(L_intervention LI, TQH *high, TQM *medium, TQL *low,L_incidents L , HistoryStack *h, unit_Q *units, FILE *f2)
{
  if(!h->top)
    {
      fprintf(f2,"INVALID OPERATION! ERROR 404\n");
      return 0;
    }  
  //we go throgh the stack, removing cells that contain solved interventions
    while(h->top)
  {
    if(strcmp(h->top->i->status,"solved") == 0)
      {
        L_history cell_to_delete = h->top;
        h->top=h->top->next;
        free(cell_to_delete);
      }
    else {
      break;
    }

  }  
  if(!h->top)
  {
    fprintf(f2, "INVALID OPERATION! ERROR 404\n");
      return 0;
  }
  //we change the status for the entities
  strcpy(h->top->i->status,"queued");
  h->top->u->availability = 1;
  //we search into the intervention list
  L_intervention aux_intervention = LI->next;
  while(aux_intervention != LI) {
        if (aux_intervention->i->id == h->top->i->id) {
            aux_intervention->prev->next = aux_intervention->next;
            aux_intervention->next->prev = aux_intervention->prev;
            free(aux_intervention);
            break;
        }
        aux_intervention = aux_intervention->next;
    }
  L_incidents aux = L ->next;
  //we search for the specific cell into the list of incidents so we can insert it into the prio queues
  while(aux != L){
    if(aux->info->id == h->top->i->id)
      break;
    aux = aux->next;
  }
  if(aux == L) {
    fprintf(f2, "INVALID OPERATION! ERROR 404\n");
    return 1;
}
  //prio queue high insertion
  if (strcmp(h->top->i->priority, "high") == 0) 
    {
        L_incidentsH new_node = (L_incidentsH)malloc(sizeof(C_incidentH));
        if(!new_node)
          return 1;
        new_node->incident = aux;
        if (high->end == NULL) {
            high->end = new_node;
            new_node->next = new_node;
        } else {
            new_node->next = high->end->next;
            high->end->next = new_node;
        }
    }
  //prio queue medium insertion
  else if (strcmp(h->top->i->priority, "medium") == 0) 
    {
        L_incidentsM new_node = (L_incidentsM)malloc(sizeof(C_incidentM));
        if(!new_node)
          return 1;
        new_node->incident = aux;
        if (medium->end == NULL) {
            medium->end = new_node;
            new_node->next = new_node;
        } else {
            new_node->next = medium->end->next;
            medium->end->next = new_node;
        }
    }
  //prio queue low insertion
  else if (strcmp(h->top->i->priority, "low") == 0) {
        L_incidentsL new_node = (L_incidentsL)malloc(sizeof(C_incidentL));
        if(!new_node)
          return 1;
        new_node->incident = aux;
        if (low->end == NULL) {
            low->end = new_node;
            new_node->next = new_node;
        } else {
            new_node->next = low->end->next;
            low->end->next = new_node;
        }
    }
    //unit is inserted back
    insert_unit(units, h->top->u);
    //stack cell gets popped
    L_history cancelled = h->top;
    h->top = h->top->next;
    free(cancelled);
    
    return 0;
} 
//garbage collector
void cleanup(unit *u,unit_Q *unit_queue,L_incidents incidents,TQL *QL, TQM *QM, TQH *QH,L_intervention interventions,HistoryStack *history,char *command,FILE *f1, FILE *f2)
{
    // free the unit array
    if (u)
        free(u);
    // free the unit availability queue (circular singly linked)
    if (unit_queue) {
        if (unit_queue->end) {
            unit_L start = unit_queue->end->next;
            unit_L curr = start;
            do {
                unit_L next = curr->next;
                free(curr);
                curr = next;
            } while (curr != start);
        }
        free(unit_queue);
    }
    // free the incidents list 
    // each node has an incident*, hence we need to start the free from the deepest point of the structure
    if (incidents) {
        L_incidents curr = incidents->next;
        while (curr != incidents) {
            L_incidents next = curr->next;
            if (curr->info) {
                if (curr->info->description)
                    free(curr->info->description);
                free(curr->info);
            }
            free(curr);
            curr = next;
        }
        // free the sentinel last
        if (incidents->info) {
            if (incidents->info->description)
                free(incidents->info->description);
            free(incidents->info);
        }
        free(incidents);
    }
    // free the priority queueues
    if (QL) {
        if (QL->end) {
            L_incidentsL start = QL->end->next;
            L_incidentsL curr = start;
            do {
                L_incidentsL next = curr->next;
                free(curr);
                curr = next;
            } while (curr != start);
        }
        free(QL);
    }

    if (QM) {
        if (QM->end) {
            L_incidentsM start = QM->end->next;
            L_incidentsM curr = start;
            do {
                L_incidentsM next = curr->next;
                free(curr);
                curr = next;
            } while (curr != start);
        }
        free(QM);
    }

    if (QH) {
        if (QH->end) {
            L_incidentsH start = QH->end->next;
            L_incidentsH curr = start;
            do {
                L_incidentsH next = curr->next;
                free(curr);
                curr = next;
            } while (curr != start);
        }
        free(QH);
    }
    // free the interventions list (circular doubly linked)
       
    if (interventions) {
        L_intervention curr = interventions->next;
        while (curr != interventions) {
            L_intervention next = curr->next;
            free(curr);
            curr = next;
        }
        free(interventions); // free sentinel
    }

    // free the history stack
    if (history) {
        L_history curr = history->top;
        while (curr) {
            L_history next = curr->next;
            free(curr);
            curr = next;
        }
        free(history);
    }

    if (command)
        free(command);

    if (f1) fclose(f1);
    if (f2) fclose(f2);
}