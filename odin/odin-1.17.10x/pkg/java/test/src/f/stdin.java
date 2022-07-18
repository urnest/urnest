package f;

public class stdin {
    public static void main(String[] args) throws Exception {
        int s = System.in.read();
        while(s != -1) {
            System.out.print((char)s);
            s = System.in.read();
        }
        System.out.flush();
    }
}
