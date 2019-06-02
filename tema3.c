//Sima Iustin,325 CB
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DIM (36)

int maxlen;

typedef struct Nod 
{
	struct Nod *fii[DIM];     // vectorul de fii
	int flag;				// flag e 0 daca in nodul dat nu se termina niciun cuvant
} TNod;						//imi creez structura de nod numita TNod

typedef struct Trie 
{
	TNod *radacina;
} TTrie;

int TRANSFORMA(char caracter)				// functia TRANSFORMA imi calculeaza tinand cont de caracter indexul pe care il va avea nodul asociat caracterului caracterul
{
	int codcar=(int)caracter;
	if(codcar>=97 && codcar <=122)          //daca e litera indexul va incepe cu 10 pentru 'a' , 11 pentru 'b' si asa mai departe
		return codcar-87;					
		else if (codcar>=48 && codcar <=57)			//daca e cifra indexul va fi intre 0 si 9
				return codcar-48;
	
}

TNod *AlocaNod(void)		//aloc un nod				
{
	TNod *nodnou = malloc(sizeof(TNod));

	if (nodnou) {
		int cnt;

		nodnou->flag = 0;
		for (cnt = 0; cnt < DIM; cnt++)
			nodnou->fii[cnt] = NULL;     //imediat dupa alocare campul flag al noului nod va fi 0 iar nodul nu va avea fii
	}

	return nodnou;
}

void InitializareTrie(TTrie *p)
{
	p->radacina = AlocaNod();           //pentru initializarea trie-ului aloc doar radacina
}

void add(TTrie *p, const char *cuvant)
{
	int nivel, ind, lungime = strlen(cuvant);
	TNod *r = p->radacina;

	if (lungime > maxlen)								// update maxlen
		maxlen = lungime;

	for (nivel = 0; nivel < lungime; nivel++) 
	{    //parcurg un numar de nivele din trie egal cu lungimea cuvantului dat ca parametru
		ind = TRANSFORMA(cuvant[nivel]);
		if (r->fii[ind] == NULL)
			r->fii[ind] = AlocaNod();
		r = r->fii[ind];
	}

	r->flag = 1;
}

int find(TTrie *p, const char *cuvant)
{
	int nivel, ind, lungime = strlen(cuvant);
	TNod *r = p->radacina;                          //plec din radacina 

	for (nivel = 0; nivel < lungime; nivel++) 
	{
		ind = TRANSFORMA(cuvant[nivel]);
		if (r->fii[ind] == NULL)
			return 0;
		r = r->fii[ind];
	}

	if (r != NULL && r->flag == 1)          //daca dupa ce am parcurs un numar de nivele egal cu lungimea cuvantului dat ca parametru am in continuare nodul curent diferit de NULL si campul flag al  
		return 1;							//nodului curent egal cu 1 asta inseamna ca am gasit cuvantul dat in trie si returnez 1	

	return 0;								//daca nu s-a returnat deja 1 voi returna 0,cuvantul nefiind gasit anterior in trie deci nu mai sunt sanse sa il gasesc
}

char *find_longest_prefix(TTrie *p, char *cuvant)
{
	int nivel, ind, lungime = strlen(cuvant);
	char *prefix;

	prefix = malloc(maxlen * sizeof(char));
	strcpy(prefix, "N");					//initializez variabila prefix cu "N"
	TNod *r = p->radacina;

	for (nivel = 0; nivel < lungime; nivel++) 
	{
		ind = TRANSFORMA(cuvant[nivel]);

		if (r->fii[ind] == NULL) 
		{
			if (r == p->radacina)
				prefix = strcpy(prefix, "None");	//daca nodul r nu are niciun fiu pe pozitia ind si daca r e chiar radacina trie-ului atunci voi returna prefix

			return prefix;
		}

		if (strcmp(prefix, "N") == 0) 
		{				//altfel daca prefixul curent este "N" copiez sirul vid peste prefix si apoi pun primul caracter de la adresa cuvant[nivel] in prefix
			strcpy(prefix, "");
			strncpy(prefix, &cuvant[nivel], 1);
		} else
			strncat(prefix, &cuvant[nivel], 1);    //altfel,daca prefixul curent este diferit de "N" tot ce va trebui sa fac e sa anexez un nou caracter la prefixul curent 

		r = r->fii[ind];
	}

	if (r != NULL)								
		return prefix;
}

