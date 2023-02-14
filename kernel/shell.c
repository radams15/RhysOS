int prompt[] = "> ";
int buffer[256];

void run_cmd(int* cmd){
    print_string("Run: \n");
    print_string(cmd);
    print_string("\r\n");
}

void shell() {
    int index;
    int byte;
    
    index = 0;
    memset(buffer, 0, sizeof(buffer));

    clear_screen();
    
    for(;;) {
        print_string(prompt);

        while((byte = getch())) {
            if((byte >> 8)  == 0x1c) {
                print_char('\r');
                print_char('\n');
                
                buffer[index] = 0; // Terminate the command
                run_cmd(buffer);
                memset(buffer, 0, sizeof(buffer));
                
                index = 0;
            } else {
                buffer[index] = (char) byte;
                index++;
                print_char((char) byte);
            }
        }
    }
}

