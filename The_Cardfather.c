#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

//---------------------------------------------------------------------------------------------------

/// Enums per semi e valori delle carte
typedef enum
{
    CUORI,  // 0
    FIORI,  // 1
    QUADRI, // 2
    PICCHE  // 3
} Seme;

//---------------------------------------------------------------------------------------------------

typedef enum
{
    ASSO,    // 0
    DUE,     // 1
    TRE,     // 2
    QUATTRO, // 3
    CINQUE,  // 4
    SEI,     // 5
    SETTE,   // 6
    J,       // 7
    Q,       // 8
    K        // 9
} Valore;

//---------------------------------------------------------------------------------------------------

/// Struttura carta
typedef struct
{
    Seme seme;
    Valore valore;
} Carta;

//---------------------------------------------------------------------------------------------------

/// Carte in mano
typedef struct
{
    Carta carta1;
    Carta carta2;
} in_mano;

//---------------------------------------------------------------------------------------------------

/// Giocatori
typedef struct
{
    int id;
    in_mano mano;
    int HP;
    char nome[15]; // massimo caratteri
} player;

//---------------------------------------------------------------------------------------------------

/// Funzione che genera due carte casuali
Carta genera_carte_random(Carta *carta1)
{
    carta1->valore = rand() % 10; // da 0 a 9
    carta1->seme = rand() % 4;    // da 0 a 3
    return *carta1;
}

//---------------------------------------------------------------------------------------------------

/// Stampa carta
void stampaCarta(Carta c)
{
    const char *semi[] = {"Cuori", "Fiori", "Quadri", "Picche"};
    const char *valori[] = {"Asso", "Due", "Tre", "Quattro", "Cinque", "Sei", "Sette", "J", "Q", "K"};
    printf("Carta: %s di %s\n", valori[c.valore], semi[c.seme]);
}

//---------------------------------------------------------------------------------------------------

/// Chiedi numero di giocatori
void n_giocatori(player **condannati, int *sizePlayers)
{
    printf("Quanti giocatori intendono giocare: "); 
    /// implementazione inserimento in modo tale da assicurare che giochino dai 2 ai 20 giocatori
    do
    {
        scanf("%d", sizePlayers);
        printf("**********************\n");
        if ((*sizePlayers) < 2 || (*sizePlayers) > 20)
        {
            printf("Errore: Numero di giocatori non valido. Inserisci un numero da 2 a 20\n");
        }
    } while (((*sizePlayers) < 2) || ((*sizePlayers) > 20));

    ///*condannati = (players *)malloc(*sizePlayers * sizeof(players));
    *condannati = (player *)calloc(*sizePlayers, sizeof(player));
    if (*condannati == NULL)
    {
        printf("Errore nell'allocazione della memoria per i giocatori.\n");
        exit(1);
    }
    ///implementazione per avere una massima lunghezza del nome dei giocatori
    for (int i = 0; i < (*sizePlayers); i++)
    {
        do
        {
            printf("Giocatore numero %d: \nInserire il nome (max 14 caratteri): ", i + 1);
            scanf("%14s", (*condannati)[i].nome);                                        /// Evita buffer overflow e mette massima lunghezza
            /// Pulisce il buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);///scarta tutti i caratteri in input in più fino a quando non si incontra '\n' o la fine del file

            if (strlen((*condannati)[i].nome) == 0)
            {
                printf("Nome troppo lungo, inserisci un massimo di 14 caratteri\n");
            }

        } while (strlen((*condannati)[i].nome) == 0); ///controllo per assicurarsi che la stringa non sia vuota

        (*condannati)[i].id = i;
        (*condannati)[i].HP = 2;
        printf("Punti vita iniziali: %d\n", (*condannati)[i].HP);
        printf("***********************\n");
    }///ciclo esterno: esegue per tutti i giocatori
}

//---------------------------------------------------------------------------------------------------

/// Mescolamento turni dei giocatori
void mescola_turni(player **condannati, int size)
{
    ///Scegli un indice casuale per il primo giocatore
    int primo = rand() % size;

    /// Crea un array temporaneo per riorganizzare i turni
    player *temp = (player *)calloc(size, size * sizeof(player));
    if (temp == NULL)
    {
        printf("Errore nell'allocazione della memoria per i turni.\n");
        exit(1);
    }

    /// Copia i giocatori in ordine crescente a partire da "primo"
    int index = 0;
    for (int i = primo; i < size; i++)
    {
        temp[index] = (*condannati)[i];
        index++;
    }
    for (int i = 0; i < primo; i++)
    {
        temp[index] = (*condannati)[i];
        index++;
    }

    /// Aggiorna l'ordine originale con l'array temporaneo
    for (int i = 0; i < size; i++)
    {
        (*condannati)[i] = temp[i];
       
    }

    free(temp); /// Libera la memoria temporanea

    ///Stampa i turni
    for (int i = 0; i < size; i++)
    {
        printf("Turno %d: %s\n", i + 1, (*condannati)[i].nome);
    }
}