int count_words_with_prefix_aux(TNod *r)         //masoara cate cuvinte se termina in nodul r sau in nodurile inferioare lui r    
{
	int i, count = 0;

	if (r == NULL)										//conditie de oprire	
		return 0;

	for (i = 0; i < DIM; i++)
		count += count_words_with_prefix_aux(r->fii[i]); //apelez recursiv iar la finalul forului se va fi parcurs tot trie-ul

	return r->flag + count;									//returnez r->flag + count deoarece daca in r se termina un cuvant va trebuie si acesta adaugat
}

int count_words_with_prefix(TNod *r, char *prefix)  //parcurge trie-ul pana cand ajunge la nodul corespunzator ultimului caracter din prefix iar apoi se apeleaza functia auxiliara de acel nod
{
	int i, len = strlen(prefix);

	/* coboara pe trie */
	for (i = 0; i < len; i++) {
		if (r == NULL)
			return 0;

		r = r->fii[TRANSFORMA(prefix[i])];
	}

	return count_words_with_prefix_aux(r);
}

void auxfind(TNod *r, char **rez, int *contor, char *posibilcuvant, int nivel)
{
	char c;
	int i;

	for (i = 0; i < DIM; i++)
		if (r->fii[i]) 
		{
			if (i>=0 && i<=9)
				c=(char)(i+48);						//obtin din index caracterul
				else if (i>=10 && i<=35)
					 c=(char)(i+87);
			posibilcuvant[nivel] = c;				//anexez caracterul la posibil cuvant 

			if (r->fii[i]->flag) 
			{
				rez[(*contor)] = malloc((nivel + 1) * sizeof(char));
				strcpy(rez[(*contor)++], posibilcuvant);				//daca in fiul de pe pozitia i se termina un cuvant atunci adaug in rez cuvantul respectiv
			}

			auxfind(r->fii[i], rez, contor, posibilcuvant, nivel + 1);
			posibilcuvant[nivel] = 0;
		}
}

char **find_all_with_prefix(TTrie *p, char *prefix, int *rezlen)
{
	int i, nivel, ind, lungime = strlen(prefix);
	char **rez;
	int contor = 0;
	TNod *r = p->radacina;

	*rezlen = count_words_with_prefix(p->radacina, prefix);     // in rezlen retin numarul de cuvinte care incep cu prefix
	rez = malloc(*rezlen * sizeof(char *));

	for (nivel = 0; nivel < lungime; nivel++) 
	{	//iterez prin prefix si daca nu se gaseste un caracter din prefix automat nu vor exista cuvinte care sa inceapa cu respetivul prefix0si returnez "None"
		ind = TRANSFORMA(prefix[nivel]);
		if (r->fii[ind] == NULL) 
		{
			rez[0] = malloc(5 * sizeof(char));
			strcpy(rez[0], "None");
			*rezlen = 1;
			return rez;
		}
		r = r->fii[ind];
	}

	if (r->flag == 1) {
		rez[contor] = malloc((lungime + 1) * sizeof(char));
		strcpy(rez[contor++], prefix);                             //daca in r se termina un cuvant adaug cuvantul in rez
	}

	char *posibilcuvant = calloc(maxlen + 1, sizeof(char));
	strcpy(posibilcuvant, prefix);
	auxfind(r, rez, &contor, posibilcuvant, lungime);
	free(posibilcuvant);
	return rez;
}

int enodtermcuv(TNod *r)		//returneaza 1 daca in nodul dat ca parametru se termina un cuvant si 0 altfel
{
	if (r->flag)
		return 1;
	else
		return 0;
}

int efrunza(TNod *r)			// returneaza 1 daca nodul dat ca parametru e frunza si 0 altfel
{
	int i;

	for (i = 0; i < DIM; i++) 
	{
		if (r -> fii[i] != NULL)
			return 0;               //daca gasesc un fiu asta inseamna ca nodul dat ca parametru nu e frunza;
	}

	return 1;
}

