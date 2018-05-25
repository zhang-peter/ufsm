#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ufsm.h>

#include "output.h"

static FILE *fp_c = NULL;
static FILE *fp_h = NULL;

static char * id_to_decl(const char *id)
{
    char * decl = malloc (strlen(id));
    char * decl_ptr = decl;

    do {
        *decl_ptr = *id++;
        if (*decl_ptr == '+')
            *decl_ptr = '_';
        if (*decl_ptr == '/')
            *decl_ptr = '_';
        if (*decl_ptr == '=')
            *decl_ptr = 0;

    } while (*decl_ptr++);
    
    
    return decl;
}



static void ufsm_gen_regions(struct ufsm_region *region);

static void ufsm_gen_states(struct ufsm_state *state)
{
    fprintf(fp_c,"static struct ufsm_state %s {\n",id_to_decl(state->id));
    fprintf(fp_c,"  .id   = \"%s\",\n",state->id);
    fprintf(fp_c,"  .name = \"%s\",\n",state->name);
    fprintf(fp_c,"  .kind = %i,\n",state->kind);
    if (state->entry)
        fprintf(fp_c,"  .entry = &%s,\n",id_to_decl(state->entry->id));
    else
        fprintf(fp_c,"  .entry = NULL,\n");
    if (state->exit)
        fprintf(fp_c,"  .exit = &%s,\n",id_to_decl(state->exit->id));
    else
        fprintf(fp_c,"  .exit = NULL,\n");

    if (state->region)
        fprintf(fp_c,"  .region = &%s,\n",id_to_decl(state->region->id));
    else
        fprintf(fp_c,"  .region = NULL,\n");

    if (state->submachine)
        fprintf(fp_c,"  .submachine = &%s,\n",id_to_decl(state->submachine->id));
    else
        fprintf(fp_c,"  .submachine = NULL,\n");

    if (state->next)
        fprintf(fp_c,"  .next = &%s,\n",id_to_decl(state->next->id));
    else
        fprintf(fp_c,"  .next = NULL,\n");

    fprintf(fp_c,"};\n");
    if (state->region)
        ufsm_gen_regions(state->region);



    for (struct ufsm_entry_exit *e = state->entry; e; e = e->next) {
        fprintf(fp_c, "static struct ufsm_entry_exit %s {\n",
                        id_to_decl(e->id)); 
        fprintf(fp_c, "  .id = \"%s\",\n", e->id);
        fprintf(fp_c, "  .name = \"%s\",\n",e->name);
        fprintf(fp_c, "};\n");
    }

    for (struct ufsm_entry_exit *e = state->exit; e; e = e->next) {
        fprintf(fp_c, "static struct ufsm_entry_exit %s {\n",
                        id_to_decl(e->id));

        fprintf(fp_c, "};\n");
    }


}

static void ufsm_gen_regions(struct ufsm_region *region) 
{
    for (struct ufsm_region *r = region; r; r = r->next) {
        fprintf (fp_c,"static struct ufsm_region %s {\n",id_to_decl(r->id));
        fprintf (fp_c,"  .id = \"%s\",\n", r->id);
        fprintf (fp_c,"  .name = \"%s\",\n", r->name);
        fprintf (fp_c,"  .state = &%s,\n", id_to_decl(r->state->id));
        if (r->transition)
            fprintf (fp_c,"  .transition = &%s,\n", id_to_decl(r->transition->id));
        else   
            fprintf (fp_c,"  .transition = NULL,\n");

        if (r->next)
            fprintf (fp_c,"  .next = &%s,\n",id_to_decl(r->next->id));
        else
            fprintf (fp_c,"  .next = NULL,\n");
        fprintf (fp_c,"};\n");
        for (struct ufsm_transition *t = r->transition; t; t = t->next) {
            fprintf(fp_c, "static struct ufsm_transition %s {\n",
                               id_to_decl(t->id));
            fprintf(fp_c, "  .id = \"%s\",\n", t->id);
            fprintf(fp_c, "  .name = \"\",\n");
            fprintf(fp_c, "  .trigger = %i,\n",t->trigger);
            fprintf(fp_c, "  .kind = %i,\n",t->kind);
            if (t->action)
                fprintf(fp_c, "  .action = &%s,\n", id_to_decl(t->action->id));
            else
                fprintf(fp_c, "  .action = NULL,\n");
            if (t->guard)
                fprintf(fp_c, "  .guard = &%s,\n", id_to_decl(t->guard->id));
            else
                fprintf(fp_c, "  .guard = NULL,\n");
            fprintf(fp_c, "  .source = &%s,\n",id_to_decl(t->source->id));
            fprintf(fp_c, "  .dest = &%s,\n",id_to_decl(t->dest->id));
            
            if (t->next)
                fprintf(fp_c, "  .next = &%s,\n",id_to_decl(t->next->id));
            else
               fprintf(fp_c, "  .next = NULL,\n");
            fprintf(fp_c, "};\n");

            for (struct ufsm_action *a = t->action; a; a = a->next) {
                fprintf(fp_c, "static struct ufsm_action %s {\n",
                            id_to_decl(a->id));
                fprintf(fp_c, "  .id = \"%s\",\n", a->id);
                fprintf(fp_c, "  .name = \"%s\",\n", a->name);
                fprintf(fp_c, "  .f = &%s,\n", a->name);
                if (a->next)
                    fprintf(fp_c, "  .next = &%s,\n", id_to_decl(a->next->id));
                else
                    fprintf(fp_c, "  .next = NULL,\n");
                fprintf(fp_c, "};\n");
            }

            for (struct ufsm_guard *g = t->guard; g; g = g->next) {
                fprintf(fp_c, "static struct ufsm_guard %s {\n",
                            id_to_decl(g->id));
                fprintf(fp_c, "  .id = \"%s\",\n", g->id);
                fprintf(fp_c, "  .name = \"%s\",\n", g->name);
                fprintf(fp_c, "  .f = &%s,\n", g->name);
                if (g->next)
                    fprintf(fp_c, "  .next = &%s,\n", id_to_decl(g->next->id));
                else
                    fprintf(fp_c, "  .next = NULL,\n");
 
                fprintf(fp_c, "};\n");
            }
        }

        for (struct ufsm_state *s = r->state; s; s = s->next)
            ufsm_gen_states(s);

    }
}

