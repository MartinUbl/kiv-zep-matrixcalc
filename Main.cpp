#include <iostream>
#include <ctime>
#include <windows.h>
#include <time.h>
using namespace std;

LARGE_INTEGER frequency;
LARGE_INTEGER t1, t2;
unsigned int rad;
unsigned int pas;

inline void startTimer(const char* task = "")
{
    cout << task << endl;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);
}

inline void stopTimer()
{
    QueryPerformanceCounter(&t2);
    cout << "Cas: " << (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart << "ms" << endl;
}

inline int maticeRandom()
{
    return rand() % 10;
}

/**
 * Standardni bruteforce metoda nasobeni matic - vygeneruje vsechny prvky vcetne nul, nasobi vsechny prvky vcetne nul
 *
 * Nebezpecne pro n > 10000 (lisi se podle PC - dle typu procesoru (x86 / x86_64) a dostupne operacni pameti)
 */
void metoda_bruteforce()
{
    unsigned int i, j;
    int** matice = NULL;
    int* vektor = NULL;
    int* vysledek = NULL;

    cout << "Metoda: bruteforce" << endl << endl;

    startTimer("Alokace pameti");

    matice = new int*[rad];
    vektor = new int[rad];
    vysledek = new int[rad];

    // alokace radku matice
    for (i = 0; i < rad; i++)
    {
        matice[i] = new int[rad];
        memset(matice[i], 0, sizeof(int)*rad);
    }

    stopTimer();

    startTimer("Naplneni matice");

    // inicializace matice, nasobeneho vektoru a vynulovani vysledku
    for (i = 0; i < rad; i++)
    {
        matice[i][i] = maticeRandom();
        for (j = 1; j < pas && j + i < rad; j++)
        {
            matice[i][i + j] = maticeRandom();
            matice[i + j][i] = maticeRandom();
        }

        vektor[i] = maticeRandom();
        vysledek[i] = 0;
    }

    stopTimer();

    startTimer("Nasobeni");

    // nasobeni
    for (i = 0; i < rad; i++)
    {
        for (j = 0; j < rad; j++)
        {
            vysledek[i] += matice[i][j] * vektor[j];
        }
    }

    stopTimer();

    /*cout << "Matice: " << endl;
    for (i = 0; i < rad; i++)
    {
        for (j = 0; j < rad; j++)
        {
            cout << matice[i][j] << " ";
        }
        cout << "\t" << vektor[i];
        cout << "\t\t" << vysledek[i];
        cout << endl;
    }*/

    startTimer("Dealokace");

    for (i = 0; i < rad; i++)
        delete[] matice[i];
    delete[] matice;
    delete[] vektor;
    delete[] vysledek;

    stopTimer();
}

/**
 * Tato metoda zaklada na vygenerovani "pruhu", ktere obsahuji pouze platne prvky matice,
 * ale vygeneruje take "padding" v podobe nul, aby byl algoritmus genericky, a nebylo treba
 * pouzivat v prubehu nasobeni podminky
 */
void metoda_pasova_padding()
{
    unsigned int i, j;
    int* matice = NULL;
    int* vektor = NULL;
    int* vysledek = NULL;

    int sirkaRadku = (2 * (pas - 1) + 1);

    cout << "Metoda: pasova s paddingem" << endl << endl;

    startTimer("Alokace");

    matice = new int[sirkaRadku*rad];
    vektor = new int[rad + 2*(pas-1)];
    vysledek = new int[rad];

    memset(matice, 0, sizeof(int)*sirkaRadku*rad);
    memset(vektor, 0, sizeof(int)*(rad + 2 * (pas - 1)));

    stopTimer();

    startTimer("Naplneni matice");

    for (i = 0; i < rad; i++)
    {
        for (j = 0; j < sirkaRadku; j++)
        {
            if (((i > pas - 1) || (j >= pas - 1 - i)) && ((i < rad - pas + 1) || (j + i - pas+1 < rad)))
                matice[sirkaRadku*i + j] = maticeRandom();
        }

        vektor[i + pas - 1] = i + 1;
        vysledek[i] = 0;
    }

    stopTimer();

    /*for (i = 0; i < rad; i++)
    {
        for (j = 0; j < sirkaRadku; j++)
        {
            cout << matice[sirkaRadku*i + j] << " ";
        }
        cout << endl;
    }*/

    startTimer("Nasobeni");

    for (i = 0; i < rad; i++)
    {
        for (j = 0; j < sirkaRadku; j++)
        {
            vysledek[i] += matice[sirkaRadku*i + j] * vektor[i+j];
        }
    }

    stopTimer();

    startTimer("Dealokace");

    delete[] matice;
    delete[] vektor;
    delete[] vysledek;

    stopTimer();
}

void metoda_pasova_tesna()
{
    //
}

void (*methods[])() = {metoda_bruteforce, metoda_pasova_padding, metoda_pasova_tesna};

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        cout << "Parametry: <rad matice> <sirka pasu> <metoda>" << endl;
        cout << "<rad matice> - rad vstupni matice, cislo od 2 do 2^32-1" << endl;
        cout << "<sirka pasu> - sirka diagonalniho pasu, cislo od 1 do n/2" << endl;
        cout << "<metoda>     - metoda nasobeni" << endl;
        cout << "Dostupne metody: 0 - bruteforce" << endl;
        cout << "                 1 - pasove nasobeni s paddingem" << endl;
        cout << "                 2 - pasove nasobeni bez paddingu" << endl;
        exit(-1);
    }

    rad = atoi(argv[1]);
    pas = atoi(argv[2]);
    unsigned char metoda = atoi(argv[3]);

    if (metoda > 2)
    {
        cout << "Neplatna metoda. Zvolte 0, 1 nebo 2";
        exit(-1);
    }

    srand((unsigned int)time(NULL));

    methods[metoda]();

    cin.get();

    return 0;
}