//---------------------------------------------------------------------------------------------------

/// Mescola mazzo di carte
void mescola_mazzo(Carta **deck, int *dim) 
{
    *deck = (Carta *)calloc((*dim), sizeof(Carta));
    /// controllo allocazione di memoria
    if (*deck == NULL)
    {
        printf("Errore nell'allocazione del mazzo.\n");
        exit(1);
    }
    /// inizializzazione del mazzo
    int index = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            (*deck)[index].seme = (Seme)i;
            (*deck)[index].valore = (Valore)j;
            index++;
        }
    }
    /// shuffle: assegnazione randomica dei valori e semi tramite una struttura di appoggio temp
    for (int i = (*dim) - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        Carta temp = (*deck)[i];
        (*deck)[i] = (*deck)[j];
        (*deck)[j] = temp;
    }
    
}

//---------------------------------------------------------------------------------------------------
void effetti(player **condannati, int sizePlayers, int *campogioco, int i, bool scoperta);
int elimina_giocatori(player **condannati, int *sizePlayers);
//---------------------------------------------------------------------------------------------------

Carta genera_e_assegna(player **condannati, int sizePlayers, int *campogioco, int turno, Carta **deck)
{
    bool scoperta; ///segnale booleano per fare in modo che si tratti la carta in mano desiderata

    Carta a = genera_carte_random(&(*condannati)[turno].mano.carta1);
    (*condannati)[turno].mano.carta1 = a;

    Carta b = genera_carte_random(&(*condannati)[turno].mano.carta2);
    (*condannati)[turno].mano.carta2 = b;

    /// stampa delle carte
    printf("carte di %s\n", (*condannati)[turno].nome);
    stampaCarta((*condannati)[turno].mano.carta1);
    scoperta = false;
    effetti(condannati, sizePlayers, campogioco, turno, scoperta);

    char risposta;
    printf("vuoi visionare la carta coperta? digita 's' per scoprirla e un qualsiasi altro tasto per lasciarla coperta\n");
    scanf(" %c", &risposta);
    if ((risposta == 's') || (risposta == 'S')) /// accetta sia maiuscolo che minuscolo
    {
        scoperta = true; /// segnale per indirizzare la funzione ad applicare gli effetti a carta2(coperta) invece di carta1(scoperta)
        stampaCarta((*condannati)[turno].mano.carta2);
        effetti(condannati, sizePlayers, campogioco, turno, scoperta);
    }

    else
    {
        printf("la carta resta coperta! :| \n");
    }

}

//---------------------------------------------------------------------------------------------------