bool ufsm_gen_machine (struct ufsm_machine *m)
{
    fprintf (fp_c,"static struct ufsm_machine %s {\n",id_to_decl(m->id));
    fprintf (fp_c,"  .id     = \"%s\", \n", m->id);
    fprintf (fp_c,"  .name   = \"%s\", \n", m->name);
    fprintf (fp_c,"  .region = &%s,    \n",id_to_decl(m->region->id));
    if (m->next)
        fprintf (fp_c,"  .next = &%s, \n", id_to_decl(m->next->id));
    else
        fprintf (fp_c,"  .next = NULL,\n");
        
    fprintf (fp_c,"};\n");
    ufsm_gen_regions(m->region);
    return true;
}



static void ufsm_gen_regions_decl(struct ufsm_region *region);

static void ufsm_gen_states_decl(struct ufsm_state *state)
{
    fprintf(fp_c,"static struct ufsm_state %s;\n",id_to_decl(state->id));
    if (state->region)
        ufsm_gen_regions_decl(state->region);



    for (struct ufsm_entry_exit *e = state->entry; e; e = e->next)
        fprintf(fp_c, "static struct ufsm_entry_exit %s;\n",
                        id_to_decl(e->id));
    for (struct ufsm_entry_exit *e = state->exit; e; e = e->next)
        fprintf(fp_c, "static struct ufsm_entry_exit %s;\n",
                        id_to_decl(e->id));


}

static void ufsm_gen_regions_decl(struct ufsm_region *region) 
{
    for (struct ufsm_region *r = region; r; r = r->next) {
        fprintf (fp_c,"static struct ufsm_region %s;\n",id_to_decl(r->id));

        for (struct ufsm_transition *t = r->transition; t; t = t->next) {
            fprintf(fp_c, "static struct ufsm_transition %s;\n",
                               id_to_decl(t->id));
        
            for (struct ufsm_action *a = t->action; a; a = a->next)
                fprintf(fp_c, "static struct ufsm_action %s;\n",
                            id_to_decl(a->id));


            for (struct ufsm_guard *g = t->guard; g; g = g->next)
                fprintf(fp_c, "static struct ufsm_guard %s;\n",
                            id_to_decl(g->id));

        }

        for (struct ufsm_state *s = r->state; s; s = s->next)
            ufsm_gen_states_decl(s);

    }
}

bool ufsm_gen_machine_decl (struct ufsm_machine *m)
{
    fprintf (fp_c,"static struct ufsm_machine %s;\n",id_to_decl(m->id));
    ufsm_gen_regions_decl(m->region);
    return true;
}

bool ufsm_gen_output(struct ufsm_machine *root) {
    printf ("o Generating output\n");

    const char output_name[] = "ufsm_gen_output";
    
    char *fn_c = malloc(strlen(output_name)+3);
    char *fn_h = malloc(strlen(output_name)+3);

    sprintf(fn_c, "%s.c",output_name);
    sprintf(fn_h, "%s.h",output_name);

    fp_c = fopen(fn_c, "w");
    fp_h = fopen(fn_h, "w");

    fprintf(fp_h,"#ifndef __%s_H__\n", output_name);
    fprintf(fp_h,"#define __%s_H__\n", output_name);
    fprintf(fp_h,"#include <ufsm.h>\n");

    fprintf(fp_c,"#include \"%s\"\n", fn_h);

    for (struct ufsm_machine *m = root; m; m = m->next)
        ufsm_gen_machine_decl(m);

    fprintf(fp_c,"\n\n\n");
    for (struct ufsm_machine *m = root; m; m = m->next)
        ufsm_gen_machine(m);

    fprintf(fp_c,"\n");
    fprintf(fp_c,"struct ufsm_machine * get_%s(void) { return &%s; }\n",
                output_name, id_to_decl(root->id));

    fprintf(fp_h,"struct ufsm_machine * get_%s(void);\n",output_name);
    fprintf(fp_h,"#endif\n");
    fclose(fp_c);
    fclose(fp_h);


    return true;
}