package f;

import java.io.FileReader;
import java.io.IOException;

public class DriverTest {
    public static void main(String args[]) throws IOException {
        FileReader f=new FileReader(args[0]+"/config/files/x.txt");
        int c=f.read();
        assert c=='x' : ""+c;
        System.out.println("PASS");
    }
}
