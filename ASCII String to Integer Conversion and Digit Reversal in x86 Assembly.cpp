
#include <iostream>
using namespace std;
int main(int argc, char** argv)
{
    char* s = argv[1];
    int error = 0; // 0 = gerai / 1 = perpildymas / 2 = ne skaitmuo
    int out = 0;
    int reversed = 0;
    int count = 0; // ant kiek pastumiam skaiciu steke
    if (argc < 2)
    {
        cout << "Nepateiktas skaicius";
        return 0;
    }
    _asm
    {
        push esi
        push edi
        push ebx
        push eax
        mov esi, s // input - rodykle i eulutes pradzios adresa 
        xor edi, edi // output kaupia rezultata
        mov ebx, 1 // zenklo nustatymas 
        xor eax, eax // darbo registras
        movzx eax, byte ptr[esi]// pirma simboli nuskaito
        cmp eax, '-'
        jne ciklas // jeigu mato skaiciu su minusu, tai soka i cikla tik tada
        mov ebx, -1 // galim sakit, kad is skaiciaus atimam - ir tada veliau pritaikom cikle, kad negalvoti apie skaiciu
        inc esi
        jmp ciklas
        ciklas :
        movzx eax, byte ptr[esi] // nuskaito po 1 baita is atminties kur dabar yra esi, o movzx iraso gauta baita i EAX ir uzpildo likusia vieta 0
            cmp eax, 0 // tikrinama, ar pasiekta string pabaiga, nustato veliaveles JE suveikia jeigu rezultatas = 0, ZF = 1
            je success
            //tikrinam ar 48>n<57 (Ar tikrai sakicius) ziuri ribas pagal ascii lentele
            cmp eax, 48
            jl invalid
            cmp eax, 57
            ja invalid
            //konvertuojam simboli i skaiciu
            sub eax, 48 // PVZ: jeigu pagal ascii 5 = 53, bet mums reikia norm skaiciaus, tai 53 - 48 = 5
            imul edi, 10 // padaugina EDI is 10, arba pastumia kairen, pvz: 5*10 = 50, per viena skaitmeni 5 permeta i kaire
            jo overflow // oveerflow, pasiekiamos ribos
            add edi, eax // sudeda EDI su EAX, 50 + 
            jo overflow// jeigu OF = 1, soka i overflow, persipildo intas
            push eax // stumia viska i steka, tai pvz: pirmas skaicius bus 5, tada pastums i steka, ir jau dirbis su 2, nesi ESI rodo i '2'steke, ir tada reikes pridet EDI + EAX = 50 + 2, ir taip toliau vyksta ciklas
            inc count// per kiek vienetu stumia viska steke
            inc esi // adresas padidinamas +1, pereiti prie kito simbolio
            jmp ciklas // soka skaiciu 
            invalid : // jei ivestas ne skaicius
        mov error, 2
            jmp pabaiga
            overflow : // jei skaicius per didelis
        mov error, 1
            jmp pabaiga
            success : // rezultato isvedimas
        imul edi, ebx
            mov out, edi
            //reverse skaiciai
            xor edi, edi // nuvalomas EDI, nes anskciau laike originalu sk, pvz = 5
            mov ecx, count // ECX rodo kiek skaitmenu steke, pvz 52431, ECX = 5
            reverse_loop : // jeigu buvo 54321, jis padaro 13452
        cmp ecx, 0
            je reverse_done // tikrina ar stekas tuscias
            pop eax // isima skaiciu is steko virsau tai butu 1
            imul edi, 10 // tada daugina EDI = 0 * 10 = 0
            jo overflow_rev
            add edi, eax // tada EDI + ECX 0 + 1 = 1
            jo overflow_rev
            dec ecx// Tada 5 - 1 = 4 ir ECX = 4, ir taip tesia cikla toliau, kol bus 4,3,2,1,0
            jmp reverse_loop// soka atgal i cikla, kol stekas nebus tuscias
            overflow_rev :
        mov error, 3 // atvirk tinis per didelis
            clean_rev :
            cmp ecx, 0 // isvalom steka
            je pabaiga
            pop eax
            dec ecx
            jmp clean_rev
            reverse_done :
        imul edi, ebx // pritaiko zenkla
            mov reversed, edi// perraso reiksme i EDI jau c++ kintamajame
            pabaiga :
        pop eax
            pop ebx
            pop edi
            pop esi
    }
    if (error == 0) {
        cout << "int = " << out << endl;
        cout << "reverse = " << reversed;
    }
    else if (error == 1) cout << "Skaicius per didelis";
    else if (error == 3) cout << "Atvirkstinis skaicius per didelis";
    else cout << "Simboliai eiluteje - ne skaicius";
    return 0;
}