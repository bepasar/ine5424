#include <utility/ostream.h>

using namespace EPOS::S::U;

OStream cout;

int main()
{
    cout << "Hello world!" << endl;

    int * a =  new int[30];
    cout << "Endereco a = " << a << endl;

    int * b = new int;
    cout << "Endereco b = " << b << endl;

    int * c = new int;
    cout << "Endereco c = " << c << endl;

    assert(a < b);
    assert(b < c);

    cout << "Desalocando b..." << endl;
    free(b);

    cout << "Alocando d..." << endl;
    int * d = new int;

    cout << "Endereco d = " << d << endl;

    assert(a < d);
    assert(d > c);

    return 0;
}