void effetti(player **condannati, int sizePlayers, int *campogioco, int turno, bool scoperta) /// cambiato nome di "i" in "turno"
{                                                                                              
    int next;
    Carta *carta;
    if (scoperta)
    {
        carta = &(*condannati)[turno].mano.carta2;
    }
    else
    {
        carta = &(*condannati)[turno].mano.carta1;
    } /// implementazione per carta2

    switch (carta->valore)
    {
    case 0: /// Carta Asso
        printf("%s perde 1 HP e lo deposita nel campo di gioco :(\n", (*condannati)[turno].nome);
        printf("\"CHI SE LO AGGIUDICHERA\'??\"\n");
        (*condannati)[turno].HP--;
        (*campogioco)++;
        break;
    case 6: /// carta 7
         next = (turno + 1) % sizePlayers;

    // Cerca il prossimo giocatore con HP > 0
    int tentativi = 0;
    while ((*condannati)[next].HP <= 0 && tentativi < sizePlayers) {
        next = (next + 1) % sizePlayers;
        tentativi++;
    }

    // Se tutti i giocatori sono eliminati o nessuno è valido
    if (tentativi >= sizePlayers) {
        printf("Non ci sono giocatori validi per mostrare la carta.\n");
        break;
    }

    scoperta = false; // Forza la visione della carta coperta
    printf("%s mostra la carta coperta!\n", (*condannati)[next].nome);
    stampaCarta((*condannati)[next].mano.carta2);
    effetti(condannati, sizePlayers, campogioco, next, scoperta);
    break;


    case 7: /// Carta J
        next = (turno - 1) % sizePlayers; /// operazione modulo per trovare il giocatore precedente

        ///stampa della situazione aggiornata e operazioni varie sugli HP
        printf("%s regala punto vita a %s\n", (*condannati)[turno].nome, (*condannati)[next].nome);
        (*condannati)[turno].HP--;
        (*condannati)[next].HP++;
        printf("HP aggiornati: giocatore %s ha %d HP e giocatore %s ha %d HP\n", (*condannati)[turno].nome, (*condannati)[turno].HP, (*condannati)[next].nome, (*condannati)[next].HP);

        break;     
    case 8:  /// Carta Q
        next = (turno + 2) % sizePlayers;
        ///il giocatore corrente da un punto vita al giocatore successivo al successivo
        if(sizePlayers==2) ///caso di soli 2 giocatori
            printf("Gli HP di %s non cambiano :c\n", (*condannati)[turno].nome);
        else{
            printf("%s regala punto vita a %s\n", (*condannati)[turno].nome, (*condannati)[next].nome);
            (*condannati)[turno].HP--;
            (*condannati)[next].HP++;
            printf("HP aggiornati:giocatore %s ha %d HP e giocatore %s ha %d HP\n", (*condannati)[turno].nome, (*condannati)[turno].HP, (*condannati)[next].nome, (*condannati)[next].HP);
        }
        break;
    case 9: /// carta K
    ///prende i punti vita disponibili nel campo da gioco
        if ((*campogioco) != 0)
        {
            printf("%s, \"LA FORTUNA TI ARRIDE!\"\nPrendi tutti i punti vita sul campo da gioco :D\n", (*condannati)[turno].nome); 
            printf("Punti vita disponibili nel campo: %d \n", (*campogioco));
            (*condannati)[turno].HP = (*condannati)[turno].HP + (*campogioco);
            (*campogioco) = 0;
            printf("\nSITUAZIONE AGGIORNATA:\n");
            printf("%s \"ha preso tutti gli HP in campo, e le sue chance di vittoria aumentano ;-)\"\n", (*condannati)[turno].nome);
        }
        else
        {
            printf("\"LA FORTUNA TI ARRIDE UN PO' MENO\", il campo di gioco e\' vuoto! :c\n");
        }
        break;
    default:
        printf("\"POTEVA ANDARE MEGLIO MA ANCHE PEGGIO\": la carta di %s non ha effetti! :{\n", (*condannati)[turno].nome);
    }
}
//---------------------------------------------------------------------------------------------------
int elimina_giocatori(player **condannati, int *sizePlayers)
{
    /// Calcolare la nuova dimensione (numero di giocatori con HP > 0)
    int nuova_dim = 0;
    for (int i = 0; i < *sizePlayers; i++)
    {
        if ((*condannati)[i].HP > 0)
        {
            nuova_dim++;
        }
        else
        {
            printf("%s e\' stato eliminato  :'-(\n", (*condannati)[i].nome);
        }
    }
    /// se non ci sono più giocatori allora si esce dalla funzione
    if (nuova_dim == 0)
    {
        printf("Non ci sono giocatori rimasti!\n");
        return nuova_dim;
    }

    player *temp = (player *)calloc(nuova_dim, nuova_dim * sizeof(player));
    if (temp == NULL)
    {
        printf("Errore di allocazione memoria.\n");
        exit(1);
    }

    int index = 0;
    for (int i = 0; i < *sizePlayers; i++)
    {
        if ((*condannati)[i].HP > 0)
        {
            temp[index] = (*condannati)[i];
            index++;
        }
    }
    /// libera array originale
    free(*condannati);
    /// reallocazione di condannati aggiornato
    *condannati = temp;
    *sizePlayers = nuova_dim; /// Aggiorna la dimensione dell'array

    /// stampo i giocatori rimasti
    printf("Giocatori rimasti:\n");
    for (int i = 0; i < (*sizePlayers); i++)
    {
        printf("%s\n", (*condannati)[i].nome);
    }
    return *sizePlayers;
}
//---------------------------------------------------------------------------------------------------

int main()
{
    
    srand(time(NULL));
    int size;
    int dim=40;
    int campogioco = 0; /// indica quanti punti vita sono stati depositati su campo di gioco,
                        
    /// dichiarazioni varie
    Carta carta1;
    Carta carta2;
    Carta *deck;
    player *condannati; /// giocatori
    int round = 1;

    printf("\nBenvenuti su \"The Cardfather\", mi presento: sono il dottor Jack Deckman e vi introdurro' al gioco\n");
    printf("Ognuno di voi avra' assegnate 2 carte una scoperta e una coperta, durante il vostro turrno potrete decidere di scoprirla, se siete abbastanza audaci (*_*)\n");
    printf("Il vincitore sara' colui che restera' in vita e ricevera' il glorioso titolo di Mastro di cuori!\n");
    printf("\"NE RESTERA' SOLTATO UNO\". Buon divertimento e che la fortuna sia con voi!\n\n");

    n_giocatori(&condannati, &size);
    while (size != 1)
    {
        printf("\nround %d\n", round);
        mescola_turni(&condannati, size); 
        mescola_mazzo(&deck, &dim);
        printf("********************************\n");
        for (int t = 0; t < size; t++)
        {
            printf("Sta giocando %s...\n", (condannati)[t].nome);
            genera_e_assegna(&condannati, size, &campogioco, t, &deck); 
            printf("Turno di %s concluso!\n", condannati[t].nome);
            printf("\n********************************\n\n");
    
        }
        
        printf("Round %d concluso\n", round);
        size = elimina_giocatori(&condannati, &size);
        printf("\n********************************\n\n");
        round ++;
    }
    printf("\nEd e' con immenso onore che proclamo %s il nuovo mastro di cuori. Congratulazioni!\n", (*condannati).nome);

    free(condannati);
    free(deck);

    return 0;
}

