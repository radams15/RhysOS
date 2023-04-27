extern int interrupt (int number, int AX, int BX, int CX, int DX);

int main() {
	interrupt(0x21, 0, "Hello world\r\n", 0, 0);

	return 0x6;
}
