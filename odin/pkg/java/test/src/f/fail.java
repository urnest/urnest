package f;

public class fail {
    public static void main(String[] args) throws Exception {
        System.out.println("out");
        System.out.flush();
        System.err.println("err");
        System.exit(3);
    }
}
