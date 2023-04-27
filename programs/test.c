extern int interrupt (int number, int AX, int BX, int CX, int DX);

int main() {
	enableInterrupts();
	interrupt(0x21, 0, "hello world\r\n", 0, 0);

	return 5;
}
