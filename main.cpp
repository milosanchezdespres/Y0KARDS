#include "source/File.h"
#include "source/Colors.h"

int main()
{
    BMP test = load_bmp("assets/charsets/player");

    test.edit(0, 1, {238, 36, 255});

    RGBA color = to_rgba(test.pixel(0, 1));

    cout << color << endl;

    //...
    
    return 0;
}
