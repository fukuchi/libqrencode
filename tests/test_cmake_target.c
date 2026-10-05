#include <qrencode.h>

int main(void)
{
	QRcode *code = QRcode_encodeString("CMake target", 0, QR_ECLEVEL_L, QR_MODE_8, 1);
	if(code == 0) return 1;
	QRcode_free(code);
	return 0;
}