bool auxremove(TNod *r, char *cuvant, int nivel, int lungime)
{
	if (r != NULL) 
	{
		if (nivel == lungime) 
		{				//daca nivel este egal cu lungime tot ce va trebui sa fac este sa verific daca flag-ul e 1 si daca da atunci il fac 0 pentru ca scot cuvantul si verific 
			if (r->flag == 1) 
			{				
				r->flag = 0;
				if (efrunza(r))				//daca e frunza
					return true;
				return false;
			}
		} else {
			int ind = TRANSFORMA(cuvant[nivel]);

			if (auxremove(r->fii[ind], cuvant, nivel + 1, lungime))     //apelez recursiv pentru nivelul urmator 
			{	free(r->fii[ind]);										//eliberez fiul corespunzator indicelui ind 
				r->fii[ind]=NULL;
				if (enodtermcuv(r) == 0 && efrunza(r) == 1)				// daca r e frunza si daca in r nu se mai termina niciun cuvant returnez true altfel false
					return true;
				else
					return false;
			}
		}
	}

	return false;
}

int removew(TTrie *p, char *cuvant)
{
	int lungime = strlen(cuvant);

	if (lungime)
		auxremove(p->radacina, cuvant, 0, lungime);		//apelez cu p->radacina si 0 pentru a incepe cat mai de sus posibil stergerea cuvantului 

	return 1;
}

void free_memory(char **rez, int nr)
{
	int i;

	for (i = 0; i < nr; i++) {
		free(rez[i]);
	}
	free(rez);
}

double mean_length(TTrie *p, char *prefix)
{
	int nrcuvinte, i, suma=0;
	double medie;
	char **rez; 
	
	rez = find_all_with_prefix(p, prefix, &nrcuvinte);   //in rez retin cuvintele care incep cu prefix,adica cele intoarse de functia find_with_all_prefx
	
	if(!strcmp(rez[0],"None"))
		return 0;
	
	for(i = 0; i < nrcuvinte; i++)			//iterez prin vectorul de cuvinte si la suma curenta adaug lungimea cuvantului curent
		suma += strlen(rez[i]);
	
	medie = (double)suma/nrcuvinte;			//dupa ce am calculat suma lungimilor tuturor cuvintelor care incep cu prefix calculez media
	
	free_memory(rez, nrcuvinte);

	return medie;
}

int main(int argc, char **argv)
{
	int i, j, valoare, N, lungime, ret, rezlen;
	double rez3;
	int rez;
	char **rez2;
	char *rez4;
	char comanda[50];
	char cuvant[3000];
	char line[3000];
	TTrie trie;

	InitializareTrie(&trie);

	FILE *in = fopen (argv[1],"rt");
	FILE *out = fopen (argv[2],"wt");
	fscanf (in,"%d",&N);
	fgetc(in);
	for (i = 1; i <= N; i++)
	{
		fgets(line, 3000, in);
		ret = sscanf(line, "%s%s", comanda, cuvant);
		if (ret == 2) 
		{
			if (!strcmp (comanda, "add"))
				add (&trie, cuvant);
			if(!strcmp (comanda, "find"))
				{valoare = find (&trie, cuvant);
				 if (valoare)
				 	 fprintf (out, "True\n");
						else fprintf(out,"False\n");
				}
			if (!strcmp (comanda, "remove"))
				removew (&trie, cuvant);
			if (!strcmp (comanda, "find_longest_prefix"))
			{
				rez4 = find_longest_prefix (&trie, cuvant);
				fprintf (out, "%s\n", rez4);
				free(rez4);
			}
			if(!strcmp(comanda,"find_all_with_prefix"))
			{
				rez2=find_all_with_prefix(&trie,cuvant,&rezlen);
				for(j=0;j<rezlen;j++)
					fprintf(out, "%s ",rez2[j]);
				fprintf(out,"\n");
				free_memory(rez2, rezlen);
			}
			if(!strcmp(comanda,"mean_length"))
			{	rez3=mean_length(&trie,cuvant);
				if(rez3==0) fprintf(out,"0\n");
					else fprintf(out, "%.3f\n",rez3);
			}
		} else 
		{
			if(!strcmp(comanda,"find_all_with_prefix"))
			{
				rez2=find_all_with_prefix(&trie,"",&rezlen);
				for(j=0;j<rezlen;j++)
					fprintf(out, "%s ",rez2[j]);
				fprintf(out,"\n");
				free_memory(rez2, rezlen);
			}
			if(!strcmp(comanda,"mean_length"))
			{	rez3=mean_length(&trie,"");
				fprintf(out, "%.3f\n",rez3);
			}
		}
	}	

	fclose (in);
	fclose (out);

	return 0;
}
